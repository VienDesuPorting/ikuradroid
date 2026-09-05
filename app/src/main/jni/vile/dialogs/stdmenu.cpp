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

#include "stdmenu.h"
#include "../engine/evn.h"

StdMenu::StdMenu(EngineVN *Engine) : DialogBase(Engine,false){
	int w=Engine->NativeWidth()/SI_ITEM_COUNT;
	int y=2;
	int x=10;
	w_items[SI_NEW]=new TextButton(x+(w*0),y,"New");
	w_items[SI_LOAD]=new TextButton(x+(w*1),y,"Load");
	w_items[SI_SAVE]=new TextButton(x+(w*2),y,"Save");
	w_items[SI_OPTIONS]=new TextButton(x+(w*3),y,"Options");
	w_items[SI_TITLE]=new TextButton(x+(w*4),y,"Title");
	w_items[SI_EXIT]=new TextButton(x+(w*5),y,"Exit");

	Uint32 hover=0x000000FF;
	//Uint32 hover=0xFFFFFFFF;
	Uint32 normal=0x7F7F7FFF;
	for(int i=0;i<SI_ITEM_COUNT;i++){
		w_items[i]->SetColorForeground(WS_NORMAL,normal);
		w_items[i]->SetColorForeground(WS_HOVER,hover);
		w_items[i]->FocusEnter();
		w_items[i]->FocusLeave();
		AddWidget(w_items[i]);
	}

	// Create base graphics
	Resize(Engine->NativeWidth(),w_items[SI_NEW]->GetHeight()+(y*2));
	//Fill(0xFF,0xFF,0xFF,0xFF);
	//s_gfx=EDL_CreateSurface(simage->w,simage->h);
	//s_gfx=EDL_CreateSurface(640,50);
    //SDL_SetSurfaceBlendMode(s_gfx,SDL_BLENDMODE_BLEND);
    //SDL_SetSurfaceAlphaMod(g,254);
   // SDL_SetSurfaceBlendMode(g,SDL_BLENDMODE_BLEND);

	//EDL_BlitSurface(simage,0,s_gfx,0);

	// Hide dialog
	visible=true;
	ShowMenu(false);
}

StdMenu::~StdMenu(){
	DestroyWidgets();
	//SDL_FreeSurface(s_gfx);
}

/*! \brief Shows or hides the menu
 *  \param Visible True to show, false to hide
 */
void StdMenu::ShowMenu(bool Visible){
	if(Visible!=visible){
		visible=Visible;
		w_items[SI_NEW]->SetVisible(Visible);
		w_items[SI_LOAD]->SetVisible(Visible);
		w_items[SI_SAVE]->SetVisible(Visible);
		w_items[SI_OPTIONS]->SetVisible(Visible);
		w_items[SI_TITLE]->SetVisible(Visible);
		w_items[SI_EXIT]->SetVisible(Visible);
		if(Visible){
            Fill(0xFF,0xFF,0xFF,0xFF);
			//Blit(s_gfx);
		}
		else{
		    Fill(0x00,0x00,0x00,0x00);
			Free();
		}
	}
}

/*! \brief Overrides event to automatically show the dropdown menu
 *  \return Value from DialogBase
 */
bool StdMenu::FocusEnter(){
	if(!visible){
		ShowMenu(true);
	}
	return DialogBase::FocusEnter();
}

/*! \brief Overrides event to automatically hide the dropdown menu
 *  \return Value from DialogBase
 */
bool StdMenu::FocusLeave(){
	if(visible){
		ShowMenu(false);
	}
	return DialogBase::FocusLeave();
}

/*! \brief Hook user input and generate appropriate engine events
 *  \param Object Clicked widget
 *  \return Always true
 */
bool StdMenu::InputOk(Widget *Object){
	if(Object==w_items[SI_NEW]){
		engine->EventNew();
	}
	if(Object==w_items[SI_LOAD]){
		engine->EventGameDialog(VD_LOAD);
	}
	if(Object==w_items[SI_SAVE]){
		engine->EventGameDialog(VD_SAVE);
	}
	if(Object==w_items[SI_OPTIONS]){
		engine->EventGameDialog(VD_OPTIONS);
	}
	if(Object==w_items[SI_TITLE]){
		engine->EventGameDialog(VD_TITLE);
	}
	if(Object==w_items[SI_EXIT]){
		engine->EventGameDialog(VD_SHUTDOWN);
	}
	return true;
}

