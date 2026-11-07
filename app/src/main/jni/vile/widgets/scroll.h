/*! \class Scroll
 *  \brief Scrolling widget
 *  \todo Implement scrolling in all four directions!
 *
 *  Scroll currently only works if you are scrolling downwards a vertical
 *  surface. This should be relatively easy to fix when needed.
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
#ifndef _SCROLL_H_
#define _SCROLL_H_

#include "animation.h"

class Scroll : public Animation {
	private:
		// Common
		SDL_Surface *surface;		//!< Scrolling surface
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect rstart;			//!< Start source rectange
		SDL_Rect rdest;				//!< End source rectange
	public:
		Scroll();
		Scroll(SDL_Rect Dst);
		Scroll(SDL_Rect Dst,SDL_Surface *Source,SDL_Rect Start,
				SDL_Rect End,Uint32 Time);
		~Scroll();
		void Animate(SDL_Surface *Src,SDL_Rect Start,SDL_Rect End,
				Uint32 Time);
		virtual bool Skip();
		virtual bool Continue();
		virtual void Render();
};

#endif
