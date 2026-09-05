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

#include "fade.h"

FadeBlack::FadeBlack(SDL_Rect Dst,Uint32 Duration) : Fade(Dst){
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	SetFade(0,0,0,Rect,Duration);
}

FadeWhite::FadeWhite(SDL_Rect Dst,Uint32 Duration) : Fade(Dst){
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
    SetFade(0xFF,0xFF,0xFF,Rect,Duration);
}

FadeColor::FadeColor(SDL_Rect Dst,
		SDL_Color Color,Uint32 Duration) : Fade(Dst){
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	SetFade(Color.r,Color.g,Color.b,Rect,Duration);
}

FadeColor::FadeColor(SDL_Rect Dst,
		Uint8 R,Uint8 G,Uint8 B,Uint32 Duration) : Fade(Dst){
	SDL_Rect Rect={0,0,Dst.w,Dst.h};
	SetFade(R,G,B,Rect,Duration);
}

Fade::Fade(SDL_Rect Dst) : Animation(Dst) {
	// Set default values
	sfade=0;
	start=0;
	duration=0;
}

Fade::Fade(SDL_Rect Dst,SDL_Surface *Surface,
		SDL_Rect Rect,Uint32 Duration) : Animation(Dst) {
	// Set default values
	sfade=0;
	start=0;
	duration=0;
	// Prepare surface
	SetFade(Surface,Rect,Duration);
}

Fade::~Fade(){
	if(sfade){
		SDL_FreeSurface(sfade);
	}
}

void Fade::SetFade(SDL_Surface *Surface,SDL_Rect Rect,Uint32 Duration){
	// Prepare surface
	if(Surface && Duration){
		// Configure object for fading
		duration=Duration;
		start=0;

		// Find position relative to widget
		crect.x=pos.x;
		crect.y=pos.y;
		crect.w=Rect.w;
		crect.h=Rect.h;
		if(crect.w<pos.w){
			crect.w=pos.w;
		}
		if(crect.h<pos.h){
			crect.h=pos.h;
		}

		if(sfade){
			SDL_FreeSurface(sfade);
		}
		sfade=EDL_CreateSurface(Rect.w,Rect.h);
		EDL_BlitSurface(Surface,&Rect,sfade,0);
	}
}


void Fade::SetFade(Uint8 R,Uint8 G,Uint8 B,SDL_Rect Rect,Uint32 Duration)
{
	if(Duration){
        color = {R,G,B,0xFF};
		// Configure object for fading
		duration=Duration;
		start=0;
		crect.x=pos.x;
		crect.y=pos.y;
		crect.w=Rect.w;
		crect.h=Rect.h;
		if(crect.w<pos.w){
			crect.w=pos.w;
		}
		if(crect.h<pos.h){
			crect.h=pos.h;
		}
		if(sfade){
			SDL_FreeSurface(sfade);
		}
		sfade=EDL_CreateSurface(Rect.w,Rect.h);
		EDL_SetBox(sfade,0,0,Rect.w,Rect.h,R,G,B,0xFF);
	}
}
/*! \brief Completes the running fade effect
 *
 *  The finished graphics are copied to the primary widget surface
 *  and the animation surface is deleted. Subsequent calls to Copy()
 *  will use the widget graphics as source.
 */
bool Fade::Skip(){
	bool retval=sfade;

	if(retval){
//		SDL_SetAlpha(sfade,SDL_SRCALPHA,0xFF);

        SDL_SetSurfaceAlphaMod(sfade,0xFF);
		Blit(sfade);
		SDL_FreeSurface(sfade);
		sfade=0;

	}
	return retval;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Fade::Continue(){
	return sfade!=0;
}

/*! \brief Copies graphics
 *  \param Dst Target surface
 */
 void Fade::Render(){
   	if(!start){
		Move(crect);
		start=SDL_GetTicks();
	}
	Uint32 now=SDL_GetTicks();
	if(sfade){
		if(now<start){
			// Overflow!!
			LogError("FADING:OVERRUN!");
			now=now+(0xFFFFFFFF-start);
			start=0;
		}
	}

	// Continue fade effect
	if(!sfade){
       //  boxRGBA(EDLRenderer,0,0,crect.w,crect.h,color.r,color.g,color.b,0xFF);
	     printf("boxRGBA\n");
	}
	else if(now>=start+duration){
       // EDL_SetAlpha(sfade,0,0xFF);
		Blit(sfade);
		SDL_FreeSurface(sfade);
		sfade=0;
        Widget::Render();
	}
	else{
		// Calculate a new fade step based upon time
		double p=(now-start)/(double)duration;
		p=255-(EDL_HANNING(p,2)*255);


		if(sfade){
            EDLTexture *stexture = EDL_CreateTexture(crect.w,crect.h);
            stexture->lockTexture();
            stexture->copyPixels( sfade->pixels );
            stexture->unlockTexture();
            stexture->setAlpha((Uint8)EDL_LIMIT(p,0x00,0xFF));
            stexture->render(crect.x,crect.y);
            stexture->free();
            stexture=0;
		}

		Refresh();
	}
 }



