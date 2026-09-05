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
#include <libavutil/mathematics.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}

// Base include

#include "audio.h"
#include "../common/dstack.h"


struct FFMPEG_AUDIO_FRAME {
	FFMPEG_AUDIO_FRAME(int Framesize,int64_t Time);
	FFMPEG_AUDIO_FRAME(int Framesize);
	~FFMPEG_AUDIO_FRAME();
	unsigned char *buffer;
	unsigned int size;
	int64_t pts;
};

class AudioFFMPEG : public AudioBuffer{
	private:
		// Callbacks and utilities
		static void log_callback(void *ptr,int lvl,const char *fmt,va_list vl);
		static int read_callback(void *rwops,uint8_t *buf,int size);
		static int write_callback(void *rwops,uint8_t *buf,int size);
		static int64_t seek_callback(void *rwops,int64_t offset,int whence);
		AVPacket *GetPacket();
		bool GetFrame();
		bool Seek(uint64_t Offset);

#if LIBAVCODEC_VERSION_MAJOR>=53
		AVIOContext *pb;					//!< Custom IO interface
		AVFrame *decoded_frame;
#else
		ByteIOContext *pb;					//!< Custom IO interface
#endif
		RWops *ops;							//!< Holds source data
		DStack framebuffer;					//!< Stacks formatted frames
		SDL_mutex *datalock;				//!< Mutex for controldata
		SDL_mutex *framelock;				//!< Mutex for framebuffer
    	AVFormatContext *FormatContext;		//!< Holds FFMPEG data
		AVStream *stream;					//!< Audio stream to decode
		struct SwrContext *resampler;
		enum AVSampleFormat fmt;
	//	ReSampleContext *resampler;			//!< Context for resampling output
		AVPacket *packetcache;				//!< Intermediary packet
		FFMPEG_AUDIO_FRAME *framecache;		//!< Intermediary frame
		Uint8 *resamplebuffer;				//!< Buffer for resampling data
		uint64_t packettime;				//!< Time of intermediary packet
		uint64_t mintime;					//!< Min timestamp to decode
		int packetsize;						//!< Size of intermediary packet
		int index;							//!< Index of audio stream
		bool Process(time_t Time,
				unsigned char *Buffer,
				unsigned int Size);
		void CloseData();
		void CloseFrames();
	public:
		AudioFFMPEG();
		~AudioFFMPEG();
		virtual bool Open(RWops *Resource);
		virtual void Mix(unsigned char *Buffer,int Length,double Volume);
		virtual bool Decode();
		virtual void Close();
};

#endif

