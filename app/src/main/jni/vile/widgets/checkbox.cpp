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

#include "checkbox.h"

Checkbox::Checkbox(SDL_Rect Pos,uString Caption,bool Leftsided)
	: ValueButton(Pos) {
	leftsided=Leftsided;
	caption=Caption;
	autogenerate();
}

Checkbox::Checkbox(uString Caption,bool Leftsided) : ValueButton() {
	leftsided=Leftsided;
	caption=Caption;
	autogenerate();
}

void Checkbox::SetOrientation(bool Leftsided){
	leftsided=Leftsided;
	autogenerate();
}

void Checkbox::SetCaption(uString Caption){
	caption=Caption;
	autogenerate();
}

void Checkbox::autogenerate(){
	SDL_Surface *nssurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *nusurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *hssurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *husurface=EDL_CreateSurface(pos.w,pos.h);
	SDL_Surface *bmp=Media::GetImage("checkbox");
	if(bmp){
		SDL_Surface *txt=EDL_CreateText(caption,
				Cfg::Color::WidgetFont,
				nssurface->w-bmp->w,nssurface->h);
		if(txt){
			SDL_Rect d={bmp->w*1.25,0,txt->w,txt->h};
			if(!leftsided){
				d.x=0;
			}
			if(txt->h<pos.h){
				d.y=(pos.h-txt->h)/2;
			}
			EDL_BlitSurface(txt,0,nssurface,&d);
			EDL_BlitSurface(txt,0,nusurface,&d);
			EDL_BlitSurface(txt,0,hssurface,&d);
			EDL_BlitSurface(txt,0,husurface,&d);
			SDL_FreeSurface(txt);
		}
		SDL_Rect s={0,0,bmp->w,bmp->h/4};
		SDL_Rect d={0,0,bmp->w,bmp->h/4};
		if(s.h<pos.h){
			d.y=(pos.h-s.h)/2;
		}
		if(!leftsided){
			d.x=pos.w-d.w;
		}
		EDL_BlitSurface(bmp,&s,nssurface,&d);
		s.y+=s.h;
		EDL_BlitSurface(bmp,&s,hssurface,&d);
		s.y+=s.h;
		EDL_BlitSurface(bmp,&s,nusurface,&d);
		s.y+=s.h;
		EDL_BlitSurface(bmp,&s,husurface,&d);

		SDL_FreeSurface(bmp);
		SetState(WS_NORMAL,nssurface,0,nusurface,0);
		SetState(WS_HOVER,hssurface,0,husurface,0);
	}
	SDL_FreeSurface(nssurface);
	SDL_FreeSurface(nusurface);
	SDL_FreeSurface(hssurface);
	SDL_FreeSurface(husurface);
}

bool Checkbox::MouseLeftDown(int X,int Y){
	SetValue(!GetValue());
	return true;
}

bool Checkbox::KeyDown(SDL_Keycode Key){
	if(KEY_ACTION_OK(Key)){
		SetValue(!GetValue());
	}
	return true;
}

