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

#include "blinds.h"

Blinds::Blinds(SDL_Rect Dst,SDL_Surface *Surface,
		SDL_Rect Rect,Uint32 Duration,
		bool Vertical,int Pitch,
		Uint8 R,Uint8 G,Uint8 B) : Animation(Dst) {
	// Preset values
	simage=0;
	sframe=0;
	start=0;
	duration=0;

	if(Surface && Duration){
		// Configure object for the wipe
		duration=Duration;
		vertical=Vertical;
		color={R,G,B,0xFF};

		// Assert slats pitch against the span they cover
		int span=Vertical?Rect.w:Rect.h;
		pitch=Pitch>0?Pitch:32;
		if(pitch>span)pitch=span;

		// Find position relative to widget
		crect.x=pos.x;
		crect.y=pos.y;
		crect.w=Rect.w;
		crect.h=Rect.h;

		// Copy final graphics and prepare the frame buffer
		simage=EDL_CreateSurface(Rect.w,Rect.h);
		EDL_BlitSurface(Surface,&Rect,simage,0);
		sframe=EDL_CreateSurface(crect.w,crect.h);
	}
}

Blinds::~Blinds(){
	if(simage){
		SDL_FreeSurface(simage);
	}
	if(sframe){
		SDL_FreeSurface(sframe);
	}
}

/*! \brief Completes the running wipe effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the frame buffer is deleted.
 */
bool Blinds::Skip(){
	bool retval=(sframe!=0);
	if(retval){
		Blit(simage);
		SDL_FreeSurface(sframe);
		sframe=0;
	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Blinds::Continue(){
	return sframe!=0;
}

/*! \brief Paints the current wipe frame
 *
 *  The final graphics are masked by colored slats which shrink
 *  as the effect progresses. Each slat reveals the graphics from
 *  its leading edge towards the anchored one.
 */
void Blinds::Render(){
	if(!start){
		Move(crect);
		start=SDL_GetTicks();
	}
	Uint32 now=SDL_GetTicks();
	if(now<start){
		// Overflow!!
		LogError("BLINDS:OVERRUN!");
		now=now+(0xFFFFFFFF-start);
		start=0;
	}

	// Continue wipe effect
	if(sframe){
		if(now>=start+duration){
			// Complete the wipe
			Blit(simage);
			SDL_FreeSurface(sframe);
			sframe=0;
			Widget::Render();
		}
		else{
			// Assemble the current frame
			double p=(now-start)/(double)duration;
			EDL_BlitSurface(simage,0,sframe,0);
			int span=vertical?crect.w:crect.h;
			int cells=(span+pitch-1)/pitch;
			int revealed=(int)(pitch*p);
			for(int i=0;i<cells;i++){
				int x,y,w,h;
				int edge=i*pitch+revealed;
				if(vertical){
					x=edge;
					y=0;
					w=pitch-revealed;
					h=crect.h;
					if(x+w>crect.w)w=crect.w-x;
				}
				else{
					x=0;
					y=edge;
					w=crect.w;
					h=pitch-revealed;
					if(y+h>crect.h)h=crect.h-y;
				}
				if(w>0&&h>0){
					EDL_SetBox(sframe,x,y,w,h,
						color.r,color.g,color.b,0xFF);
				}
			}

			// Paint the frame
			EDLTexture *stexture=EDL_CreateTexture(crect.w,crect.h);
			stexture->lockTexture();
			stexture->copyPixels(sframe->pixels);
			stexture->unlockTexture();
			stexture->render(crect.x,crect.y);
			stexture->free();
			stexture=0;
			Refresh();
		}
	}
}

