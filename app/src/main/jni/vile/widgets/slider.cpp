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

#include "slider.h"

Slider::Slider(SDL_Rect Pos) : StateWidget(Pos){
	value=-1;
	focus=false;
	fhittable=true;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		full[i]=0;
		empty[i]=0;
	}

	// Load default graphics and autogenerate
	SDL_Surface *bmp=Media::GetImage("slider");
	if(bmp){
		SDL_Rect ns={0,(bmp->h/4)*0,bmp->w,bmp->h/4};
		SDL_Rect nu={0,(bmp->h/4)*2,bmp->w,bmp->h/4};
		SDL_Rect hs={0,(bmp->h/4)*1,bmp->w,bmp->h/4};
		SDL_Rect hu={0,(bmp->h/4)*3,bmp->w,bmp->h/4};
		SetGraphics(WS_NORMAL,bmp,&ns,bmp,&nu);
		SetGraphics(WS_HOVER,bmp,&hs,bmp,&hu);
		SDL_FreeSurface(bmp);
	}
}

Slider::~Slider(){
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		if(full[i]){
			SDL_FreeSurface(full[i]);
		}
		if(empty[i]){
			SDL_FreeSurface(empty[i]);
		}
	}
}

void Slider::ChangeState(WIDGET_STATE State){
	StateWidget::ChangeState(State);
	if(State<WIDGET_STATE_SIZE){
		AutoGenerate();
	}
}

bool Slider::MouseMove(int X,int Y){
	if(focus){
		if(!(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))){
			focus=false;
		}
	}
	if(focus && X>pos.x){
		SetValue(((X-pos.x)/(double)pos.w)*100);
	}
	return true;
}

bool Slider::MouseLeftDown(int X,int Y){
	focus=true;
	SetValue(((X-pos.x)/(double)pos.w)*100);
	return StateWidget::MouseLeftDown(X,Y);
}

bool Slider::MouseLeftUp(int X,int Y){
	focus=false;
	return StateWidget::MouseLeftUp(X,Y);
}

int Slider::GetValue(){
	return value;
}

void Slider::SetValue(int Value){
	if(value!=Value && Value>=0 && Value<=100){
		value=Value;
		AutoGenerate();
	}
}

void Slider::AutoGenerate(){

	if(state<WIDGET_STATE_SIZE && full[state] && pos.w>0 && pos.h>0){
		// Get positions
		int xval=(pos.w*GetValue())/100;
		SDL_Surface *surface=EDL_CreateSurface(pos.w,pos.h);
		SDL_Rect rsrc={0,0,full[state]->w/2,full[state]->h};
		SDL_Rect rdst={0,0,full[state]->w/2,full[state]->h};
		if(pos.h>full[state]->h){
			rdst.y=(pos.h-full[state]->h)/2;
		}
		rsrc.w=EDL_LIMIT(rsrc.w,2,4);
		rdst.w=EDL_LIMIT(rdst.w,2,4);

		// Paint middle section
		rdst.x=rdst.w;
		rsrc.x=full[state]->w/3;
		while(rdst.x<xval-rdst.w){
			EDL_BlitSurface(full[state],&rsrc,surface,&rdst);
			rdst.x+=rdst.w;
		}
		if(xval>rdst.x){
			SDL_Rect tsrc=rsrc;
			SDL_Rect tdst=rdst;
			tdst.w=xval-rdst.x;
			tsrc.w=tdst.w;
			EDL_BlitSurface(full[state],&tsrc,surface,&tdst);
			rdst.x+=tdst.w;
		}
		while(rdst.x<pos.w-rdst.w){
			EDL_BlitSurface(empty[state],&rsrc,surface,&rdst);
			rdst.x+=rdst.w;
		}

		// Paint ends
		rdst.x=0;
		rsrc.x=0;
		if(xval>0){
			EDL_BlitSurface(full[state],&rsrc,surface,&rdst);
		}
		else{
			EDL_BlitSurface(empty[state],&rsrc,surface,&rdst);
		}
		rdst.x=pos.w-rdst.w;
		rsrc.x=full[state]->w-rsrc.w;
		if(xval<pos.w-rdst.w){
			EDL_BlitSurface(empty[state],&rsrc,surface,&rdst);
		}
		else{
			EDL_BlitSurface(full[state],&rsrc,surface,&rdst);
		}

		// Paint details
		if(xval>rdst.x){
			EDL_BlitSurface(full[state],&rsrc,surface,&rdst);
		}

		// Update widget graphic
		Free();
		Blit(surface);
		SDL_FreeSurface(surface);
		Refresh();
	}
}

void Slider::SetGraphics(WIDGET_STATE State,
		SDL_Surface *SFull,SDL_Rect *RFull,
		SDL_Surface *SEmpty,SDL_Rect *REmpty){
	if(State<WIDGET_STATE_SIZE){
		// Set full graphics
		if(full[State]){
			SDL_FreeSurface(full[State]);
		}
		if(empty[State]){
			SDL_FreeSurface(empty[State]);
		}
		SDL_Rect r={0,0,SFull->w,SFull->h};
		if(RFull){
			r=*RFull;
		}
		full[State]=EDL_CreateSurface(r.w,r.h);
		EDL_BlitSurface(SFull,&r,full[State],0);

		// Set empty graphics
		if(empty[State]){
			SDL_FreeSurface(empty[State]);
		}
		if(empty[State]){
			SDL_FreeSurface(empty[State]);
		}
		EDL_SETRECT(r,0,0,SEmpty->w,SEmpty->h);
		if(REmpty){
			r=*REmpty;
		}
		empty[State]=EDL_CreateSurface(r.w,r.h);
		EDL_BlitSurface(SEmpty,&r,empty[State],0);

		// Recreate graphics
		if(State==state){
			AutoGenerate();
		}
	}
}


