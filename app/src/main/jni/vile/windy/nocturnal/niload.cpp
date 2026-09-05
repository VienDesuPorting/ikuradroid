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

#include "niload.h"
#include "nocturnal.h"

NILoad::NILoad(Nocturnal *Engine) : DialogBase(Engine,true){
	// Save main menu graphics
	SDL_Surface *sback=Engine->LoadImage("omabg.ggd");
	if(sback){
		// Fade in background graphics
		SDL_Rect rfull={0,0,sback->w,sback->h};
		Engine->AddAnimation(new Fade(rfull,sback,rfull,1000));
		Move(rfull);
		Set(sback);

		// Create exit button
		SDL_Rect rexitd={263,444,116,36};
		w_exit=new TextButton(rexitd,"Exit");
		AddWidget(w_exit);

		// Create saveslab widgets
		for(int i=0;i<4;i++){
			SDL_Rect rdest={0,117+(80*i),317,78};
			w_slabs[i]=new SaveSlab(rdest);
			AddWidget(w_slabs[i]);
		}
		for(int i=4;i<8;i++){
			SDL_Rect rdest={320,117+(80*(i-4)),317,78};
			w_slabs[i]=new SaveSlab(rdest);
			AddWidget(w_slabs[i]);
		}

		// Configure slabs
		SDL_Rect rthumb={3,3,96,72};
		SDL_Rect rtext={110,8,200,64};
		for(int i=0;i<8;i++){
			// Alphablend some dead simple backgrounds
			w_slabs[i]->Fill(0,0,0,255);
			w_slabs[i]->SetAlpha(172);
			w_slabs[i]->SetAlpha(128,&rtext);

			// Load savegame data
			Savegame save(engine->NativeID(),i);
			if(save.Read()){
				SDL_Surface *screen=0;
				if(save.LoadSurface("screen-thumb",&screen,96,72)){
					// Set thumbview
					w_slabs[i]->SetThumb(screen,0,&rthumb);
					SDL_FreeSurface(screen);

					// Set saveinfo
					uString savedate="Unknown date";
					save.LoadString("savedate",&savedate);
					w_slabs[i]->SetText(savedate,&rtext);
				}
			}
			else{
				w_slabs[i]->SetText("Empty slot",&rtext);
			}
		}

		// Free graphics
		SDL_FreeSurface(sback);
	}
}

NILoad::~NILoad(){
	DestroyWidgets();
}

bool NILoad::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_exit){
		// Close dialog
		engine->DestroyWidget(this);
		engine->SetTransition();
		retval=true;
	}
	else if(Object==w_slabs[0] ||
			Object==w_slabs[1] ||
			Object==w_slabs[2] ||
			Object==w_slabs[3] ||
			Object==w_slabs[4] ||
			Object==w_slabs[5] ||
			Object==w_slabs[6] ||
			Object==w_slabs[7]){
		// Load game
		int index=-1;
		if(Object==w_slabs[0])	index=0;
		if(Object==w_slabs[1])	index=1;
		if(Object==w_slabs[2])	index=2;
		if(Object==w_slabs[3])	index=3;
		if(Object==w_slabs[4])	index=4;
		if(Object==w_slabs[5])	index=5;
		if(Object==w_slabs[6])	index=6;
		if(Object==w_slabs[7])	index=7;
		if(((Nocturnal*)engine)->EventLoad(index)){
			engine->DestroyWidget(this);
			engine->SetTransition();
		}
		retval=true;
	}

	return retval;
}

