/*! \class VideoFFMPEG
 *	\brief VideoFFMPEG implements libav/ffmpeg on top of a VideoBuffer object.
 */

#ifndef _VFFMPEG_H_
#define _VFFMPEG_H_

#include "video.h"
#include "affmpeg.h"
#include "../common/edl_texture.h"

#include <SDL.h>


struct FFMPEG_VIDEO_FRAME {					//!< Encapsulates a buffered frame
	int64_t pts;							//!< FFMPEG timestamp
	EDLTexture* Texture;				//!< RGB surface
	FFMPEG_VIDEO_FRAME();
	~FFMPEG_VIDEO_FRAME();
};


class VideoFFMPEG : public VideoBuffer{
	private:
		// Callbacks and utilities
		static int read_callback(void *rwops,uint8_t *buf,int size);
		static int write_callback(void *rwops,uint8_t *buf,int size);
		static int64_t seek_callback(void *rwops,int64_t offset,int whence);
		static void yuv420_to_yuyv422(FFMPEG_VIDEO_FRAME *Frame);
		static void yuv_to_rgb(FFMPEG_VIDEO_FRAME *Frame);
		AVPacket *GetPacket();
		bool GetFrame();
		void CloseData();
		void CloseFrames();

#if LIBAVCODEC_VERSION_MAJOR>=53
		AVIOContext *pb;					//!< Custom IO interface
#else
		ByteIOContext *pb;					//!< Custom IO interface
#endif
		RWops *ops;							//!< Holds source data
		DStack framebuffer;					//!< Stacks formatted frames
		SDL_mutex *datalock;				//!< Mutex for controldata
		SDL_mutex *framelock;				//!< Mutex for framebuffer
    	AVFormatContext *FormatContext;		//!< Holds FFMPEG data
		SwsContext *yuycontext;				//!< YUY2 Conversion context
		SwsContext *rgbcontext;				//!< RGB Conversion context
		AVStream *stream;					//!< Audio stream to decode
		AVPacket *packetcache;				//!< Intermediary packet
		uint64_t packettime;				//!< Time of intermediary packet
		uint64_t mintime;					//!< Min timestamp to decode
		int packetsize;						//!< Size of intermediary packet
		int index;							//!< Index of audio stream
    	AVFrame *dframe;					//!< Intermediary decoding frame
    	AVFrame* pFrameYUV;
        EDLTexture* Texture;
	public:
		VideoFFMPEG();
		virtual ~VideoFFMPEG();

		// Engine interface
		virtual bool Open(RWops *Resource);
		virtual double Framerate();
		virtual bool Paint();
		virtual bool Decode();
		virtual void Close();
};

#endif

