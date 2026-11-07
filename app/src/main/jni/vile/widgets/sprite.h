/*! \class Sprite
 *  \brief Animated sprite
 *
 *  The sprite can only animate at low framerate as it uses the bulky
 *  widget-tick event to drive itself. Consider an highspeed option on
 *  demand.
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
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "widget.h"

class Sprite : public Widget {
	private:
		struct SPRITEFRAME {
			SDL_Surface *Surface;
			Uint32 Duration;
			SPRITEFRAME *NextPtr;
		}*spritestack,*spritecur;
		Uint32 start;
	public:
		Sprite(SDL_Rect Dst);
		Sprite();
		~Sprite();
		void AddFrame(SDL_Surface *Surface,SDL_Rect *Src,Uint32 Duration);
		void ClearFrames();
		virtual void Tick();
		//virtual void Copy(SDL_Surface *Dst);
		void Render();
};

#endif
