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

#include "sprite.h"

Sprite::Sprite(SDL_Rect Dst) : Widget(Dst) {
	start=SDL_GetTicks();
	spritestack=0;
	spritecur=0;
	fhittable=false;
}

Sprite::Sprite() : Widget() {
	start=SDL_GetTicks();
	spritestack=0;
	spritecur=0;
	fhittable=false;
}

Sprite::~Sprite(){
	ClearFrames();
}

void Sprite::ClearFrames(){
	spritecur=0;
	while(spritestack){
		SPRITEFRAME *tmpptr=spritestack->NextPtr;
		SDL_FreeSurface(spritestack->Surface);
		delete spritestack;
		spritestack=tmpptr;
	}
}

void Sprite::AddFrame(SDL_Surface *Surface,SDL_Rect *Src,Uint32 Duration){
	// Add a new frame to the stackpointer
	SPRITEFRAME *tmpptr=spritestack;
	if(tmpptr){
		while(tmpptr->NextPtr){
			tmpptr=tmpptr->NextPtr;
		}
		tmpptr->NextPtr=new SPRITEFRAME;
		tmpptr=tmpptr->NextPtr;
	}
	else{
		spritestack=new SPRITEFRAME;
		tmpptr=spritestack;
	}

	// Populate sprite
	tmpptr->Surface=EDL_CopySurface(Surface,Src);
	tmpptr->Duration=Duration;
	tmpptr->NextPtr=0;

	// Assert sprite
	if(!spritecur){
		spritecur=spritestack;
	}
}

/*! \brief Triggers refreshes in order to advance the animation
 */
void Sprite::Tick(){
	if(spritestack){
		Uint32 now=SDL_GetTicks();
		if(now<start || now-start>=spritecur->Duration){
			Refresh();
		}
	}
}

void Sprite::Render(){
	if(spritestack){
		// Change frame
		Uint32 now=SDL_GetTicks();
		if(now<start || now-start>=spritecur->Duration){
			start=now;
			if(spritecur->NextPtr){
				spritecur=spritecur->NextPtr;
			}
			else{
				spritecur=spritestack;
			}
		}

		// Copy frame
		SDL_Rect lpos={0,0,pos.w,pos.h};
		//EDL_BlendSurface(spritecur->Surface,&lpos,Dst,&pos);
		EDLTexture *stexture=EDL_CreateTexture(pos.w,pos.h);
		//stexture->loadFromSurface(spritecur->Surface);

		stexture->lockTexture();
        stexture->copyPixels(spritecur->Surface );
        stexture->unlockTexture();

		stexture->render(&lpos,&pos);
		stexture->free();
		stexture=0;
	}
}



