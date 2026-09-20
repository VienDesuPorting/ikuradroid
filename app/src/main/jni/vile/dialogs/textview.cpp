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

#include "textview.h"
#include "../engine/evn.h"

Textview::Textview(EngineVN *Engine) : DialogBase(Engine,false) {
	// Set defaults
	w_text=0;
	w_savebutton=0;
	w_loadbutton=0;
	w_optionbutton=0;
	w_backbutton=0;
	w_skipbutton=0;
	w_autobutton=0;
	textlog_flag=true;
	xdisplacement=0;
	ydisplacement=0;
	nameid=0;

	// Always allocate text widget
	w_text=new Printer();
	w_text->SetFontColor(0xFF,0xFF,0xFF);
	AddWidget(w_text);
}

Textview::~Textview(){
	uString *object;
	while((object=(uString*)textlog_stack.Pop())){
		delete object;
	}
}

void Textview::SetSkipmode(bool Skip){
	if(w_skipbutton){
		w_skipbutton->SetValue(Skip);
	}
}

bool Textview::GetSkipmode(){
	if(w_skipbutton){
		return w_skipbutton->GetValue();
	}
	else{
		return false;
	}
}

void Textview::SetSaveButton(BitmapButton *Object){
	w_savebutton=Object;
	AddWidget(Object);
}

void Textview::SetLoadButton(BitmapButton *Object){
	w_loadbutton=Object;
	AddWidget(Object);
}

void Textview::SetSkipButton(ValueButton *Object){
	w_skipbutton=Object;
	AddWidget(Object);
}

void Textview::SetAutoButton(BitmapButton *Object){
	w_autobutton=Object;
	AddWidget(Object);
}

void Textview::SetOptionButton(BitmapButton *Object){
	w_optionbutton=Object;
	AddWidget(Object);
}

void Textview::SetBackButton(BitmapButton *Object){
	w_backbutton=Object;
	AddWidget(Object);
}

bool Textview::GetTextPosition(int *X,int *Y,int *W,int *H){
	if(w_text){
		*X=w_text->GetX();
		*Y=w_text->GetY();
		*W=w_text->GetWidth();
		*H=w_text->GetHeight();
	}
	return w_text;
}

void Textview::SetTextPosition(int X,int Y,int W,int H){
	if(w_text){
		w_text->Move(X,Y);
		w_text->Resize(W,H);
		xdisplacement=0;
		ydisplacement=0;
	}
}

void Textview::SetTextInterval(Uint32 Milliseconds){
	if(w_text){
		w_text->SetInterval(Milliseconds);
	}
}

void Textview::CompleteText(){
	if(w_text){
		w_text->SkipScreen();
	}
}

void Textview::ClearText(){
	if(w_text){
		if(textlog_flag){
			uString *object=(uString*)textlog_stack.Pop();
			if(object && object->length()){
				textlog_stack.Push(object);
			}
			else{
				delete object;
			}
			textlog_stack.Push(new uString);
			if(textlog_stack.Count()>TEXLOG_LINES){
				uString *s=(uString*)textlog_stack.Drop(
						textlog_stack.Count()-1);
				delete s;
			}
		}
		w_text->Clear();
	}
}

void Textview::PrintNewline(){
	if(w_text){
		if(textlog_flag){
			uString *object=(uString*)textlog_stack.Pop();
			if(!object){
				object=new uString;
			}
			*object+="\n";
			textlog_stack.Push(object);
		}
		w_text->Newline();
	}
}

void Textview::PrintText(uString Title,uString Text){
	if(w_text){
		uString newstring="[";
		newstring+=Title;
		newstring+="]\n";
		newstring+=Text;
		PrintText(newstring);
	}
}

void Textview::PrintText(uString Text){
	if(w_text){
		if(textlog_flag){
			uString *object=(uString*)textlog_stack.Pop();
			if(!object){
				object=new uString;
			}
			*object+=Text;
			textlog_stack.Push(object);
		}
		int delay=0;
		if(engine->GetMessageDelayEnabled()){
			delay=engine->GetMessageDelayInterval();
		}
		w_text->Print(Text.c_str(),delay);
	}
}

int Textview::GetRemainingText(){
	if(w_text){
		return w_text->GetRemaining();
	}
	else{
		return 0;
	}
}

void Textview::EnableTextlog(bool Enable){
	textlog_flag=Enable;
}

uString Textview::GetTextlog(int Index){
	uString *retval=(uString*)textlog_stack.Get(Index);
	if(retval){
		return *retval;
	}
	else{
		return "";
	}
}

bool Textview::GetTextSize(uString Text,int *Width,int *Height){
	if(w_text){
		return w_text->GetTextSize(Text.c_str(),Width,Height);
	}
	else{
		return false;
	}
}

void Textview::SetFontFace(uString Filename){
	if(w_text){
		w_text->SetFontFace(Filename.c_str());
	}
}

void Textview::SetFontSize(int Size){
	if(w_text){
		w_text->SetFontSize(Size);
	}
}

void Textview::SetFontColor(Uint8 Red,Uint8 Green,Uint8 Blue){
	if(w_text){
		w_text->SetFontColor(Red,Green,Blue);
	}
}

void Textview::SetFontColor(SDL_Color Color){
	if(w_text){
		w_text->SetFontColor(Color);
	}
}

void Textview::SetFontStyle(int Style){
	if(w_text){
		w_text->SetFontStyle(Style);
	}
}

void Textview::SetFontShadow(int X,int Y,SDL_Color Color){
	if(w_text){
		w_text->SetFontShadow(X,Y,Color);
	}
}

void Textview::SetFontGlow(int Glow,SDL_Color Color){
	if(w_text){
		w_text->SetFontGlow(Glow,Color);
	}
}

/*! \brief Overrides mouse click events, id clicked widget and generates event
 */
bool Textview::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_savebutton){
		// Generate savebutton event
		engine->EventGameDialog(VD_SAVE);
		retval=true;
	}
	else if(Object==w_loadbutton){
		// Generate loadbutton event
		engine->EventGameDialog(VD_LOAD);
		retval=true;
	}
	else if(Object==w_optionbutton){
		// Generate optionbutton event
		engine->EventGameDialog(VD_OPTIONS);
		retval=true;
	}
	else if(Object==w_backbutton){
		retval=true;
	}
	else if(Object==w_skipbutton){
		engine->SetSkipmode(!GetSkipmode());
		retval=true;
	}
	else if(Object==w_autobutton){
		retval=true;
	}
	return retval;
}

