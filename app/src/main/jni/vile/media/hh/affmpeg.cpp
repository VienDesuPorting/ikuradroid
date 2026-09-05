/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "affmpeg.h"


int AudioFFMPEG::packet_queue_put_private(PacketQueue *q, AVPacket *pkt)
{
    MyAVPacketList *pkt1;

    if (q->abort_request)
       return -1;

    pkt1 = (MyAVPacketList *)av_malloc(sizeof(MyAVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;
    if (pkt == &flush_pkt)
        q->serial++;
    pkt1->serial = q->serial;

    if (!q->last_pkt)
        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->nb_packets++;
    q->size += pkt1->pkt.size + sizeof(*pkt1);
    /* XXX: should duplicate packet data in DV case */
    SDL_CondSignal(q->cond);
    return 0;
}

int AudioFFMPEG::packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    int ret;

    /* duplicate the packet */
    if (pkt != &flush_pkt && av_dup_packet(pkt) < 0)
        return -1;

    SDL_LockMutex(q->mutex);
    ret = packet_queue_put_private(q, pkt);
    SDL_UnlockMutex(q->mutex);

    if (pkt != &flush_pkt && ret < 0)
        av_free_packet(pkt);

    return ret;
}

int AudioFFMPEG::packet_queue_put_nullpacket(PacketQueue *q, int stream_index)
{
    AVPacket pkt1, *pkt = &pkt1;
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;
    pkt->stream_index = stream_index;
    return packet_queue_put(q, pkt);
}

/* packet queue handling */
void AudioFFMPEG::packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    q->mutex = SDL_CreateMutex();
    q->cond = SDL_CreateCond();
    q->abort_request = 1;
}

void AudioFFMPEG::packet_queue_flush(PacketQueue *q)
{
    MyAVPacketList *pkt, *pkt1;

    SDL_LockMutex(q->mutex);
    for (pkt = q->first_pkt; pkt; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    q->size = 0;
    SDL_UnlockMutex(q->mutex);
}

void AudioFFMPEG::packet_queue_destroy(PacketQueue *q)
{
    packet_queue_flush(q);
    SDL_DestroyMutex(q->mutex);
    SDL_DestroyCond(q->cond);
}

void AudioFFMPEG::packet_queue_abort(PacketQueue *q)
{
    SDL_LockMutex(q->mutex);

    q->abort_request = 1;

    SDL_CondSignal(q->cond);

    SDL_UnlockMutex(q->mutex);
}

void AudioFFMPEG::packet_queue_start(PacketQueue *q)
{
    SDL_LockMutex(q->mutex);
    q->abort_request = 0;
    packet_queue_put_private(q, &flush_pkt);
    SDL_UnlockMutex(q->mutex);
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
int AudioFFMPEG::packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
    MyAVPacketList *pkt1;
    int ret;

    SDL_LockMutex(q->mutex);

    for (;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size + sizeof(*pkt1);
            *pkt = pkt1->pkt;
            if (serial)
                *serial = pkt1->serial;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            SDL_CondWait(q->cond, q->mutex);
        }
    }
    SDL_UnlockMutex(q->mutex);
    return ret;
}


