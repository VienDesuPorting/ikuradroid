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

#include "mcmain.h"
#include "mayclub.h"

MCMain::MCMain(Mayclub *Engine) : DialogBase(Engine,true){
	// Load buttons
	w_button_newgame=new BitmapButton();
	w_button_loadgame=new BitmapButton();
	w_button_album=new BitmapButton();
	w_button_exit=new BitmapButton();
	AddWidget(w_button_newgame);
	AddWidget(w_button_loadgame);
	AddWidget(w_button_album);
	AddWidget(w_button_exit);

	// Load main menu graphics
	int width=Engine->NativeWidth();
	int height=Engine->NativeHeight();
	SDL_Surface *stitle1=Engine->LoadImage("title1.bet");
	SDL_Surface *stitle2=Engine->LoadImage("title2.bet");
	SDL_Surface *stitle3=Engine->LoadImage("title3.bet");
	SDL_Surface *ptitle4=Engine->LoadImage("title4.bet");
	SDL_Surface *mtitle4=Engine->LoadImage("title4_.bet");
	SDL_Surface *ptitlep=Engine->LoadImage("titlep.bet");
	SDL_Surface *mtitlep=Engine->LoadImage("titlep_.bet");
	if(stitle1 && stitle2 && stitle3 && 
			ptitle4 && mtitle4 && 
			ptitlep && mtitlep){
		// Fade in background graphics
		SDL_Rect rfull={0,0,stitle1->w,stitle1->h};
		Engine->AddAnimation(new Fade(rfull,stitle1,rfull,1000));
		Engine->AddAnimation(new Fade(rfull,stitle2,rfull,1000));

		SDL_Rect rstart,rdest;
		Slide *tmpslide;

		// Slide menu across the screen beneath the chick
		EDL_SETRECT(rstart,
			width,
			200,
			stitle3->w,
			stitle3->h);
		EDL_SETRECT(rdest,
			0,
			200,
			stitle3->w,
			stitle3->h);
		tmpslide=new Slide();
		tmpslide->Set(stitle3);
		tmpslide->Animate(rstart,rdest,500);
		Engine->AddAnimation(tmpslide);
		EDL_BlendSurface(stitle3,0,stitle2,&rdest);


		// Slide chick across the screen
		EDL_SETRECT(rstart,
			width,
			height-ptitle4->h,
			ptitle4->w,
			ptitle4->h);
		EDL_SETRECT(rdest,
			width-ptitle4->w,
			height-ptitle4->h,
			ptitle4->w,
			ptitle4->h);
		tmpslide=new Slide();
		SDL_Surface *stitle4=EDL_MaskSurface(ptitle4,mtitle4,0);
		if(stitle4){
			tmpslide->Set(stitle4);
			tmpslide->Animate(rstart,rdest,500);
			Engine->AddAnimation(tmpslide);
			EDL_BlendSurface(stitle4,0,stitle2,&rdest);
		}

		// Set completed graphics as background
		Move(rfull);
		Set(stitle2);

		// Create compatible button graphics
		SDL_Rect rselsize={0,0,20,20};
		SDL_Rect rselsource={0,356,rselsize.w,rselsize.h};
		SDL_Rect rseldest={10,10,rselsize.w,rselsize.h};
		SDL_Surface *stitlep=EDL_MaskSurface(ptitlep,mtitlep,&rselsource);
		if(stitlep){
			// Configure button graphics
			SDL_Rect rbutton={0,0,117,59};
			SDL_Rect rdest={28,219,rbutton.w,rbutton.h};
			SDL_Rect rsrc={28,19,rbutton.w,rbutton.h};
			w_button_newgame->Move(rdest.x,rdest.y);
			w_button_newgame->Resize(rbutton.w,rbutton.h);
			w_button_newgame->SetState(WS_NORMAL,stitle3,&rsrc);
			EDL_SETRECT(rseldest,rsrc.x+6,rsrc.y+14,rselsize.w,rselsize.h);
			EDL_BlitSurface(stitlep,0,stitle3,&rseldest);
			w_button_newgame->SetState(WS_HOVER,stitle3,&rsrc);

			EDL_SETRECT(rdest,rdest.x,rdest.y+rbutton.h,rdest.w,rdest.h);
			EDL_SETRECT(rsrc,rsrc.x,rsrc.y+rbutton.h,rsrc.w,rsrc.h);
			w_button_loadgame->Move(rdest.x,rdest.y);
			w_button_loadgame->Resize(rbutton.w,rbutton.h);
			w_button_loadgame->SetState(WS_NORMAL,stitle3,&rsrc);
			EDL_SETRECT(rseldest,rsrc.x+6,rsrc.y+14,rselsize.w,rselsize.h);
			EDL_BlitSurface(stitlep,0,stitle3,&rseldest);
			w_button_loadgame->SetState(WS_HOVER,stitle3,&rsrc);

			EDL_SETRECT(rdest,rdest.x,rdest.y+rbutton.h,rdest.w,rdest.h);
			EDL_SETRECT(rsrc,rsrc.x,rsrc.y+rbutton.h,rsrc.w,rsrc.h);
			w_button_album->Move(rdest.x,rdest.y);
			w_button_album->Resize(rbutton.w,rbutton.h);
			w_button_album->SetState(WS_NORMAL,stitle3,&rsrc);
			EDL_SETRECT(rseldest,rsrc.x+6,rsrc.y+14,rselsize.w,rselsize.h);
			EDL_BlitSurface(stitlep,0,stitle3,&rseldest);
			w_button_album->SetState(WS_HOVER,stitle3,&rsrc);

			EDL_SETRECT(rdest,rdest.x,rdest.y+rbutton.h,rdest.w,rdest.h);
			EDL_SETRECT(rsrc,rsrc.x,rsrc.y+rbutton.h,rsrc.w,rsrc.h);
			w_button_exit->Move(rdest.x,rdest.y);
			w_button_exit->Resize(rbutton.w,rbutton.h);
			w_button_exit->SetState(WS_NORMAL,stitle3,&rsrc);
			EDL_SETRECT(rseldest,rsrc.x+6,rsrc.y+14,rselsize.w,rselsize.h);
			EDL_BlitSurface(stitlep,0,stitle3,&rseldest);
			w_button_exit->SetState(WS_HOVER,stitle3,&rsrc);
		}

		// Free graphics
		SDL_FreeSurface(stitle1);
		SDL_FreeSurface(stitle2);
		SDL_FreeSurface(stitle3);
		SDL_FreeSurface(ptitle4);
		SDL_FreeSurface(mtitle4);
		SDL_FreeSurface(stitle4);
		SDL_FreeSurface(ptitlep);
		SDL_FreeSurface(mtitlep);
		SDL_FreeSurface(stitlep);
	}
}

bool MCMain::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_button_newgame){
		// Load a new game from the original scripts
		Mayclub *mcengine=(Mayclub*)engine;
		mcengine->FillBackground(0xFFFFFFFF);
		mcengine->EventNew();
		SetVisible(false);
		retval=true;
	}
	else if(Object==w_button_loadgame){
		// Load dialog on top of this one
		engine->AddWidget(new MCLoad((Mayclub*)engine),VL_DIALOG);
		retval=true;
	}
	else if(Object==w_button_album){
		// Load dialog on top of this one
		engine->AddWidget(new MCAlbum((Mayclub*)engine),VL_DIALOG);
		retval=true;
	}
	else if(Object==w_button_exit){
		// Send shutdown signal to SDL
		engine->EventGameShutdown();
		retval=true;
	}
	return retval;
}

