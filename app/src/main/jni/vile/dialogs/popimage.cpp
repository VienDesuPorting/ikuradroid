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

#include "popimage.h"
#include "../engine/evn.h"

PopImage::PopImage(EngineVN *Engine) : 
		DialogBase(Engine,true){
	// Configure container dialog
	int w=Engine->NativeWidth();
	int h=Engine->NativeHeight();
	Move(0,0);
	Resize(w,h);

	// Configure hotspot and display
	widget=new Hotspot();
	widget->Move(0,0);
	widget->Resize(w,h);
	AddWidget(widget);
	slides=0;
}

PopImage::PopImage(EngineVN *Engine,SDL_Surface *Surface) : 
		DialogBase(Engine,true){
	// Configure container dialog
	int w=Engine->NativeWidth();
	int h=Engine->NativeHeight();
	Move(0,0);
	Resize(w,h);
	Fill(0,0,0,0);

	// Configure hotspot and display
	widget=new Hotspot();
	if(Surface){
		int x=(w-Surface->w)/2;
		int y=(h-Surface->h)/2;
		widget->Move(x,y);
		widget->Resize(Surface->w,Surface->h);
		widget->Blit(Surface);
	}
	AddWidget(widget);
	slides=0;
}

PopImage::~PopImage(){ 
	while(slides){
		SLIDES_TYPE *tmpptr=slides->next;
		SDL_FreeSurface(slides->surface);
		delete slides;
		slides=tmpptr;
	}
}

/*! \brief Adds a manually positioned image 
 *  \param Surface Image to display
 *  \param X Screen coordinate
 *  \param Y Screen coordinate
 */
void PopImage::AddImage(SDL_Surface *Surface,int X,int Y){
	if(widget->GetSurface()){
		// Queue image for later
		if(slides){
			SLIDES_TYPE *tmpptr=slides;
			while(tmpptr->next){
				tmpptr=tmpptr->next;
			}
			tmpptr->next=new SLIDES_TYPE;
			tmpptr=tmpptr->next;
			tmpptr->surface=EDL_CopySurface(Surface,0);
			tmpptr->rect.x=X;
			tmpptr->rect.y=Y;
			tmpptr->rect.w=Surface->w;
			tmpptr->rect.h=Surface->h;
			tmpptr->next=0;
		}
		else{
			slides=new SLIDES_TYPE;
			slides->surface=EDL_CopySurface(Surface,0);
			slides->rect.x=X;
			slides->rect.y=Y;
			slides->rect.w=Surface->w;
			slides->rect.h=Surface->h;
			slides->next=0;
		}
	}
	else{
		// Display image immidiatly
		widget->Move(X,Y);
		widget->Resize(Surface->w,Surface->h);
		widget->Blit(Surface);
	}

}

/*! \brief Adds a centered image
 *  \param Surface Image to display
 */
void PopImage::AddImage(SDL_Surface *Surface){
	int x=(GetWidth()-Surface->w)/2;
	int y=(GetHeight()-Surface->h)/2;
	AddImage(Surface,x,y);
}

bool PopImage::InputOk(Widget *Object){
	if(slides){
		// Display image
		widget->Move(slides->rect.x,slides->rect.y);
		widget->Resize(slides->surface->w,slides->surface->h);
		widget->Blit(slides->surface);

		// Drop from queue
		SLIDES_TYPE *tmpptr=slides->next;
		SDL_FreeSurface(slides->surface);
		delete slides;
		slides=tmpptr;
	}
	else{
		// Nothing more to display ...
		engine->DestroyWidget(this);
	}
	return true;
}