int AudioFFMPEG::decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub) {
    int got_frame = 0;

    do {
        int ret = -1;

        if (d->queue->abort_request)
            return -1;

        if (!d->packet_pending || d->queue->serial != d->pkt_serial) {
            AVPacket pkt;
            do {
                if (d->queue->nb_packets == 0)
                    SDL_CondSignal(d->empty_queue_cond);
                if (packet_queue_get(d->queue, &pkt, 1, &d->pkt_serial) < 0)
                    return -1;
                if (pkt.data == flush_pkt.data) {
                    avcodec_flush_buffers(d->avctx);
                    d->finished = 0;
                    d->next_pts = d->start_pts;
                    d->next_pts_tb = d->start_pts_tb;
                }
            } while (pkt.data == flush_pkt.data || d->queue->serial != d->pkt_serial);
            av_free_packet(&d->pkt);
            d->pkt_temp = d->pkt = pkt;
            d->packet_pending = 1;
        }

        switch (d->avctx->codec_type) {
            case AVMEDIA_TYPE_AUDIO:
                ret = avcodec_decode_audio4(d->avctx, frame, &got_frame, &d->pkt_temp);
                if (got_frame) {
                    AVRational tb = (AVRational){1, frame->sample_rate};
                    if (frame->pts != AV_NOPTS_VALUE)
                        frame->pts = av_rescale_q(frame->pts, d->avctx->time_base, tb);
                    else if (frame->pkt_pts != AV_NOPTS_VALUE)
                        frame->pts = av_rescale_q(frame->pkt_pts, av_codec_get_pkt_timebase(d->avctx), tb);
                    else if (d->next_pts != AV_NOPTS_VALUE)
                        frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
                    if (frame->pts != AV_NOPTS_VALUE) {
                        d->next_pts = frame->pts + frame->nb_samples;
                        d->next_pts_tb = tb;
                    }
                }
                break;
        }

        if (ret < 0) {
            d->packet_pending = 0;
        } else {
            d->pkt_temp.dts =
            d->pkt_temp.pts = AV_NOPTS_VALUE;
            if (d->pkt_temp.data) {
                if (d->avctx->codec_type != AVMEDIA_TYPE_AUDIO)
                    ret = d->pkt_temp.size;
                d->pkt_temp.data += ret;
                d->pkt_temp.size -= ret;
                if (d->pkt_temp.size <= 0)
                    d->packet_pending = 0;
            } else {
                if (!got_frame) {
                    d->packet_pending = 0;
                    d->finished = d->pkt_serial;
                }
            }
        }
    } while (!got_frame && !d->finished);

    return got_frame;
}


void AudioFFMPEG::frame_queue_unref_item(Frame *vp)
{
    av_frame_unref(vp->frame);
}

int AudioFFMPEG::frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last)
{
    int i;
    memset(f, 0, sizeof(FrameQueue));
    if (!(f->mutex = SDL_CreateMutex()))
        return AVERROR(ENOMEM);
    if (!(f->cond = SDL_CreateCond()))
        return AVERROR(ENOMEM);
    f->pktq = pktq;
    f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
    f->keep_last = !!keep_last;
    for (i = 0; i < f->max_size; i++)
        if (!(f->queue[i].frame = av_frame_alloc()))
            return AVERROR(ENOMEM);
    return 0;
}

void AudioFFMPEG::frame_queue_destory(FrameQueue *f)
{
    SDL_DestroyMutex(f->mutex);
    SDL_DestroyCond(f->cond);
}

void AudioFFMPEG::frame_queue_signal(FrameQueue *f)
{
    SDL_LockMutex(f->mutex);
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}

Frame *AudioFFMPEG::frame_queue_peek_writable(FrameQueue *f)
{
    /* wait until we have space to put a new frame */
    SDL_LockMutex(f->mutex);
    while (f->size >= f->max_size &&
           !f->pktq->abort_request) {
        SDL_CondWait(f->cond, f->mutex);
    }
    SDL_UnlockMutex(f->mutex);

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[f->windex];
}

