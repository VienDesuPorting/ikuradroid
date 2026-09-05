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

#include "starrytv.h"
#include "starry.h"

StarryTextview::StarryTextview(StarrySky *Engine) : Textview(Engine) {
	SDL_Rect rect;
	//SetTextPosition(0,0,800,165);
	//MoveDialog(0,0);
	//Resize(800,165);
	w_save=0;
	w_load=0;
	w_skip=0;
	w_auto=0;
	w_history=0;
	w_repeat=0;
	w_thing=0;
	header=0;
	SDL_Surface **winbase=Engine->LoadMaskedAnimation("winbase0");
	if(winbase){
		int ypos=0;
		for(int i=0;winbase[i];i++){
			if(Engine->GetImagePosition("winbase0",i,&rect)){
				if(i==STARRYTV_BACKGROUND){
					ypos=Engine->NativeHeight()-rect.h;
					int textx=30;
					int texty=60;
					int textw=550;
					int texth=100;
					SetTextPosition(textx,texty,textw,texth);
					MoveDialog(rect.x,ypos);
					Resize(rect.w,rect.h);
					Set(winbase[i]);
				}
				if(i==STARRYTV_BUTTON_QSAVE){
					// Configure widgets
					rect.y+=ypos;
					w_save=new ValueButton(rect);
					w_save->SetState(WS_NORMAL,winbase[i+21],0,winbase[i+0],0);
					w_save->SetState(WS_HOVER,winbase[i+21],0,winbase[i+7],0);
					w_save->SetState(WS_SELECT,winbase[i+21],0,winbase[i+14],0);
					AddWidget(w_save);
				}
				if(i==STARRYTV_BUTTON_QLOAD){
					// Configure widgets
					rect.y+=ypos;
					w_load=new BitmapButton(rect);
					w_load->SetState(WS_NORMAL,winbase[i],0);
					w_load->SetState(WS_HOVER,winbase[i+7],0);
					w_load->SetState(WS_SELECT,winbase[i+14],0);
					AddWidget(w_load);
				}
				if(i==STARRYTV_BUTTON_SKIP){
					// Configure widgets
					rect.y+=ypos;
					w_skip=new ValueButton(rect);
					w_skip->SetState(WS_NORMAL,winbase[i+20],0,winbase[i+0],0);
					w_skip->SetState(WS_HOVER,winbase[i+20],0,winbase[i+7],0);
					w_skip->SetState(WS_SELECT,winbase[i+20],0,winbase[i+14],0);
					AddWidget(w_skip);
				}
				if(i==STARRYTV_BUTTON_AUTO){
					// Configure widgets
					rect.y+=ypos;
					w_auto=new ValueButton(rect);
					w_auto->SetState(WS_NORMAL,winbase[i+20],0,winbase[i+0],0);
					w_auto->SetState(WS_HOVER,winbase[i+20],0,winbase[i+7],0);
					w_auto->SetState(WS_SELECT,winbase[i+20],0,winbase[i+14],0);
					AddWidget(w_auto);
				}
				if(i==STARRYTV_BUTTON_HISTORY){
					// Configure widgets
					rect.y+=ypos;
					w_history=new BitmapButton(rect);
					w_history->SetState(WS_NORMAL,winbase[i],0);
					w_history->SetState(WS_HOVER,winbase[i+7],0);
					w_history->SetState(WS_SELECT,winbase[i+14],0);
					AddWidget(w_history);
				}

				if(i==STARRYTV_BUTTON_REPEAT){
					// Configure widgets
					rect.y+=ypos;
					w_repeat=new BitmapButton(rect);
					w_repeat->SetState(WS_NORMAL,winbase[i],0);
					w_repeat->SetState(WS_HOVER,winbase[i+7],0);
					w_repeat->SetState(WS_SELECT,winbase[i+14],0);
					AddWidget(w_repeat);
				}
				if(i==STARRYTV_BUTTON_THING){
					// Configure widgets
					rect.y+=ypos;
					w_thing=new BitmapButton(rect);
					w_thing->SetState(WS_NORMAL,winbase[i],0);
					w_thing->SetState(WS_HOVER,winbase[i+7],0);
					w_thing->SetState(WS_SELECT,winbase[i+14],0);
					AddWidget(w_thing);
				}
				if(i==STARRYTV_BUTTON_HEADER){
					// Background for displaying the name
					header=new Printer(pos.x+35,pos.y+10,
							winbase[i]->w,winbase[i]->h);
					header->Blit(winbase[i]);
					AddWidget(header);
				}
			}
			SDL_FreeSurface(winbase[i]);
		}
		delete [] winbase;
	}
}

StarryTextview::~StarryTextview(){
}

bool StarryTextview::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_load){
		((StarrySky*)engine)->EventLoad(998);
		retval=true;
	}
	else if(Object==w_save){
		((StarrySky*)engine)->EventSave(998);
		w_save->SetValue(true);
		retval=true;
	}
	else if(Object==w_history){
		((StarrySky*)engine)->EventGameDialog(VD_LOG);
	}
	else if(Object==w_skip){
		((StarrySky*)engine)->SetSkipmode(true);
		w_skip->SetValue(true);
		retval=true;
	}
	return retval;
}

void StarryTextview::PrintText(uString Title,uString Text){
	if(header){
		header->Clear();
		header->Print(Title.c_str(),0);
		header->SetVisible(true);
	}
	Textview::PrintText(Text);
}

void StarryTextview::PrintText(uString Text){
	if(header){
		header->SetVisible(false);
	}
	Textview::PrintText(Text);
}


