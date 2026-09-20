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

#include "critical.h"
#include "criticaltv.h"

CriticalTextview::CriticalTextview(CriticalPoint *Engine) : Textview(Engine) {
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
		for(int i=0;winbase[i];i++){
			if(Engine->GetImagePosition("winbase0",i,&rect)){


				if(i==CRITICALTV_BACKGROUND){
					// Dialog placement: the chrome (616x152) is bottom-anchored to the
					// play area - FRAME.WIP's transparent window spans rows 28..451, so
					// the dialog docks at y=452-152=300, horizontally centered (x=12).
					// The WIPF frame hint (0,0) is dialog-local, not a screen position.
					// Text rows: nameless pages start on row 1
					// (ink tops ~10/34/58 on the reference); named
					// pages get the name on row 1 via the header
					// printer and drop the text block to y=32 -
					// see PrintText() overrides
					// Children of a dialog render at absolute screen positions, so the
					// text block and the name row must be placed relative to the dialog
					// origin
					boxx=(Engine->NativeWidth()-rect.w)/2;
					boxy=Engine->NativeHeight()-28-rect.h;
					SetTextPosition(boxx+12,boxy+8,596,136);
					MoveDialog(boxx,boxy);
					Resize(rect.w,rect.h);
					Set(winbase[i]);
				}/*
				if(i==CRITICALTV_BUTTON_QSAVE){
					// Configure widgets
					rect.y+=ypos;
					w_save=new ValueButton(rect);

					w_save->SetState(WS_NORMAL,winbase[20],0,winbase[20],0);
					w_save->SetState(WS_HOVER,winbase[28],0,winbase[i+20+7],0);
					w_save->SetState(WS_SELECT,winbase[i+19],0,winbase[i+19],0);

					//w_save->SetState(WS_NORMAL,winbase[i+21],0,winbase[i+0],0);
					//w_save->SetState(WS_HOVER,winbase[i+21],0,winbase[i+7],0);
					//w_save->SetState(WS_SELECT,winbase[i+21],0,winbase[i+14],0);
					AddWidget(w_save);
				}
				if(i==CRITICALTV_BUTTON_QLOAD){
					// Configure widgets
					rect.y+=ypos;
					w_load=new BitmapButton(rect);
					w_load->SetState(WS_NORMAL,winbase[19],0);
					w_load->SetState(WS_HOVER,winbase[31],0);
					w_load->SetState(WS_SELECT,winbase[i+17],0);
					AddWidget(w_load);
				}
				if(i==CRITICALTV_BUTTON_SKIP){
					// Configure widgets
					rect.y+=ypos;
					w_skip=new ValueButton(rect);
					w_skip->SetState(WS_NORMAL,winbase[25],0,winbase[25],0);
					w_skip->SetState(WS_HOVER,winbase[30],0,winbase[30],0);
					w_skip->SetState(WS_SELECT,winbase[16],0,winbase[16],0);
					AddWidget(w_skip);
				}
				if(i==CRITICALTV_BUTTON_AUTO){
					// Configure widgets
					rect.y+=ypos;
					w_auto=new ValueButton(rect);
					w_auto->SetState(WS_NORMAL,winbase[i+20],0,winbase[i+20],0);
					w_auto->SetState(WS_HOVER,winbase[i+20+7],0,winbase[i+20+7],0);
					w_auto->SetState(WS_SELECT,winbase[i+20],0,winbase[i+20],0);
					//AddWidget(w_auto);
				}
				if(i==CRITICALTV_BUTTON_HISTORY){
					// Configure widgets
					rect.y+=ypos;
					w_history=new BitmapButton(rect);
					w_history->SetState(WS_NORMAL,winbase[i+8],0);
					w_history->SetState(WS_HOVER,winbase[i+7],0);
					w_history->SetState(WS_SELECT,winbase[i+14],0);
					//AddWidget(w_history);
				}

				if(i==CRITICALTV_BUTTON_REPEAT){
					// Configure widgets
					rect.y+=ypos;
					w_repeat=new BitmapButton(rect);
					w_repeat->SetState(WS_NORMAL,winbase[i],0);
					w_repeat->SetState(WS_HOVER,winbase[i+7],0);
					w_repeat->SetState(WS_SELECT,winbase[i+14],0);
					//AddWidget(w_repeat);
				}
				if(i==CRITICALTV_BUTTON_THING){
					// Configure widgets
					rect.y+=ypos;
					w_thing=new BitmapButton(rect);
					w_thing->SetState(WS_NORMAL,winbase[i],0);
					w_thing->SetState(WS_HOVER,winbase[i+7],0);
					w_thing->SetState(WS_SELECT,winbase[i+14],0);
					//AddWidget(w_thing);
				}*/

			}
			SDL_FreeSurface(winbase[i]);
		}
		delete [] winbase;
	}

	// Speaker name: the original renders it as plain colored text on the
	// window's first row - there is no plate resource for it (WINBASE0.WIP
	// carries 9 frames only: chrome + 8 skip buttons). Native-resolution
	// reference (636x480 AniVisual capture): name ink at local y 10..20,
	// centered inside a 360 px wide field (exe layout constant 0x168,
	// "Devushka" 71 px lands at x 144.5 = (360-71)/2 exactly), and it is
	// smaller than the message text (~12 px vs ~16 px)
	header=new Printer(boxx,boxy+6,360,20);
	header->SetFontSize(14);
	header->SetVisible(false);
	AddWidget(header);

	// The original renders all dialog ink with a black drop shadow by
	// default (registry FontShadow=1, FontEdge=0): both the name and the
	// message text carry a 2px-offset copy beneath the glyphs
	SDL_Color shadow={0x00,0x00,0x00,0};
	header->SetFontShadow(2,2,shadow);
	SetFontShadow(2,2,shadow);
}

