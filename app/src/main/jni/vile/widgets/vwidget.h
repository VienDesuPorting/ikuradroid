/*! \class VideoWidget
 *  \brief Defines a non-graphical widget that starts a video
 *
 *  This widget allows queing a video resource in the blitting queue, hence
 *  allowing it to execute as part of a sequence of animations or other
 *  videos.
 *
 *  Usually used through the EngineVN::QueueVideo() convenience functions
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
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