Frame *AudioFFMPEG::frame_queue_peek_readable(FrameQueue *f)
{
    /* wait until we have a readable a new frame */
    SDL_LockMutex(f->mutex);
    while (f->size - f->rindex_shown <= 0 &&
           !f->pktq->abort_request) {
        SDL_CondWait(f->cond, f->mutex);
    }
    SDL_UnlockMutex(f->mutex);

    if (f->pktq->abort_request)
        return NULL;

    return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

void AudioFFMPEG::frame_queue_push(FrameQueue *f)
{
    if (++f->windex == f->max_size)
        f->windex = 0;
    SDL_LockMutex(f->mutex);
    f->size++;
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}

void AudioFFMPEG::frame_queue_next(FrameQueue *f)
{
    if (f->keep_last && !f->rindex_shown) {
        f->rindex_shown = 1;
        return;
    }
    frame_queue_unref_item(&f->queue[f->rindex]);
    if (++f->rindex == f->max_size)
        f->rindex = 0;
    SDL_LockMutex(f->mutex);
    f->size--;
    SDL_CondSignal(f->cond);
    SDL_UnlockMutex(f->mutex);
}


/* return the number of undisplayed frames in the queue */
int AudioFFMPEG::frame_queue_nb_remaining(FrameQueue *f)
{
    return f->size - f->rindex_shown;
}



void AudioFFMPEG::decoder_abort(Decoder *d, FrameQueue *fq)
{
    packet_queue_abort(d->queue);
    frame_queue_signal(fq);
    SDL_WaitThread(d->decoder_tid, NULL);
    d->decoder_tid = NULL;
    packet_queue_flush(d->queue);
}



/* seek in the stream */
void AudioFFMPEG::stream_seek( int64_t pos, int64_t rel, int seek_by_bytes)
{
    if (!is->seek_req) {
        is->seek_pos = pos;
        is->seek_rel = rel;
        is->seek_flags &= ~AVSEEK_FLAG_BYTE;
        if (seek_by_bytes)
            is->seek_flags |= AVSEEK_FLAG_BYTE;
        is->seek_req = 1;
        SDL_CondSignal(is->continue_read_thread);
    }
}


int AudioFFMPEG::audio_thread(void *arg)
{
	LogError("audio_thread");
    AudioFFMPEG *a = (AudioFFMPEG *)arg;

    AVFrame *frame = av_frame_alloc();
    Frame *af;
    int got_frame = 0;
    AVRational tb;
    int ret = 0;

  //*
    if (!frame)
        return AVERROR(ENOMEM);

    do {
        if ((got_frame = a->decoder_decode_frame(&a->is->auddec, frame, NULL)) < 0)
        {
                av_frame_free(&frame);
                return ret;
        }

        if (got_frame) {
                tb = (AVRational){1, frame->sample_rate};


                if (!(af = a->frame_queue_peek_writable(&a->is->sampq)))
                {
                    av_frame_free(&frame);
                    return ret;
                }

                af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
                af->pos = av_frame_get_pkt_pos(frame);
                af->serial = a->is->auddec.pkt_serial;
                af->duration = av_q2d((AVRational){frame->nb_samples, frame->sample_rate});

                av_frame_move_ref(af->frame, frame);
                a->frame_queue_push(&a->is->sampq);


        }
    } while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);


    av_frame_free(&frame);
    return ret;
}


/**
 * Decode one audio frame and return its uncompressed size.
 *
 * The processed audio frame is decoded, converted if required, and
 * stored in is->audio_buf, with size in bytes given by the return
 * value.
 */
int AudioFFMPEG::audio_decode_frame()
{
    int data_size, resampled_data_size;
    int64_t dec_channel_layout;
    av_unused double audio_clock0;
    int wanted_nb_samples;
    Frame *af;

  //*
    if (is->paused)
        return -1;

    do {
        if (!(af = frame_queue_peek_readable(&is->sampq)))
            return -1;
        frame_queue_next(&is->sampq);
    } while (af->serial != is->audioq.serial);

    data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(af->frame),
                                           af->frame->nb_samples,
                                           (AVSampleFormat)af->frame->format, 1);

    dec_channel_layout =
        (af->frame->channel_layout && av_frame_get_channels(af->frame) == av_get_channel_layout_nb_channels(af->frame->channel_layout)) ?
        af->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af->frame));
    wanted_nb_samples =  af->frame->nb_samples;

    if (af->frame->format        != is->audio_src.fmt            ||
        dec_channel_layout       != is->audio_src.channel_layout ||
        af->frame->sample_rate   != is->audio_src.freq           ||
        (wanted_nb_samples       != af->frame->nb_samples && !is->swr_ctx)) {
        swr_free(&is->swr_ctx);
        is->swr_ctx = swr_alloc_set_opts(NULL,
                                         is->audio_tgt.channel_layout, is->audio_tgt.fmt, is->audio_tgt.freq,
                                         dec_channel_layout,(AVSampleFormat)af->frame->format, af->frame->sample_rate,
                                         0, NULL);
        if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
            av_log(NULL, AV_LOG_ERROR,
                   "Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
                    af->frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)af->frame->format), av_frame_get_channels(af->frame),
                    is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.channels);
            swr_free(&is->swr_ctx);
            return -1;
        }
        is->audio_src.channel_layout = dec_channel_layout;
        is->audio_src.channels       = av_frame_get_channels(af->frame);
        is->audio_src.freq = af->frame->sample_rate;
        is->audio_src.fmt = (AVSampleFormat)af->frame->format;
    }

    if (is->swr_ctx) {
        const uint8_t **in = (const uint8_t **)af->frame->extended_data;
        uint8_t **out = &is->audio_buf1;
        int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate + 256;
        int out_size  = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, out_count, is->audio_tgt.fmt, 0);
        int len2;
        if (out_size < 0) {
            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
            return -1;
        }
        if (wanted_nb_samples != af->frame->nb_samples) {
            if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - af->frame->nb_samples) * is->audio_tgt.freq / af->frame->sample_rate,
                                        wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate) < 0) {
                av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
                return -1;
            }
        }
        av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
        if (!is->audio_buf1)
            return AVERROR(ENOMEM);
        len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);
        if (len2 < 0) {
            av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
            return -1;
        }
        if (len2 == out_count) {
            av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
            if (swr_init(is->swr_ctx) < 0)
                swr_free(&is->swr_ctx);
        }
        is->audio_buf = is->audio_buf1;
        resampled_data_size = len2 * is->audio_tgt.channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
    } else {
        is->audio_buf = af->frame->data[0];
        resampled_data_size = data_size;
    }

    audio_clock0 = is->audio_clock;

    if (!isnan(af->pts))
        is->audio_clock = af->pts + (double) af->frame->nb_samples / af->frame->sample_rate;
    else
        is->audio_clock = NAN;
    is->audio_clock_serial = af->serial;
