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

#include "mcback.h"
#include "mayclub.h"

MCBack::MCBack(Mayclub *Engine,Textview *Texter) : DialogBase(Engine,true){
	// Load main menu graphics
	SDL_Surface *sctp=Engine->LoadImage("ctp.bet");
	SDL_Surface *mctp=Engine->LoadImage("ctp_.bet");
	if(sctp && mctp){
		// Set widget graphics
		SDL_Surface *ctp=EDL_MaskSurface(sctp,mctp,0);
		SDL_Rect wpos={0,0,ctp->w,ctp->h/2};
		Set(ctp,&wpos,&wpos);

		// Create buttons
		SDL_Rect rbutton={0,0,38,16};
		SDL_Rect rdest={14,10,rbutton.w,rbutton.h};
		SDL_Rect rsource={0,(ctp->h/2)+1,rbutton.w,rbutton.h};
		w_back=new BitmapButton(rdest);
		w_back->SetState(WS_NORMAL,ctp,&rsource);
		rsource.y+=rbutton.h;
		w_back->SetState(WS_HOVER,ctp,&rsource);
		w_back->SetState(WS_SELECT,ctp,&rsource);
		AddWidget(w_back);
		rdest.x+=38;
		rsource.x+=38;
		rsource.y-=rbutton.h;
		w_stop=new BitmapButton(rdest);
		w_stop->SetState(WS_NORMAL,ctp,&rsource);
		rsource.y+=rbutton.h;
		w_stop->SetState(WS_HOVER,ctp,&rsource);
		w_stop->SetState(WS_SELECT,ctp,&rsource);
		AddWidget(w_stop);
		rdest.x+=38;
		rsource.x+=38;
		rsource.y-=rbutton.h;
		w_next=new BitmapButton(rdest);
		w_next->SetState(WS_NORMAL,ctp,&rsource);
		rsource.y+=rbutton.h;
		w_next->SetState(WS_HOVER,ctp,&rsource);
		w_next->SetState(WS_SELECT,ctp,&rsource);
		AddWidget(w_next);

		// Move dialog in place
		MoveDialog(480,340);


		// Free graphics
		SDL_FreeSurface(ctp);
		SDL_FreeSurface(sctp);
		SDL_FreeSurface(mctp);
	}

	// Disable textview logging
	current=0;
	texter=Texter;
	texter->EnableTextlog(false);
}

MCBack::~MCBack(){
	current=0;
	uString tmp=texter->GetTextlog(current);
	texter->ClearText();
	texter->PrintText(tmp);
	texter->CompleteText();
	texter->EnableTextlog(true);
}

bool MCBack::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_back){
		retval=true;
		current++;
		uString tmp=texter->GetTextlog(current);
		if(tmp.length()){
			texter->ClearText();
			texter->PrintText(tmp);
			texter->CompleteText();
		}
		else{
			current--;
		}
	}
	if(Object==w_next){
		retval=true;
		if(current>0){
			current--;
			uString tmp=texter->GetTextlog(current);
			texter->ClearText();
			texter->PrintText(tmp);
			texter->CompleteText();
		}
	}
	if(Object==w_stop){
		engine->DestroyWidget(this);
		retval=true;
	}
	return retval;
}

