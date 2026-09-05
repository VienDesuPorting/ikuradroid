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

#include "cwaremap.h"
#include "cware.h"

CWareMap::CWareMap(EngineCWare *Engine) : DialogBase(Engine,false){
	Resize(576,376);
	Move(32,8);
	bg=0;
	fg=0;
}

CWareMap::~CWareMap(){
	if(bg){
		SDL_FreeSurface(bg);
	}
	if(fg){
		SDL_FreeSurface(fg);
	}
}

bool CWareMap::InputOk(Widget *Object){
	bool retval=false;
	if(Object && Object->GetTag()>0){
		((EngineCWare*)engine)->Jump(Object->GetTag());
		SetVisible(false);
		retval=true;
	}
	return retval;
}
	
/*! \brief Flush all items
 */
void CWareMap::Flush(){
	DestroyWidgets();
	istring="";
}

/*! \brief Loads old mapdata from a string
 *  \param MapSettings String to load (Usually from SaveMap())
 *  \returns True if map data was valid
 */
bool CWareMap::LoadMap(uString MapSettings){
	return false;
}

/*! \brief Generates a string that can be used to reload current map
 *  \returns Map savestring
 */
uString CWareMap::SaveMap(){
	uString retval=bgstring;
	retval+=":";
	retval+=fgstring;
	retval+=istring;
	return retval;
}

/*! \brief Adds a location to the map
 *  \param X1 Screen coordinate
 *  \param Y1 Screen coordinate
 *  \param X2 Screen coordinate
 *  \param Y2 Screen coordinate
 *  \param Position Jump position for this item
 */
bool CWareMap::AddLocation(int X1,int Y1,int X2,int Y2,unsigned int Position){
	if(bg && fg){
		// Create button
		SDL_Rect r={X1,Y1,X2-X1,Y2-Y1};
		BitmapButton *button=new BitmapButton(r);
		button->SetState(WS_NORMAL,bg,&r);
		button->SetState(WS_HOVER,fg,&r);
		button->Move(r.x+32,r.y+8);
		button->SetTag(Position);
		AddWidget(button);

		// Add item to savestring
		istring+=EDL_Format(":%d:%d:%d:%d:%u",X1,Y1,X2,Y2,Position);
	}
	return bg && fg;
}

/*! \brief Registers an image for the background image
 *  \param Name Name of the image resource
 *  \returns True if a valid image resource was found
 */
bool CWareMap::SetBackground(uString Name){
	bool retval=false;
	if(bg){
		SDL_FreeSurface(bg);
		bg=0;
	}
	if((bg=engine->LoadImage(Name))){
		Blit(bg);
		bgstring=Name;
		retval=true;
	}
	return retval;
}

/*! \brief Registers an image for the foreground overlay
 *  \param Name Name of the image resource
 *  \returns True if a valid image resource was found
 */
bool CWareMap::SetForeground(uString Name){
	bool retval=false;
	if(fg){
		SDL_FreeSurface(fg);
		fg=0;
	}
	if((fg=engine->LoadImage(Name))){
		fgstring=Name;
		retval=true;
	}
	return retval;
}