//*/
    return resampled_data_size;
}




/* open a given stream. Return 0 if OK */
int AudioFFMPEG::stream_component_open( int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;
    AVCodec *codec;

    AVDictionaryEntry *t = NULL;
    int sample_rate, nb_channels;
    int64_t channel_layout;
    int ret = 1;
    int stream_lowres = 0;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return -1;
    avctx = ic->streams[stream_index]->codec;

    codec = avcodec_find_decoder(avctx->codec_id);

    switch(avctx->codec_type){
        case AVMEDIA_TYPE_AUDIO   : is->last_audio_stream    = stream_index;  break;
    }

    if (!codec) {
        av_log(NULL, AV_LOG_WARNING, "No codec could be found with id %d\n", avctx->codec_id);
        return -1;
    }

    avctx->codec_id = codec->id;
    if(stream_lowres > av_codec_get_max_lowres(codec)){
        av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
                av_codec_get_max_lowres(codec));
        stream_lowres = av_codec_get_max_lowres(codec);
    }
    av_codec_set_lowres(avctx, stream_lowres);

    if ((ret = avcodec_open2(avctx, codec, 0)) < 0)
         return -1;


    is->eof = 0;
    ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;

    if (avctx->codec_type==AVMEDIA_TYPE_AUDIO){
        sample_rate    = avctx->sample_rate;
        nb_channels    = avctx->channels;
        channel_layout = avctx->channel_layout;

        int64_t wanted_channel_layout = av_get_default_channel_layout(nb_channels);
        is->audio_tgt.fmt = AV_SAMPLE_FMT_S16;
        is->audio_tgt.freq = Cfg::Audio::Frequency;
        is->audio_tgt.channel_layout = wanted_channel_layout;
        is->audio_tgt.channels =  Cfg::Audio::Channels;
        is->audio_tgt.frame_size = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, 1, is->audio_tgt.fmt, 1);
        is->audio_tgt.bytes_per_sec = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, is->audio_tgt.freq, is->audio_tgt.fmt, 1);

        if (is->audio_tgt.bytes_per_sec <= 0 || is->audio_tgt.frame_size <= 0) {
            av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
            return -1;
        }
        /* prepare audio output */

        is->audio_hw_buf_size = ret;
        is->audio_src = is->audio_tgt;
        is->audio_buf_size  = 0;
        is->audio_buf_index = 0;

        /* init averaging filter */
        is->audio_diff_avg_coef  = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
        is->audio_diff_avg_count = 0;
        /* since we do not have a precise anough audio fifo fullness,
           we correct audio sync only if larger than this threshold */
        is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

        is->audio_stream = stream_index;
        is->audio_st = ic->streams[stream_index];


        is->auddec.avctx = avctx;
        is->auddec.queue = &is->audioq;
        is->auddec.empty_queue_cond = is->continue_read_thread;
        is->auddec.start_pts = AV_NOPTS_VALUE;

        if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
            is->auddec.start_pts = is->audio_st->start_time;
            is->auddec.start_pts_tb = is->audio_st->time_base;
        }

        packet_queue_start(is->auddec.queue);
        is->auddec.decoder_tid = SDL_CreateThread(audio_thread,"audio", this);
        SDL_PauseAudio(0);

    }

    return ret;
}

