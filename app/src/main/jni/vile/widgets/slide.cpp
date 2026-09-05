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

#include "slide.h"

Slide::Slide(SDL_Surface *Source,SDL_Rect Start,SDL_Rect Dest,int Duration) :
		Animation(Start) {
	// Set default values
	Move(Start);
	Set(Source);
	start=0;
	duration=Duration;
	rstart=Start;
	rdest=Dest;
}


Slide::Slide(SDL_Rect Dst) : Animation(Dst) {
	// Set default values
	start=0;
	duration=0;
	rstart=Dst;
	rdest=Dst;
}

Slide::Slide() : Animation() {
	// Set default values
	start=0;
	duration=0;
	EDL_SETRECT(rstart,0,0,0,0);
	EDL_SETRECT(rdest,0,0,0,0);
}

Slide::~Slide(){
}

/*! \brief Completes the running fade effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the animation surface is deleted. Subsequent calls to Copy()
 *  will use the widget graphics as source.
 */
bool Slide::Skip(){
	bool retval=false;
	if(duration){
		Move(rdest);
		start=0;
		duration=0;
		retval=true;
	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Slide::Continue(){
	return duration;
}

void Slide::Animate(SDL_Rect Start,SDL_Rect Dest,Uint32 Duration){
	duration=Duration;
	start=0;
	rstart=Start;
	rdest=Dest;
	Move(rstart);
}

void Slide::Render(){
	if(!start && duration){
		// Start effect
		start=SDL_GetTicks();
		Move(rstart);
	}
	else if(duration){
		Uint32 now=SDL_GetTicks();
		if(now>start && now-start<duration){
			// Move one step
			double progress=(now-start)/(double)duration;
			SDL_Rect rcur={
				rstart.x+((rdest.x-rstart.x)*progress),
				rstart.y+((rdest.y-rstart.y)*progress),
				rdest.w,rdest.h};
			Move(rcur);
		}
		else{
			// Complete effect
			Move(rdest);
			start=0;
			duration=0;
		}
	}

	// Copy to display
	Widget::Render();
}
