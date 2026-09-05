/*! \class VideoBuffer
 *  \brief VideoBuffer defines a generic video decoder object.
 *
 *  VideoBuffer objects are loaded and driven by the EngineVideo class. A
 *  derived object is supposed to transparently provide a video feed by
 *  responding to the virtual methods.
 */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "../res/rwops.h"
#include "../common/log.h"

class VideoBuffer {
	public:
		VideoBuffer();
		virtual ~VideoBuffer();

		// Engine interface
		virtual bool Open(RWops *Resource)=0;
		virtual double Framerate()=0;
		virtual bool Paint()=0;
		virtual bool Decode()=0;
		virtual void Close();
};

#endif

