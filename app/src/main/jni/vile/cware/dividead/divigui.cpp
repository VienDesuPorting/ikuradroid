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

#include "divigui.h"
#include "dividead.h"

DiviGUI::DiviGUI(DiviDead *Engine) : DialogBase(Engine,false){
	// Add menu interface to background
	mainmenu=0;
	extramenu=0;
	SDL_Surface *waku_p=Engine->LoadImage("waku_p");
	SDL_Surface *waku_a=Engine->LoadImage("waku_c1");
	if(waku_a && waku_p){
		Move(0,396);
		Resize(640,64);
		mainmenu=new ValueButton();
		SDL_Rect set={0,80,64,64};
		SDL_Rect unset={16,399,64,64};
		mainmenu->Resize(64,64);
		mainmenu->Move(16,399);
		mainmenu->SetState(WS_SELECT,waku_p,&set,waku_p,&set);
		mainmenu->SetState(WS_NORMAL,waku_p,&set,waku_a,&unset);
		mainmenu->SetState(WS_HOVER,waku_p,&set,waku_a,&unset);

		extramenu=new ValueButton();
		set.x+=64;
		unset.x=640-(64+16);
		extramenu->Resize(64,64);
		extramenu->Move(640-(64+16),399);
		extramenu->SetState(WS_SELECT,waku_p,&set,waku_p,&set);
		extramenu->SetState(WS_NORMAL,waku_p,&set,waku_a,&unset);
		extramenu->SetState(WS_HOVER,waku_p,&set,waku_a,&unset);


		AddWidget(mainmenu);
		AddWidget(extramenu);
		SDL_FreeSurface(waku_p);
		SDL_FreeSurface(waku_a);
	}
}

bool DiviGUI::InputOk(Widget *Object){
	bool retval=false;
	if(Object){
		if(Object==mainmenu){
			if(extramenu->GetValue()){
				extramenu->SetValue(((DiviDead*)engine)->ToggleExtramenu());
			}
			mainmenu->SetValue(((DiviDead*)engine)->ToggleMainmenu());
			retval=true;
		}
		if(Object==extramenu){
			if(mainmenu->GetValue()){
				mainmenu->SetValue(((DiviDead*)engine)->ToggleMainmenu());
			}
			extramenu->SetValue(((DiviDead*)engine)->ToggleExtramenu());
			retval=true;
		}
	}
	return retval;
}

void DiviGUI::Clear(){
	mainmenu->SetValue(0);
	extramenu->SetValue(0);
}

