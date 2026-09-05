/*! \class AudioFFMPEG
 *	\brief AudioFFMPEG implements libav/ffmpeg on top of a AudioBuffer object.
 */

#ifndef _AMIXER_H_
#define _AMIXER_H_


#include "audio.h"
#include "../common/dstack.h"


class AudioMixer : public AudioBuffer{
	private:
		// Callbacks and utilities
		static void log_callback(void *ptr,int lvl,const char *fmt,va_list vl);
		static int read_callback(void *rwops,uint8_t *buf,int size);
		static int write_callback(void *rwops,uint8_t *buf,int size);
		static int64_t seek_callback(void *rwops,int64_t offset,int whence);
		AVPacket *GetPacket();
		bool GetFrame();
		bool Seek(uint64_t Offset);

		RWops *ops;							//!< Holds source data
		DStack framebuffer;					//!< Stacks formatted frames
		SDL_mutex *datalock;				//!< Mutex for controldata
		SDL_mutex *framelock;				//!< Mutex for framebuffer

		uint64_t packettime;				//!< Time of intermediary packet
		uint64_t mintime;					//!< Min timestamp to decode
		int packetsize;						//!< Size of intermediary packet
		int index;							//!< Index of audio stream

		void CloseData();
		void CloseFrames();
	public:
		AudioMixer();
		~AudioMixer();
		virtual bool Open(RWops *Resource);
		virtual void Mix(unsigned char *Buffer,int Length,double Volume);
		virtual bool Decode();
		virtual void Close();
};

#endif

