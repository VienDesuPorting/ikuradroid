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

#include "vffmpeg.h"
#include "../common/edl_gfx.h"


VideoFFMPEG::VideoFFMPEG() : VideoBuffer() {
	// Unset variables
	yuycontext=0;
	rgbcontext=0;
	ops=0;
	pb=0;
	FormatContext=0;
	stream=0;
	index=-1;
	mintime=0;
	packetcache=0;
	packetsize=0;
	packettime=0;
	dframe=0;
    Texture=0;

	// Create objects
	datalock=SDL_CreateMutex();
	framelock=SDL_CreateMutex();
}

VideoFFMPEG::~VideoFFMPEG(){
	Close();
	SDL_DestroyMutex(datalock);
	SDL_DestroyMutex(framelock);
}

FFMPEG_VIDEO_FRAME::FFMPEG_VIDEO_FRAME(){
	pts=0;
	Texture=0;
}

FFMPEG_VIDEO_FRAME::~FFMPEG_VIDEO_FRAME(){
 if (Texture)
 {
      Texture->free();
      delete Texture;
 }

}

// Implement reading from blobs
int VideoFFMPEG::read_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Read(buf,size);
}

// Implement writing from blobs
int VideoFFMPEG::write_callback(void *rwops,uint8_t *buf,int size){
	return ((RWops*)rwops)->Write(buf,size);
}

// Implement seeking from blobs
int64_t VideoFFMPEG::seek_callback(void *rwops,int64_t offset,int whence){
	RWops *ops=(RWops*)rwops;
	if(whence==AVSEEK_SIZE){
		return ops->Size();
	}
	else{
		return ops->Seek(offset,whence);
	}
}

void VideoFFMPEG::Close(){
	CloseData();
	CloseFrames();
}

/*! \brief Closes and flushes all currently opened data
 */
void VideoFFMPEG::CloseFrames(){
	SDL_LockMutex(framelock);
	FFMPEG_VIDEO_FRAME *frame;
	while((frame=(FFMPEG_VIDEO_FRAME*)framebuffer.Pop())){
		delete frame;
	}
	SDL_UnlockMutex(framelock);
}

void VideoFFMPEG::CloseData(){
	SDL_LockMutex(datalock);

    if(Texture){
		Texture->free();
		Texture=0;
	}
	if(dframe){
		av_free(dframe);
		dframe=0;
	}
	if(packetcache){
		av_free_packet(packetcache);
		packetcache=0;
	}
	if(stream){
		avcodec_flush_buffers(stream->codec);
		avcodec_close(stream->codec);
		stream=0;
	}
	if(yuycontext){
		sws_freeContext(yuycontext);
		yuycontext=0;
	}
	if(rgbcontext){
		sws_freeContext(rgbcontext);
		rgbcontext=0;
	}
	if(FormatContext){
		// TODO: Loop through and close stream objects?
		//avcodec_close(FormatContext->streams[i]);
#if LIBAVCODEC_VERSION_MAJOR>=54
		avformat_close_input(&FormatContext);
#else
		av_close_input_stream(FormatContext);
		FormatContext=0;
#endif
	}
	if(pb){
		// See AudioFFMPEG implementation for details on this nightmare
		unsigned char *b=pb->buffer;
		pb->buffer=0;
		//avio_close(pb);
		free(pb);
		free(b);
		pb=0;
	}
	if(ops){
		delete ops;
		ops=0;
	}
	stream=0;
	index=-1;
	mintime=0;
	SDL_UnlockMutex(datalock);
}

/*! \brief Asserts that a sufficent number of frames are cached and ready
 *  \return True if anything was decoded
 *
 *  This method must be called regularly, preferably from a traditional
 *  decoding thread.
 */
bool VideoFFMPEG::Decode(){
	// Get current framebuffer count

	bool retval=false;
	SDL_LockMutex(framelock);
	int c=framebuffer.Count();
	SDL_UnlockMutex(framelock);
	if(c<Cfg::Video::Framecount && FormatContext){

		if(!(retval=GetFrame())){
			// Handle end-of-file
			CloseData();
		}
	}
	return retval;
}

