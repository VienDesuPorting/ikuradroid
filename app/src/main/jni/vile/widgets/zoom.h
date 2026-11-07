/*! \class Zoom
 *  \brief Zoom animation widget
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
#ifndef _ZOOM_H_
#define _ZOOM_H_

#include "animation.h"
#include <math.h>

class Zoom : public Animation {
	private:
		// Common
		SDL_Surface *source;			//!< Surface to zoom or whatever into
		SDL_Rect start;
		SDL_Rect end;
		Uint32 t_start;				//!< Time the effect started
		Uint32 t_duration;			//!< How long the effect should run
	public:
		Zoom(SDL_Rect Dst);
		Zoom(SDL_Surface *Surface,SDL_Rect Source,SDL_Rect Start,
				SDL_Rect End,Uint32 Duration);
		~Zoom();
		void SetZoom(SDL_Surface *Surface,SDL_Rect Source,
				SDL_Rect Start,SDL_Rect End,Uint32 Duration);
		bool ZoomSurface(double Progress,SDL_Surface **Surface,SDL_Rect *Rect);
		virtual void Render();
		bool Skip();
		bool Continue();
};

#endif

