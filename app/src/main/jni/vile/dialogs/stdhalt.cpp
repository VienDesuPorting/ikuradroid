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

#include "stdhalt.h"
#include "../engine/evn.h"

bool StdHalt::showing=false;

StdHalt::StdHalt(EngineVN *Engine) : DialogBase(Engine,true){
	// Create background graphics
	SDL_Rect rfull={0,0,Engine->NativeWidth(),Engine->NativeHeight()};
	Move(rfull);
	Fill(Cfg::Color::DialogBackground);

	// Default title
	int x,y;
	int space=rfull.h/20;
	w_title=new Printer();
	w_title->SetFontSize(space*2);
	w_title->SetFontColor(Cfg::Color::DialogHeader);
	w_title->Autosize("Exit game?");
	w_title->Print("Exit game?",0);
	x=(rfull.w-w_title->GetWidth())/2;
	y=rfull.h/4;
	w_title->Move(x,y);
	AddWidget(w_title);

	// the buttons
	int w=rfull.w/4;
	int h=20;
	x=rfull.w/2;
	y=(rfull.h/4)*3;
	w_ok=new BitmapButton();
	w_ok->Move(x-(w+space),y);
	w_ok->Resize(w,h);
	w_ok->SetCaption("OK");
	w_cancel=new BitmapButton();
	w_cancel->Move(x+space,y);
	w_cancel->Resize(w,h);
	w_cancel->SetCaption("Cancel");
	AddWidget(w_ok);
	AddWidget(w_cancel);

	// Prevent dialog from loading multiple times
	if(showing){
		engine->EventGameShutdown();
	}
	else{
		showing=true;
	}
}

StdHalt::~StdHalt(){
	DestroyWidgets();
	showing=false;
}

bool StdHalt::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_ok){
		// Close dialog
		engine->DestroyWidget(this);
		engine->EventGameShutdown();
		retval=true;
	}
	if(Object==w_cancel){
		// Close dialog
		engine->DestroyWidget(this);
		retval=true;
	}
	return retval;
}