/*! \brief Paints next frame to the display
 *  \return True if there is still remaining data
 */
bool VideoFFMPEG::Paint(){
	SDL_LockMutex(framelock);
	FFMPEG_VIDEO_FRAME *frame=(FFMPEG_VIDEO_FRAME*)framebuffer.Pop();
	SDL_UnlockMutex(framelock);
	if(frame){
	    if(frame->Texture){
            SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
            SDL_RenderClear(EDLRenderer);
            int w,h=0;
            SDL_RenderGetLogicalSize(EDLRenderer,&w,&h);
            SDL_Rect clipdest = {0,0,w,h};
            frame->Texture->render(0,&clipdest);
            SDL_RenderPresent(EDLRenderer);
		}
		delete frame;
	}
	return frame || FormatContext;
}

/*! \brief Opens and identifies an audio resouce from a RWops source
 *  \param Resource Source data
 *  \return True if Resource was identifies as a valid audio resource
 */
bool VideoFFMPEG::Open(RWops *Resource){
	// Close existing and copy input data
	Close();
	SDL_LockMutex(datalock);
	ops=new RWops();
	if(!ops->OpenRW(Resource,false)){
		LogError("Failed to copy blob");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
	}

	// Probe input format manually
	AVProbeData probe_data;
	probe_data.filename="blob";
	int probesize=1024*4;
	if(ops->Size()<probesize){
		probesize=ops->Size();
	}
	probe_data.buf_size=probesize+AVPROBE_PADDING_SIZE;
	probe_data.buf=(uint8_t*)malloc(probesize+AVPROBE_PADDING_SIZE);
	memset(probe_data.buf+probesize,0,AVPROBE_PADDING_SIZE);
	ops->Seek(0,SEEK_SET);
	ops->Read(probe_data.buf,probesize);
	ops->Seek(0,SEEK_SET);
	AVInputFormat *fmt=av_probe_input_format(&probe_data,1);
	free(probe_data.buf);
	probe_data.buf=0;
	if (!fmt){
        LogVerbose("FFMPEG could not identify unknown media");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
	}
	//fmt->flags|=AVFMT_NOFILE;
	// TODO: Free fmt at some point?


	// Create a custom stream io handler for the resource
#if LIBAVCODEC_VERSION_MAJOR>=53
	pb=(AVIOContext*)malloc(sizeof(AVIOContext));
	memset(pb,0,sizeof(AVIOContext));
#else
	pb=(ByteIOContext*)malloc(sizeof(ByteIOContext));
	memset(pb,0,sizeof(ByteIOContext));
#endif
	unsigned int iosize=2048+FF_INPUT_BUFFER_PADDING_SIZE;
	unsigned char *iobuffer=(unsigned char*)malloc(iosize);
	memset(iobuffer,0,iosize);

	//! \todo Upstream method init_put_byte is deprecated
//	init_put_byte(pb,iobuffer,iosize,0,ops,
//			read_callback,write_callback,seek_callback);

	pb=avio_alloc_context(iobuffer,iosize,0,ops,
			read_callback,write_callback,seek_callback);



	//! \todo Upstream method av_open_input_stream is deprecated
	//if(av_open_input_stream(&FormatContext,pb,"RWops",fmt,0)!=0){
		FormatContext->pb       = pb;
    if(avformat_open_input(&FormatContext,"RWops",fmt,0)!=0){
        LogError("FFMPEG failed to open input stream");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
    }

    // retrieve format information
#if LIBAVCODEC_VERSION_MAJOR>=52
    //if(av_find_stream_info(FormatContext)<0){
    if(avformat_find_stream_info(FormatContext,NULL)<0){
#else
	if(avformat_find_stream_info(FormatContext,NULL)<0){
#endif
        LogError("FFMPEG could not retrieve file info for stream");
		SDL_UnlockMutex(datalock);
		Close();
		return false;
    }

	// Select first audio stream, discard all others
	bool retval=false;
    for(unsigned int i=0;!retval && i<FormatContext->nb_streams;i++){
        FormatContext->streams[i]->discard=AVDISCARD_ALL;
#if LIBAVCODEC_VERSION_MAJOR>=53
		enum AVMediaType type=FormatContext->streams[i]->codec->codec_type;
		if(type==AVMEDIA_TYPE_VIDEO){
#else
		enum CodecType type=FormatContext->streams[i]->codec->codec_type;
		if(type==CODEC_TYPE_VIDEO){
#endif
			// Try to find a decoder for this stream
			enum AVCodecID id=FormatContext->streams[i]->codec->codec_id;
			AVCodec *codec=avcodec_find_decoder(id);
			if(!codec){
				LogError("Could not find video codec");
			}
#if LIBAVCODEC_VERSION_MAJOR>53
			else if(avcodec_open2(FormatContext->streams[i]->codec,codec,NULL)<0)
#else
			else if(avcodec_open(FormatContext->streams[i]->codec,codec)<0)
#endif
			{
				LogError("Failed to open video codec");
			}
			else{
				// Cache stream properties
				stream=FormatContext->streams[i];
        		stream->discard=AVDISCARD_DEFAULT;
				dframe=avcodec_alloc_frame();
				//screen=SDL_GetVideoSurface();
				LogVerbose("Opened ffmpeg video");
				retval=true;
				index=i;
				//Texture = new EDLTexture();
				//Texture->createBlank(stream->codec->width,stream->codec->height);
				//Texture->createVideoBlank(stream->codec->width,stream->codec->height);
			}
		}
	}

	// Check if any streams were found
	SDL_UnlockMutex(datalock);
	if(!retval){
		Close();
	}
    return retval;

}


/*! \brief  Returns the frame rate of the stream as a fraction.
 *  \returns The result of nominator / denominator
 */
double VideoFFMPEG::Framerate(){
	if(stream && stream->codec){
		return stream->r_frame_rate.num / stream->r_frame_rate.den;
	}
	else{
		return 0;
	}
}


/*! \brief Reads a single packet from the source data
 *  \return Read packet or NULL at error or eof
 */
AVPacket *VideoFFMPEG::GetPacket(){
	AVPacket *pack=(AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(pack);
	int read=av_read_frame(FormatContext,pack);
	if(read<0){
		// End of file
		av_free(pack);
		pack=0;
	}
	else{
		// Try to read packet
		if(av_dup_packet(pack)==0){
			if(pack->stream_index==index){
				return pack;
			}
		}

		// Read packet recursively
		av_free_packet(pack);
		pack=GetPacket();
	}
	return pack;
}

/*! \brief Reads a single frame from the video stream
 *  \return False at EOF or error
 */
bool VideoFFMPEG::GetFrame(){
	bool retval=false;

	SDL_LockMutex(datalock);
	if(FormatContext && stream && packetcache){
		// Get pack properties
		uint64_t time=av_rescale(packettime,
				stream->time_base.num,
				stream->time_base.den);
		if(time!=AV_NOPTS_VALUE && time<mintime){
			stream->codec->skip_frame=AVDISCARD_ALL;
		}
		else{
			stream->codec->skip_frame=AVDISCARD_DEFAULT;
		}

		if(packetsize>0){
			// Decode data from stream
			int gotframe=0;
#if ( ( LIBAVCODEC_VERSION_MAJOR <= 52 ) && ( LIBAVCODEC_VERSION_MINOR <= 20 ) )
			int used=avcodec_decode_video(stream->codec,dframe,&gotframe,
					pack->data,pack->size);
#else
			AVPacket p;
			av_init_packet(&p);
			p.data=packetcache->data+(packetcache->size-packetsize);
			p.size=packetsize;
			int used=avcodec_decode_video2(stream->codec,dframe,&gotframe,&p);
#endif
			if(used<0){
				LogError("Error decoding video frame");
				stream->codec->skip_frame=AVDISCARD_ALL;
				av_free_packet(packetcache);
				packetcache=0;
				packetsize=0;
				gotframe=0;
			}
			else{
				packetsize-=used;
			}

			// Post-process and buffer up decoded data
			if(gotframe && stream->codec->skip_frame!=AVDISCARD_ALL){

				// Create output frame
				FFMPEG_VIDEO_FRAME *frame=new FFMPEG_VIDEO_FRAME();
				frame->Texture = new EDLTexture();
				frame->Texture->createVideoBlank(stream->codec->width,stream->codec->height);

                pFrameYUV = avcodec_alloc_frame();
                int numBytes = avpicture_get_size(
                                PIX_FMT_YUV420P,//PIX_FMT_YUV420P,
                                stream->codec->width,
                                stream->codec->height);
                uint8_t* buffer = (uint8_t *)av_malloc( numBytes*sizeof(uint8_t) );

                avpicture_fill((AVPicture *)pFrameYUV, buffer, PIX_FMT_YUV420P,//PIX_FMT_YUV420P,
                                stream->codec->width, stream->codec->height);
				// Assert conversion contexts
				if(yuycontext==0){
printf("yuycontext create");
                    yuycontext=sws_getContext(
									stream->codec->width,
									stream->codec->height,
									stream->codec->pix_fmt,
                                    stream->codec->width,
									stream->codec->height,
								    PIX_FMT_YUV420P,
									SWS_BILINEAR,0,0,0);

                    printf("yuycontext create finish");
				}

				// Convert data to output format
				if(yuycontext){

					sws_scale(yuycontext,
							(const uint8_t* const*)dframe->data,
							dframe->linesize,0,stream->codec->height,
							 pFrameYUV->data,
                            pFrameYUV->linesize );
                    SDL_Rect sdlRect;
                    sdlRect.x = 0;
                    sdlRect.y = 0;
                    sdlRect.w = stream->codec->width;
                    sdlRect.h = stream->codec->height;
                    int size1 = sdlRect.w * sdlRect.h;

                   // frame->Texture->lockTexture();
                   // frame->Texture->copyPixelsVideo(pFrameYUV->data[0], pFrameYUV->data[1], pFrameYUV->data[2], size1);
                   // frame->Texture->unlockTexture();

                    frame->Texture->UpdateTexture( &sdlRect, pFrameYUV->data[0], pFrameYUV->linesize[0],
                                                      pFrameYUV->data[1], pFrameYUV->linesize[1],
                                                      pFrameYUV->data[2], pFrameYUV->linesize[2]);//*/


                   //if(frame->Texture)
                   //  frame->Texture->render(0,0);
                   // Texture->unlockTexture();
					//SDL_UnlockYUVOverlay(frame->overlay);
				}
				// Stack up frame
				SDL_LockMutex(framelock);
				framebuffer.Queue(frame);
				SDL_UnlockMutex(framelock);
				retval=true;
			}
		}
		else{
			// Free packet
			av_free_packet(packetcache);
			packetcache=0;
			packetsize=0;
		}
    }

	// Check if we need a new packet
	if(FormatContext && !packetcache){
		if((packetcache=GetPacket())){
			packetsize=packetcache->size;
			if(packetcache->dts==(unsigned int)AV_NOPTS_VALUE){
				if(packettime<(unsigned int)stream->start_time){
					packettime=stream->start_time;
				}
				packettime+=1000*packetcache->duration;
			}
			else{
				packettime=(packetcache->dts-stream->start_time)*1000;
			}
		}
	}

	// Check if we need recursive reads
	bool recall=(FormatContext && packetcache && !retval);
	SDL_UnlockMutex(datalock);
	if(recall){
		return GetFrame();
	}
	else{
		return retval;
	}
}