void AudioFFMPEG::stream_component_close( int stream_index)
{
    AVFormatContext *ic = is->ic;
    AVCodecContext *avctx;

    if (stream_index < 0 || stream_index >= ic->nb_streams)
        return;
    avctx = ic->streams[stream_index]->codec;

    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        decoder_abort(&is->auddec, &is->sampq);
       // SDL_CloseAudio();

        av_free_packet(&is->auddec.pkt);
        swr_free(&is->swr_ctx);
        av_freep(&is->audio_buf1);
        is->audio_buf1_size = 0;
        is->audio_buf = NULL;


        break;

    default:
        break;
    }

    ic->streams[stream_index]->discard = AVDISCARD_ALL;
    avcodec_close(avctx);
    switch (avctx->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        is->audio_st = NULL;
        is->audio_stream = -1;
        break;

    default:
        break;
    }
}

AudioFFMPEG::AudioFFMPEG() : AudioBuffer() {
	// Unset variables

	ops=0;
    is = 0;
    wait_mutex = 0;

    start_time = AV_NOPTS_VALUE;
    duration = AV_NOPTS_VALUE;
   // infinite_buffer = -1;
    loop = 1;
}

AudioFFMPEG::~AudioFFMPEG(){
	Close();
}

// Implement reading from blobs
int AudioFFMPEG::read_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Read(buf,size);
}

// Implement writing from blobs
int AudioFFMPEG::write_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Write(buf,size);
}

// Implement seeking from blobs
int64_t AudioFFMPEG::seek_callback(void *rwops,int64_t offset,int whence){
	RWops *ops=(RWops*)rwops;
	if(whence==AVSEEK_SIZE){
		return ops->Size();
	}
	else{
		return ops->Seek(offset,whence);
	}
}

// Passes ffmpeg log output to our verbose output
void AudioFFMPEG::log_callback(void *ptr,int level,const char* fmt,va_list vl){
	const int size=255;
	static char buffer1[size];
	static char buffer2[size];
	int cnt=vsnprintf(buffer1,size,fmt,vl);
	if(cnt>1){
		buffer1[size-1]=0;
		cnt=sprintf(buffer2,"FFMPEG: %s",buffer1);
		if(cnt>1){
			buffer2[cnt-1]=0;
			LogVerbose(buffer2);
		}
	}
}


/*! \brief Closes and flushes all currently opened data
 */
void AudioFFMPEG::Close(){


    if (ops)
	{
        ops->Close();
        delete ops;
        ops = 0;
	}

   if (is)
   {
        if (is->audio_stream >= 0)
            stream_component_close(is->audio_stream);
        if (is->ic) {

            avformat_close_input(&is->ic);
            is->ic = NULL;
        }

        is->abort_request = 1;
//        SDL_WaitThread(is->read_tid, NULL);
//        packet_queue_destroy(&is->audioq);
//       frame_queue_destory(&is->sampq);
//        SDL_DestroyCond(is->continue_read_thread);
        av_free(is);
    }//*/

 //if (wait_mutex)
//    SDL_DestroyMutex(wait_mutex);

}


/*! \brief Asserts that a sufficent number of frames are cached and ready
 *  \return True if anything was decoded
 *
 *  This method must be called regularly, preferably from a traditional
 *  decoding thread. This method returning false means that the calling
 *  thread can relax for a while as the stream is emty or the buffer is
 *  full.
 */
