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

#include "bbutton.h"

BitmapButton::BitmapButton() : StateWidget(){
	fhittable=true;
	state=WS_NORMAL;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		surfaces[i]=0;
	}
	ChangeState(WS_NORMAL);
}

BitmapButton::BitmapButton(int X,int Y,int W,int H) : StateWidget(X,Y,W,H){
	fhittable=true;
	state=WS_NORMAL;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		surfaces[i]=0;
	}
}

BitmapButton::BitmapButton(SDL_Rect Pos) : StateWidget(Pos){
	fhittable=true;
	state=WS_NORMAL;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		surfaces[i]=0;
	}
}

BitmapButton::~BitmapButton(){
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		if(surfaces[i]){
			SDL_FreeSurface(surfaces[i]);
		}
	}
}

void BitmapButton::ChangeState(WIDGET_STATE State){
	StateWidget::ChangeState(State);
	if(State<WIDGET_STATE_SIZE){
		state=State;
		if(surfaces[State]){
			Blit(surfaces[State]);
		}
		else if(surfaces[WS_NORMAL]){
			Blit(surfaces[WS_NORMAL]);
		}
		else{
			Free();
		}
	}
}

/*! \brief Generates graphics for a captioned button
 */
void BitmapButton::SetCaption(uString String){
	SDL_Surface *nsurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *hsurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *bmp=Media::GetImage("button");
	if(bmp){
		// Create top
		SDL_Rect ns={0,0,bmp->w/3,(bmp->h/2)/3};
		SDL_Rect nd={0,0,bmp->w/3,(bmp->h/2)/3};
		SDL_Rect hs={0,bmp->h/2,bmp->w/3,(bmp->h/2)/3};
		SDL_Rect hd={0,0,bmp->w/3,(bmp->h/2)/3};

		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		ns.x+=ns.w;
		hs.x+=hs.w;
		while(nd.x+nd.w<pos.w){
			nd.x+=nd.w;
			hd.x+=hd.w;
			EDL_BlitSurface(bmp,&ns,nsurface,&nd);
			EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		}
		ns.x=bmp->w-ns.w;
		hs.x=bmp->w-hs.w;
		nd.x=pos.w-nd.w;
		hd.x=pos.w-hd.w;
		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);

		// Create middle
		EDL_SETRECT(ns,0,ns.y+ns.h,ns.w,ns.h);
		EDL_SETRECT(hs,0,hs.y+hs.h,hs.w,hs.h);
		EDL_SETRECT(nd,0,nd.y+nd.h,nd.w,nd.h);
		EDL_SETRECT(hd,0,hd.y+hd.h,hd.w,hd.h);
		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		ns.x+=ns.w;
		hs.x+=hs.w;
		while(nd.x+nd.w<pos.w){
			nd.x+=nd.w;
			hd.x+=hd.w;
			EDL_BlitSurface(bmp,&ns,nsurface,&nd);
			EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		}
		ns.x=bmp->w-ns.w;
		hs.x=bmp->w-hs.w;
		nd.x=pos.w-nd.w;
		hd.x=pos.w-hd.w;
		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);

		// Duplicate middle
		SDL_Rect ts={0,ns.h,pos.w,ns.h};
		SDL_Rect td=ts;
		while(td.y+td.h<pos.h){
			td.y+=td.h;
			EDL_BlitSurface(nsurface,&ts,nsurface,&td);
			EDL_BlitSurface(hsurface,&ts,hsurface,&td);
		}

		// Create bottom
		EDL_SETRECT(ns,0,(bmp->h/2)-ns.h,ns.w,ns.h);
		EDL_SETRECT(hs,0,bmp->h-hs.h,hs.w,hs.h);
		EDL_SETRECT(nd,0,pos.h-nd.h,nd.w,nd.h);
		EDL_SETRECT(hd,0,pos.h-hd.h,hd.w,hd.h);
		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		ns.x+=ns.w;
		hs.x+=hs.w;
		while(nd.x+nd.w<pos.w){
			nd.x+=nd.w;
			hd.x+=hd.w;
			EDL_BlitSurface(bmp,&ns,nsurface,&nd);
			EDL_BlitSurface(bmp,&hs,hsurface,&hd);
		}
		ns.x=bmp->w-ns.w;
		hs.x=bmp->w-hs.w;
		nd.x=pos.w-nd.w;
		hd.x=pos.w-hd.w;
		EDL_BlitSurface(bmp,&ns,nsurface,&nd);
		EDL_BlitSurface(bmp,&hs,hsurface,&hd);


		// Create caption
		SDL_Surface *txt=EDL_CreateText(String,
				Cfg::Color::WidgetFont,
				nsurface->w-bmp->w,
				EDL_MIN(nsurface->h,Cfg::Font::default_size));
		if(txt){
			SDL_Rect d={0,0,txt->w,txt->h};
			if(txt->w<nsurface->w){
				d.x=(nsurface->w-txt->w)/2;
			}
			if(txt->h<nsurface->h){
				d.y=(nsurface->h-txt->h)/2;
			}
			EDL_BlendSurface(txt,0,nsurface,&d);
			EDL_BlendSurface(txt,0,hsurface,&d);
			SDL_FreeSurface(txt);
		}
		SetState(WS_NORMAL,nsurface,0);
		SetState(WS_HOVER,hsurface,0);
		SDL_FreeSurface(bmp);
	}
	SDL_FreeSurface(nsurface);
	SDL_FreeSurface(hsurface);
}

/*! \brief Sets graphics for a specific state
 *  \param State Which set of graphics to use
 *	\param Src Source surface
 *  \param X X coordinate
 *  \param Y Y coordinate
 *  \param W Width
 *  \param H Height
 */
void BitmapButton::SetState(int State,SDL_Surface *Src,int X,int Y,int W,int H){
	SDL_Rect trect={X,Y,W,H};
	SetState(State,Src,&trect);
}

/*! \brief Sets graphics for a specific state
 *  \param State Which set of graphics to use
 *	\param Src Source surface
 *  \param SRect Part of source to copy
 */
void BitmapButton::SetState(int State,SDL_Surface *Src,SDL_Rect *SRect){
	if(State<WIDGET_STATE_SIZE && Src){
		// Assert destination surface
		if(surfaces[State]){
			SDL_FreeSurface(surfaces[State]);
		}
		if(SRect){
			surfaces[State]=EDL_CreateSurface(SRect->w,SRect->h);
		}
		else{
			surfaces[State]=EDL_CreateSurface(GetWidth(),GetHeight());
		}

		// Copy graphics
		EDL_BlitSurface(Src,SRect,surfaces[State],0);
		if(State==state){
			Blit(surfaces[State]);
		}
	}
}

