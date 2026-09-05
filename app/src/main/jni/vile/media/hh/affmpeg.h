/*! \class AudioFFMPEG
 *	\brief AudioFFMPEG implements libav/ffmpeg on top of a AudioBuffer object.
 */

#ifndef _AFFMPEG_H_
#define _AFFMPEG_H_

// Some compiler trickery before including c library files
#ifndef INT64_C
#define INT64_C(c)              (c ## LL)
#endif
#ifndef UINT64_C
#define UINT64_C(c)             (c ## ULL)
#endif
extern "C" {
#include "libavutil/avstring.h"
#include "libavutil/eval.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"
}

// Base include
#include "audio.h"
#include "../common/dstack.h"


#define MAX_QUEUE_SIZE (15 * 1024 * 1024)
#define MIN_FRAMES 25
#define SDL_AUDIO_MIN_BUFFER_SIZE 512
#define SDL_AUDIO_MAX_CALLBACKS_PER_SEC 30
#define AUDIO_DIFF_AVG_NB   20
#define VIDEO_PICTURE_QUEUE_SIZE 3
#define SUBPICTURE_QUEUE_SIZE 16
#define SAMPLE_QUEUE_SIZE 9
#define FRAME_QUEUE_SIZE FFMAX(SAMPLE_QUEUE_SIZE, FFMAX(VIDEO_PICTURE_QUEUE_SIZE, SUBPICTURE_QUEUE_SIZE))

typedef struct MyAVPacketList {
    AVPacket pkt;
    struct MyAVPacketList *next;
    int serial;
} MyAVPacketList;

typedef struct PacketQueue {
    MyAVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    int serial;
    SDL_mutex *mutex;
    SDL_cond *cond;
} PacketQueue;


typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

typedef struct Frame {
    AVFrame *frame;
    int serial;
    double pts;           /* presentation timestamp for the frame */
    double duration;      /* estimated duration of the frame */
    int64_t pos;          /* byte position of the frame in the input file */
} Frame;

typedef struct FrameQueue {
    Frame queue[FRAME_QUEUE_SIZE];
    int rindex;
    int windex;
    int size;
    int max_size;
    int keep_last;
    int rindex_shown;
    SDL_mutex *mutex;
    SDL_cond *cond;
    PacketQueue *pktq;
} FrameQueue;

typedef struct Decoder {
    AVPacket pkt;
    AVPacket pkt_temp;
    PacketQueue *queue;
    AVCodecContext *avctx;
    int pkt_serial;
    int finished;
    int packet_pending;
    SDL_cond *empty_queue_cond;
    int64_t start_pts;
    AVRational start_pts_tb;
    int64_t next_pts;
    AVRational next_pts_tb;
    SDL_Thread *decoder_tid;
} Decoder;

typedef struct VideoState {
    SDL_Thread *read_tid;
    AVInputFormat *iformat;
    int abort_request;
    int paused;
    int last_paused;
    int queue_attachments_req;
    int seek_req;
    int seek_flags;
    int64_t seek_pos;
    int64_t seek_rel;
    int read_pause_return;
    AVFormatContext *ic;
    FrameQueue sampq;
    Decoder auddec;
    int audio_stream;
    int av_sync_type;
    double audio_clock;
    int audio_clock_serial;
    double audio_diff_cum; /* used for AV difference average computation */
    double audio_diff_avg_coef;
    double audio_diff_threshold;
    int audio_diff_avg_count;
    AVStream *audio_st;
    PacketQueue audioq;
    int audio_hw_buf_size;
    uint8_t silence_buf[SDL_AUDIO_MIN_BUFFER_SIZE];
    uint8_t *audio_buf;
    uint8_t *audio_buf1;
    unsigned int audio_buf_size; /* in bytes */
    unsigned int audio_buf1_size;
    int audio_buf_index; /* in bytes */
    int audio_write_buf_size;
    struct AudioParams audio_src;
    struct AudioParams audio_tgt;
    struct SwrContext *swr_ctx;
    int frame_drops_early;
    int frame_drops_late;
    double frame_timer;
    double frame_last_returned_time;
    double frame_last_filter_delay;
    double max_frame_duration;      // maximum duration of a frame - above this, we consider the jump a timestamp discontinuity
    SDL_Rect last_display_rect;
    int eof;
    char filename[1024];
    int step;
    int last_audio_stream;
    SDL_cond *continue_read_thread;
} VideoState;


class AudioFFMPEG : public AudioBuffer{
	private:
		// Callbacks and utilities
		static void log_callback(void *ptr,int lvl,const char *fmt,va_list vl);
		static int read_callback(void *rwops,uint8_t *buf,int size);
		static int write_callback(void *rwops,uint8_t *buf,int size);
		static int64_t seek_callback(void *rwops,int64_t offset,int whence);

        static int audio_thread(void *arg);

        int packet_queue_put_private(PacketQueue *q, AVPacket *pkt);
        int packet_queue_put(PacketQueue *q, AVPacket *pkt);
        int packet_queue_put_nullpacket(PacketQueue *q, int stream_index);
        void packet_queue_init(PacketQueue *q);
        void packet_queue_flush(PacketQueue *q);
        void packet_queue_destroy(PacketQueue *q);
        void packet_queue_abort(PacketQueue *q);
        void packet_queue_start(PacketQueue *q);
        int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial);
        void frame_queue_unref_item(Frame *vp);
        int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last);
        void frame_queue_destory(FrameQueue *f);
        void frame_queue_signal(FrameQueue *f);
        Frame *frame_queue_peek_writable(FrameQueue *f);
        Frame *frame_queue_peek_readable(FrameQueue *f);
        void frame_queue_push(FrameQueue *f);
        void frame_queue_next(FrameQueue *f);
        int frame_queue_nb_remaining(FrameQueue *f);

        int decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub);
        void decoder_abort(Decoder *d, FrameQueue *fq);
        void stream_seek(int64_t pos, int64_t rel, int seek_by_bytes);

        int stream_component_open( int stream_index);
        void stream_component_close( int stream_index);
		int audio_decode_frame();

        int64_t start_time;
        int64_t duration;
        int infinite_buffer;
        int loop;

        AVPacket flush_pkt;

        SDL_mutex *wait_mutex;
		RWops *ops;							//!< Holds source data
	public:
        VideoState *is;
		AudioFFMPEG();
		~AudioFFMPEG();

		virtual bool Open(RWops *Resource);
		virtual void Mix(unsigned char *Buffer,int Length,double Volume);
		virtual bool Decode();
		virtual void Close();
};

#endif