bool AudioFFMPEG::Decode(){
	// Get current framebuffer count
	//LogError("Decode");
	bool retval=false;
    int pkt_in_play_range = 0;
	AVPacket pkt1, *pkt = &pkt1;
	int64_t pkt_ts;
    int ret;
    int64_t stream_start_time;

        if (is->abort_request)
            return retval;
        if (is->paused != is->last_paused) {
            is->last_paused = is->paused;
            if (is->paused)
                is->read_pause_return = av_read_pause(is->ic);
            else
                av_read_play(is->ic);
        }


        if (is->seek_req) {
            int64_t seek_target = is->seek_pos;
            int64_t seek_min    = is->seek_rel > 0 ? seek_target - is->seek_rel + 2: INT64_MIN;
            int64_t seek_max    = is->seek_rel < 0 ? seek_target - is->seek_rel - 2: INT64_MAX;

            ret = avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR,
                       "%s: error while seeking\n", is->ic->filename);
            } else {
                if (is->audio_stream >= 0) {
                    packet_queue_flush(&is->audioq);
                    packet_queue_put(&is->audioq, &flush_pkt);
                }
            }
            is->seek_req = 0;
            is->queue_attachments_req = 1;
            is->eof = 0;
//            if (is->paused)
//                step_to_next_frame(is);
        }
        if (is->queue_attachments_req) {
            is->queue_attachments_req = 0;
        }

       /* if (infinite_buffer<1 &&
              (is->audioq.size > MAX_QUEUE_SIZE
            || (   (is->audioq   .nb_packets > MIN_FRAMES || is->audio_stream < 0 || is->audioq.abort_request)))) {

            SDL_LockMutex(wait_mutex);
            SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
            SDL_UnlockMutex(wait_mutex);
            return retval;
//            continue;
        }*/
        if (!is->paused &&
            (!is->audio_st || (is->auddec.finished == is->audioq.serial && frame_queue_nb_remaining(&is->sampq) == 0)))
            {
            if (loop != 1 && (!loop || --loop)) {
                stream_seek( start_time != AV_NOPTS_VALUE ? start_time : 0, 0, 0);
            }
        }

        ret = av_read_frame(is->ic, pkt);
        if (ret < 0) {
            if ((ret == AVERROR_EOF || avio_feof(is->ic->pb)) && !is->eof) {

                if (is->audio_stream >= 0)
                    packet_queue_put_nullpacket(&is->audioq, is->audio_stream);

                is->eof = 1;
            }
            if (is->ic->pb && is->ic->pb->error)
                return retval;
            SDL_LockMutex(wait_mutex);
            SDL_CondWaitTimeout(is->continue_read_thread, wait_mutex, 10);
            SDL_UnlockMutex(wait_mutex);
             retval = true;
            return retval;
        } else {
            is->eof = 0;
        }

        stream_start_time = is->ic->streams[pkt->stream_index]->start_time;
        pkt_ts = pkt->pts == AV_NOPTS_VALUE ? pkt->dts : pkt->pts;
        pkt_in_play_range = duration == AV_NOPTS_VALUE ||
                (pkt_ts - (stream_start_time != AV_NOPTS_VALUE ? stream_start_time : 0)) *
                av_q2d(is->ic->streams[pkt->stream_index]->time_base) -
                (double)(start_time != AV_NOPTS_VALUE ? start_time : 0) / 1000000
                <= ((double)duration / 1000000);
        if (pkt->stream_index == is->audio_stream && pkt_in_play_range) {
            packet_queue_put(&is->audioq, pkt);

        } else {
            av_free_packet(pkt);
        }
    retval = true;
	return retval;
}

/*! \brief Mixes decoded data to the output audio stream
 *
 *  This method should typically be called from within the applications audio
 *  callback. Cached frames should always be cached with the same bitrate and
 *  characteristics as the audio output, so the data can be copied directly
 *  without any further conversion.
 */
