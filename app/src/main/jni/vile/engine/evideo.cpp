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

#include "evideo.h"
#include "../common/config.h"

EngineVideo::EngineVideo(int Width,int Height) : EngineMixer(Width,Height){
	asurface=0;
	aframe=0;
	if(Cfg::Video::Overlay){
		//! \todo Find out why buffering video overlays causes memory corruption
		Cfg::Video::Framecount=1;
	}
}

EngineVideo::~EngineVideo(){

}

/*! \brief Background thread to decode and buffer video data
 */
 SDL_Renderer * EngineVideo::getrenderer()
 {
 return EDLRenderer;
 }

void EngineVideo::StopAnimation(){
	if(asurface){
		for(int i=0;asurface[i];i++){
			SDL_FreeSurface(asurface[i]);
		}
		delete asurface;
		asurface=0;
		aframe=0;
	}
}

/*! \brief Plays an animation sequence
 *  \param Animation Nullterminated sequence to animate
 *  \return True if animation was accepted
 */
bool EngineVideo::PlayAnimation(SDL_Surface **Animation){
	StopAnimation();
	if(Animation){
		asurface=Animation;
		aframe=0;
	}
	return Animation;
}

 void EngineVideo::render(){
	if(asurface){
		// Blit animation sequence until user cancels
		if(asurface[aframe]){

			SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
			SDL_RenderClear(EDLRenderer);
            EDLTexture *stexture = EDL_CreateTexture(asurface[aframe]->w,asurface[aframe]->h);
            stexture->loadFromSurface(asurface[aframe]);
           // stexture->lockTexture();
            //stexture->copyPixels( asurface[aframe]->pixels );
           // stexture->unlockTexture();
            SDL_SetRenderTarget( EDLRenderer, NULL );
            stexture->render(0,0);
            SDL_RenderPresent(EDLRenderer);

            stexture->free();
            stexture=0;
          //  LogError("fals");

            aframe++;
		}
		else{
			aframe=0;
		}

		// Fixed framerate limit for animations
		LimitFramerate(10);
	}
	else{
		// Call base painter
		EngineBase::render();
	}
}

bool EngineVideo::PlayingAnimation(){
	return asurface;
}

void EngineVideo::EventHostTick(){
	//if(!PlayingAnimation()){
		EngineBase::EventHostTick();
	//}
}

void EngineVideo::EventHostMouseMove(SDL_Surface *Screen,int X,int Y){
	if(!PlayingAnimation()){
		EngineBase::EventHostMouseMove(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseLeftDown(SDL_Surface *Screen,int X,int Y){
	if(!PlayingAnimation()){
		EngineBase::EventHostMouseLeftDown(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseRightDown(SDL_Surface *Screen,int X,int Y){
	if(!PlayingAnimation()){
		EngineBase::EventHostMouseRightDown(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseLeftUp(SDL_Surface *Screen,int X,int Y){
	if(PlayingAnimation()){
		StopAnimation();
	}
	else{
		EngineBase::EventHostMouseLeftUp(Screen,X,Y);
	}
}

void EngineVideo::EventHostMouseRightUp(SDL_Surface *Screen,int X,int Y){
	if(PlayingAnimation()){
		StopAnimation();
	}
	else{
		EngineBase::EventHostMouseRightUp(Screen,X,Y);
	}
}

void EngineVideo::EventHostKeyDown(SDL_Keycode Key){
	if(PlayingAnimation()){
		if(KEY_ACTION(Key)){
			StopAnimation();
		}
	}
	else{
		EngineBase::EventHostKeyDown(Key);
	}
}

void EngineVideo::EventHostKeyUp(SDL_Keycode Key){
	if(!PlayingAnimation()){
		EngineBase::EventHostKeyUp(Key);
	}
}

