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

#include "zoom.h"

Zoom::Zoom(SDL_Rect Dst) : Animation(Dst) {
	// Set default values
	EDL_SETRECT(start,0,0,0,0);
	EDL_SETRECT(end,0,0,0,0);
	t_start=0;
	t_duration=0;
	source=0;
}

Zoom::Zoom(SDL_Surface *Surface,SDL_Rect Source,
	SDL_Rect Start,SDL_Rect End,Uint32 Duration) : Animation(Start) {
	// Set default values
	EDL_SETRECT(start,0,0,0,0);
	EDL_SETRECT(end,0,0,0,0);
	t_start=0;
	t_duration=0;
	source=0;

	// Prepare surface
	SetZoom(Surface,Source,Start,End,Duration);
}

Zoom::~Zoom(){
	if(source){
		SDL_FreeSurface(source);
	}
}

void Zoom::SetZoom(SDL_Surface *Surface,SDL_Rect Source,
		SDL_Rect Start,SDL_Rect End,Uint32 Duration){
	// Configure object
	t_duration=Duration;
	t_start=0;
	start=Start;
	end=End;

	// Copy graphics
	if(source){
		SDL_FreeSurface(source);
	}
	source=EDL_CopySurface(Surface,&Source);
}

/*! \brief Completes the running fade effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the animation surface is deleted. Subsequent calls to Copy()
 *  will use the widget graphics as source.
 */
bool Zoom::Skip(){
	bool retval=source;
	if(source){
		// Complete zoom
		SDL_Surface *zoom=0;
		SDL_Rect rect;
		if(ZoomSurface(100,&zoom,&rect)){
			//if(simage){
			//	SDL_FreeSurface(simage);
			//}
			//simage=zoom;
			Move(rect);
			Refresh();
		}
		SDL_FreeSurface(source);
		source=0;
	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Zoom::Continue(){
	return source!=0;
}

/*! \brief Copies graphics
 *  \param Dst Target surface
 */

void Zoom::Render(){
if(source){
		if(!t_start){
			// Assert time
			t_start=SDL_GetTicks();
			Move(start);
		}
		Uint32 now=SDL_GetTicks();
		if(now>=t_start+t_duration){
			// Zoom is complete ...
			SDL_Surface *zoom=0;
			SDL_Rect rect;
			if(ZoomSurface(100,&zoom,&rect)){
				if(simage){
					SDL_FreeSurface(simage);
				}
				simage=zoom;
				Move(rect);
				Refresh();
			}
			if(source){
				SDL_FreeSurface(source);
				source=0;
			}
		}
		else{
			// Calculate a new step based upon time
			double p=(now-t_start)/(double)t_duration;
			SDL_Surface *zoom=0;
			SDL_Rect rect;
			if(ZoomSurface(p*100,&zoom,&rect)){
				if(simage){
					SDL_FreeSurface(simage);
				}
				simage=zoom;
				Move(rect);
				Refresh();
			}
		}
	}

	Widget::Render();

}

bool Zoom::ZoomSurface(double Progress,SDL_Surface **Surface,SDL_Rect *Rect){
	bool retval=false;
	if(source){
		Progress=Progress/100;
		SDL_Rect c;
		if(end.x>start.x){
			// Move right
			c.x=start.x+((end.x-start.x)*Progress);
		}
		else{
			// Move left
			c.x=end.x+((start.x-end.x)*(1-Progress));
		}
		if(end.y>start.y){
			// Move down
			c.y=start.y+((end.y-start.y)*Progress);
		}
		else{
			// Move up
			c.y=end.y+((start.y-end.y)*(1-Progress));
		}

		if(end.w>start.w){
			// Expand
			c.w=start.w+((end.w-start.w)*Progress);
		}
		else{
			// Shrink
			c.w=end.w+((start.w-end.w)*(1-Progress));
		}
		if(end.h>start.h){
			// Expand
			c.h=start.h+((end.h-start.h)*Progress);
		}
		else{
			// Shrink
			c.h=end.h+((start.h-end.h)*(1-Progress));
		}

		// Scale widget image and redraw using base
		if(c.w>1 && c.h>1 && !(c.w&0x8000) && !(c.h&0x8000)){
			if((*Surface=zoomSurface(source,
					c.w/(double)source->w,
					c.h/(double)source->h,1))){
				*Rect=c;
				retval=true;
			}
		}
	}
	return retval;
}

