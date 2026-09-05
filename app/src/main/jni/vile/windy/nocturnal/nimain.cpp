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

#include "nimain.h"
#include "niremenisce.h"
#include "nocturnal.h"

NIMain::NIMain(Nocturnal *Engine) : DialogBase(Engine,true){
	// Load main menu graphics
	SDL_Surface *stitle1=Engine->LoadImage("brand1.ggd");
	SDL_Surface *stitle2=Engine->LoadImage("brand2.ggd");
	SDL_Surface *stitle3=Engine->LoadImage("title.ggd");
	SDL_Surface *stitle4=Engine->LoadImage("sel.ggd");
	if(stitle1 && stitle2 && stitle3 && stitle4){
		// Fade in background graphics
		Move(0,0);
		Resize(stitle3->w,stitle3->h);
		Set(stitle3);
		SDL_Rect rfull={0,0,stitle1->w,stitle1->h};
		Engine->AddAnimation(new Fade(rfull,stitle1,rfull,1000));
		Engine->AddAnimation(new Fade(rfull,stitle2,rfull,1000));
		Engine->AddAnimation(new Fade(rfull,stitle3,rfull,3000));

		// Load selection dialog
		SDL_Rect rmenu={40,40,stitle4->w,stitle4->h};
		SDL_Rect ritem={60,50,stitle4->w-40,20};
		w_button_newgame=new TextButton(ritem,"New Game");
		AddWidget(w_button_newgame);
		ritem.y+=24;
		w_button_loadgame=new TextButton(ritem,"Load Game");
		AddWidget(w_button_loadgame);
		ritem.y+=24;
		w_button_album=new TextButton(ritem,"Remenisce");
		AddWidget(w_button_album);
		ritem.y+=24;
		w_button_exit=new TextButton(ritem,"Exit");
		AddWidget(w_button_exit);
		Blend(stitle4,0,&rmenu);

		// Free graphics
		SDL_FreeSurface(stitle1);
		SDL_FreeSurface(stitle2);
		SDL_FreeSurface(stitle3);
		SDL_FreeSurface(stitle4);
	}
}

bool NIMain::InputOk(Widget *Object){
	bool retval=false;
	Nocturnal *niengine=(Nocturnal*)engine;
	if(niengine && Object==w_button_newgame){
		// Start a new game from the original scripts
		niengine->EventNew();
		niengine->FillBackground(0xFFFFFFFF);
		SetVisible(false);
		retval=true;
	}
	else if(niengine && Object==w_button_loadgame){
		// Show load dialog
		niengine->EventGameDialog(VD_LOAD);
		retval=true;
	}
	else if(niengine && Object==w_button_album){
		// Show album/remenisce feature
		niengine->AddWidget(new NIRemenisce(niengine),VL_DIALOG);
		retval=true;
	}
	else if(niengine && Object==w_button_exit){
		// Send shutdown signal to SDL
		niengine->EventGameShutdown();
		retval=true;
	}
	return retval;
}