void AudioFFMPEG::Mix(unsigned char *Output,int Length,double Volume){

    int audio_size, len1;

    while (Length > 0) {
        if (is->audio_buf_index >= is->audio_buf_size) {
           audio_size = audio_decode_frame();
           if (audio_size < 0) {
               is->audio_buf      = is->silence_buf;
               is->audio_buf_size = sizeof(is->silence_buf) / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
           } else {
               is->audio_buf_size = audio_size;
           }
           is->audio_buf_index = 0;
        }
        len1 = is->audio_buf_size - is->audio_buf_index;
        if (len1 > Length)
            len1 = Length;
        memcpy(Output, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
        Length -= len1;
        Output += len1;
        is->audio_buf_index += len1;
    }
    is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
}

/*! \brief Opens and identifies an audio resouce from a RWops source
 *  \param Resource Source data
 *  \return True if Resource was identified as a valid audio resource
 */
bool AudioFFMPEG::Open(RWops *Resource){
	Close();

    is = (VideoState *)av_mallocz(sizeof(VideoState));
    if (!is)
        return false;

    av_strlcpy(is->filename, "RWops", sizeof(is->filename));

    if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
        return false;

    packet_queue_init(&is->audioq);
    is->continue_read_thread = SDL_CreateCond();

    AVFormatContext *ic = NULL;
    int  i, ret;
    int st_index[AVMEDIA_TYPE_NB];

    wait_mutex = SDL_CreateMutex();

    memset(st_index, -1, sizeof(st_index));
    is->last_audio_stream = is->audio_stream = -1;
    is->eof = 0;

    ic = avformat_alloc_context();
    if (!ic) {
        av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
        ret = AVERROR(ENOMEM);
        return false;
    }

	ops=new RWops();
	if(!ops->OpenRW(Resource,false)){
		LogError("Failed to copy blob");
		Close();
		 return false;
	}

    unsigned int iosize=2048+FF_INPUT_BUFFER_PADDING_SIZE;
	unsigned char *iobuffer=(unsigned char*)malloc(iosize);
	memset(iobuffer,0,iosize);
	AVIOContext *pb=avio_alloc_context(iobuffer,iosize,0,ops,
			read_callback,write_callback,seek_callback);

	AVProbeData probe_data;
    memset(&probe_data, 0, sizeof(AVProbeData));
	probe_data.filename="";
	int probesize=(1024*4)+AVPROBE_PADDING_SIZE;
	if(ops->Size()<probesize){
		probesize=ops->Size()+AVPROBE_PADDING_SIZE;
	}
	probe_data.buf_size=probesize;
	probe_data.buf=(unsigned char*)av_malloc(probesize*sizeof(unsigned char));
    memset(probe_data.buf,0,probesize);
	ops->Read(probe_data.buf,probesize);
	ops->Seek(0,SEEK_SET);

    AVInputFormat *fmt=av_probe_input_format(&probe_data,1);

	free(probe_data.buf);
	probe_data.buf=0;

	if (!fmt){
	    free(iobuffer);
        LogVerbose("FFMPEG could not identify unknown media");
		Close();
		return false;
	}
	//fmt->flags|=AVFMT_NOFILE;

    is->iformat = fmt;
    ic->pb=pb;

    if (avformat_open_input(&ic, is->filename, is->iformat, 0) < 0)
        return false;
    is->ic = ic;

    if (ic->pb)
        ic->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

    av_dump_format(ic, 0, is->filename, 0);

    const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = {0};

    for (i = 0; i < ic->nb_streams; i++) {
        AVStream *st = ic->streams[i];
        enum AVMediaType type = st->codec->codec_type;
        st->discard = AVDISCARD_ALL;
        if (wanted_stream_spec[type] && st_index[type] == -1)
            if (avformat_match_stream_specifier(ic, st, wanted_stream_spec[type]) > 0)
                st_index[type] = i;
    }
    for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
        if (wanted_stream_spec[i] && st_index[i] == -1) {
            av_log(NULL, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", wanted_stream_spec[i], av_get_media_type_string((AVMediaType)i));
            st_index[i] = INT_MAX;
        }
    }

    st_index[AVMEDIA_TYPE_AUDIO] =
            av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO,
                                st_index[AVMEDIA_TYPE_AUDIO],
                                st_index[AVMEDIA_TYPE_VIDEO],
                                NULL, 0);


    if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
        stream_component_open(st_index[AVMEDIA_TYPE_AUDIO]);
    }


    if ( is->audio_stream < 0) {
        av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n",
               is->filename);
        return false;
    }

     return true;
}