CriticalTextview::~CriticalTextview(){
}

bool CriticalTextview::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_load){
		((CriticalPoint*)engine)->EventLoad(998);
		retval=true;
	}
	else if(Object==w_save){
		((CriticalPoint*)engine)->EventSave(998);
		w_save->SetValue(true);
		retval=true;
	}
	else if(Object==w_history){
		((CriticalPoint*)engine)->EventGameDialog(VD_LOG);
	}
	else if(Object==w_skip){
		((CriticalPoint*)engine)->SetSkipmode(true);
		w_skip->SetValue(true);
		retval=true;
	}
	return retval;
}

void CriticalTextview::PrintText(uString Title,uString Text){
	if(header){
		// Name color: the original indexes its color table with the SAY
		// id word (COLORREF 0x00BBGGRR):
		// 0=white 1=blue 2=crimson 3=green 4=magenta 5=cyan 6=yellow.
		// CP scripts only ever use 1 (Elise/Charlotte) and 2 (Reiko)
		static const SDL_Color nametable[7]={
			{0xFF,0xFF,0xFF,0},{0x00,0x80,0xFF,0},{0xFF,0x00,0x80,0},
			{0x00,0xFF,0x00,0},{0xFF,0x00,0xFF,0},{0x00,0xFF,0xFF,0},
			{0xFF,0xFF,0x00,0}};
		int id=GetNameId();
		if(id<0||id>6){
			id=2;
		}
		header->SetFontColor(nametable[id]);
		int w=0,h=0;
		header->GetTextSize(Title.c_str(),&w,&h);
		if(w<=0||w>360){
			w=360;
		}
		header->Move(boxx+(360-w)/2,boxy+6);
		header->Clear();
		header->Print(Title.c_str(),0);
		header->SetVisible(true);
		// The name occupies row 1: the message text drops to row 2
		// (reference text ink tops at local ~32/56/80, 24px pitch)
		SetTextPosition(boxx+12,boxy+32,596,112);
	}
	Textview::PrintText(Text);
}

void CriticalTextview::PrintText(uString Text){
	if(header){
		header->SetVisible(false);
	}
	// Nameless page: the text starts on row 1 right away (reference:
	// 3 nameless lines with ink tops at local ~10/34/58)
	SetTextPosition(boxx+12,boxy+8,596,136);
	Textview::PrintText(Text);
}


