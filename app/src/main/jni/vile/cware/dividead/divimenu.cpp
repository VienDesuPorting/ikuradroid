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

#include "divimenu.h"
#include "dividead.h"

DiviMenu::DiviMenu(DiviDead *Engine) : Selection(Engine){
	SDL_Surface *tmps=Engine->LoadImage("waku_p");
	if(tmps){
		graphics=EDL_ColorkeySurface(tmps,0x00FF0000,0);
		SDL_FreeSurface(tmps);
	}
}

DiviMenu::~DiviMenu(){
	if(graphics){
		SDL_FreeSurface(graphics);
	}
}

void DiviMenu::Generate(){
	// Generate and set background
	int count=GetWidgetCount();
	int x=100;
	int y=100;
	int width=240;
	int header=36;
	int bottom=20;
	int item=20;
	int height=header+bottom+(item*count);
	SDL_Rect rs={0,0,width,header};
	SDL_Surface *surface=EDL_CreateSurface(width,height);
	EDL_BlitSurface(graphics,&rs,surface,0);
	rs.y+=header;
	rs.h=item;
	for(int i=0;i<count;i++){
		SDL_Rect rd={0,header+(item*i),width,item};
		EDL_BlitSurface(graphics,&rs,surface,&rd);
	}
	rs.y+=item;
	rs.h=bottom;
	SDL_Rect rd={0,height-bottom,width,bottom};
	EDL_BlitSurface(graphics,&rs,surface,&rd);

	// Move widget and set graphics
	Move(x,y);
	Resize(width,height);
	Blit(surface);
	SDL_FreeSurface(surface);

	// Move items about
	for(int i=0;i<count;i++){
		Widget *w=GetWidget(count-(i+1));
		if(w){
			w->Move(x+20,y+header+(item*i)+4);
			w->Resize(width-40,item);
		}
	}
}

bool DiviMenu::InputOk(Widget *Object){
	bool retval=false;
	DiviDead *dengine=(DiviDead*)engine;
	if(Object && dengine){
		int tag=Object->GetTag();
		if(tag==DM_NEW){
			dengine->ClearMenu();
			dengine->EventNew();
			retval=true;
		}
		if(tag==DM_LOAD){
			dengine->ClearMenu();
			dengine->EventGameDialog(VD_LOAD);
			retval=true;
		}
		if(tag==DM_SAVE){
			dengine->ClearMenu();
			dengine->EventGameDialog(VD_SAVE);
			retval=true;
		}
		if(tag==DM_CONFIG){
			dengine->ClearMenu();
			dengine->EventGameDialog(VD_OPTIONS);
			retval=true;
		}
		if(tag==DM_EXIT){
			dengine->ClearMenu();
			dengine->EventGameDialog(VD_SHUTDOWN);
			retval=true;
		}
		if(tag==DM_EXTRAS){
			dengine->ClearMenu();
			dengine->EventGameDialog(VD_EXTRAS);
			retval=true;
		}
		if(tag==DM_SCREENSHOT){
			dengine->ClearMenu();
			dengine->AddWidget(new StdMessage(dengine,
						"Title","Text"),VL_OVERLAY);
		}
	}
	if(retval){
		engine->DestroyWidget(this);
	}
	return retval;
}

