/*! \class MusicWidget
 *  \brief Defines a non-graphical widget that starts a music track
 *
 *  This widget allows queing a video resource in the blitting queue, hence
 *  allowing it to execute as part of a sequence of animations or videos.
 *
 *  Usually used through the EngineVN::QueueMusic() convenience functions
 */
#ifndef _MUSICWIDGET_H_
#define _MUSICWIDGET_H_

#include "animation.h"

class EngineVN;

class MusicWidget : public Animation {
	private:
		// Overrides
		virtual void Render();
		virtual bool Continue();
		virtual bool Skip();

		// The actual payload
		EngineVN *engine;
		RWops *music;
	public:
		MusicWidget(EngineVN *Engine,RWops *Music);
};

#endif
