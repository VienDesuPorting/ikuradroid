/*! \class AudioBuffer
 *  \brief AudioBuffer defines a generic audio decoder object.
 *
 *  AudioBuffer is the generic audio interface. EngineMixer loads and drives
 *  a derived object which provides a buffered audio feed through the virtual
 *  interface.
 */

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "../res/rwops.h"
#include "../common/log.h"

class AudioBuffer {
	protected:
		bool playing;			//!< Wether resource is currently playing
		int replays;			//!< How many times the resource should play
	public:
		AudioBuffer();
		virtual ~AudioBuffer();

		// Audio interface
		virtual void SetPlaying(bool Playing);
		virtual void SetReplays(int Replays);
		virtual bool GetPlaying();
		virtual int GetReplays();

		// Engine interface
		virtual bool Open(RWops *Resource)=0;
		virtual void Mix(unsigned char *Buffer,int Length,double Volume)=0;
		virtual bool Decode()=0;
		virtual void Close()=0;
};

#endif

