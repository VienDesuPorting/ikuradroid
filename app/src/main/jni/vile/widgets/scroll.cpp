/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "scroll.h"

Scroll::Scroll(SDL_Rect Dst,SDL_Surface *Source,SDL_Rect Start,SDL_Rect End,
		Uint32 Duration) : Animation(Dst) {
	// Set default values
	start=0;
	duration=0;
	surface=0;
	Animate(Source,Start,End,Duration);
}


Scroll::Scroll(SDL_Rect Dst) : Animation(Dst) {
	// Set default values
	start=0;
	duration=0;
	surface=0;
}

Scroll::Scroll() : Animation() {
	// Set default values
	start=0;
	duration=0;
	surface=0;
}

Scroll::~Scroll(){
	if(surface){
		SDL_FreeSurface(surface);
	}
}

/*! \brief Completes the running fade effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the animation surface is deleted. Subsequent calls to Copy()
 *  will use the widget graphics as source.
 */
bool Scroll::Skip(){
	bool retval=surface;
	if(surface){
		Blit(surface,&rdest);
		SDL_FreeSurface(surface);
		surface=0;
		duration=0;
		start=0;
	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Scroll::Continue(){
	return surface;
}

void Scroll::Animate(SDL_Surface *Src,SDL_Rect Start,SDL_Rect End,Uint32 Time){
	// Prepare
	start=0;
	duration=Time;
	rstart=Start;
	rdest=End;
	if(surface){
		SDL_FreeSurface(surface);
	}

	// Create surface and build graphics
	surface=EDL_CopySurface(Src,0);

	// Assert redraw
	Refresh();
}

void Scroll::Render(){
	if(!start && surface){
		// Start effect
		start=SDL_GetTicks();
	}
	else if(surface){
		Uint32 now=SDL_GetTicks();
		if(now>start && now-start<duration){
			// Calculate current position and blit graphics
			double progress=(now-start)/(double)duration;
			SDL_Rect rsrc={rstart.x+((rdest.x-rstart.x)*progress),
				rstart.y+((rdest.y-rstart.y)*progress),rstart.w,rstart.h};
			SDL_Rect rdst={0,0,rstart.w,rstart.h};
			Blit(surface,&rsrc,&rdst);
		}
		else{
			// Complete effect
			Blit(surface,&rdest);
			SDL_FreeSurface(surface);
			surface=0;
			duration=0;
			start=0;
		}
	}

	// Copy to display
	Widget::Render();
}

