/*! \class Slide
 *  \brief Animation that slides across the screen
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
#ifndef _SLIDE_H_
#define _SLIDE_H_

#include "animation.h"

class Slide : public Animation {
	private:
		// Common
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect rstart;
		SDL_Rect rdest;
	public:
		Slide();
		Slide(SDL_Rect Dst);
		Slide(SDL_Surface *Source,SDL_Rect Start,SDL_Rect Dest,int Duration);
		~Slide();
		void Animate(SDL_Rect Start,SDL_Rect Dest,Uint32 Duration);
        virtual void Render();
		virtual bool Skip();
		virtual bool Continue();
};

#endif
