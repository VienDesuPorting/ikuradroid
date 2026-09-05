/*! \class VideoWidget
 *  \brief Defines a non-graphical widget that starts a video
 *
 *  This widget allows queing a video resource in the blitting queue, hence
 *  allowing it to execute as part of a sequence of animations or other
 *  videos.
 *
 *  Usually used through the EngineVN::QueueVideo() convenience functions
 */
#ifndef _VIDEOWIDGET_H_
#define _VIDEOWIDGET_H_

#include "animation.h"

class EngineVN;

class VideoWidget : public Animation {
	private:
		// Overrides
		virtual void Render();
		virtual bool Continue();
		virtual bool Skip();

		// The actual payload
		EngineVN *engine;
		RWops *video;
	public:
		VideoWidget(EngineVN *Engine,RWops *Video);
};

#endif
