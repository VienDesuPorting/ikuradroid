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

#include "stdmsg.h"
#include "../engine/evn.h"

StdMessage::StdMessage(EngineVN *Engine,uString Title,uString Message)
		: DialogBase(Engine,true){
	// Create background graphics
	SDL_Rect rfull={0,0,Engine->NativeWidth(),Engine->NativeHeight()};
	Move(rfull);
	Fill(Cfg::Color::DialogBackground);

	// Set title
	int x,y;
	int space=rfull.h/20;
	if(Title.length()){
		w_title=new Printer();
		w_title->SetFontSize(space*2);
		w_title->SetFontColor(Cfg::Color::DialogHeader);
		w_title->Autosize(Title.c_str());
		w_title->Print(Title.c_str(),0);
		x=(rfull.w-w_title->GetWidth())/2;
		y=rfull.h/6;
		w_title->Move(x,y);
		AddWidget(w_title);
	}
	else{
		w_title=0;
	}

	// Create message
	w_message=new Printer();
	w_message->SetFontSize(space);
	w_message->SetFontColor(Cfg::Color::DialogTopic);
	if(w_title){
		w_message->Move(space*2,w_title->GetY()+w_title->GetHeight()+space);
		w_message->Resize(rfull.w-(space*4),rfull.h-(space*8));
	}
	else{
		w_message->Move(space*2,space*2);
		w_message->Resize(rfull.w-(space*4),rfull.h-(space*8));
	}
	w_message->Print(Message.c_str(),0);
	AddWidget(w_message);

	// Add a button
	int w=rfull.w-(space*2);
	int h=20;
	x=space;
	y=rfull.h-(h+space);
	if(EDL_ReadableFile(Cfg::Path::resource)){
		BitmapButton *w_tmp=new BitmapButton();
		w_tmp->Move(x,y);
		w_tmp->Resize(w,h);
		w_tmp->SetCaption("OK");
		w_ok=w_tmp;
	}
	else{
		w_ok=new TextButton(x,y,w,h,"OK");
	}
	AddWidget(w_ok);
}

StdMessage::~StdMessage(){
	DestroyWidgets();
}

bool StdMessage::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_ok){
		// Close dialog
		engine->DestroyWidget(this);
		retval=true;
	}
	return retval;
}

