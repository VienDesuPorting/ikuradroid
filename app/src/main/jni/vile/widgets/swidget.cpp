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

#include "swidget.h"

StateWidget::StateWidget() : Widget(){
	state=WS_NORMAL;
}

StateWidget::StateWidget(SDL_Rect Pos) : Widget(Pos){
	state=WS_NORMAL;
}

StateWidget::StateWidget(int X,int Y,int W,int H) : Widget(X,Y,W,H){
	state=WS_NORMAL;
}

void StateWidget::ChangeState(WIDGET_STATE State){
	if(State<WIDGET_STATE_SIZE){
		state=State;
	}
}

bool StateWidget::FocusEnter(){
	if(state!=WS_HOVER){
		ChangeState(WS_HOVER);
	}
	return true;
}

bool StateWidget::FocusLeave(){
	if(state!=WS_NORMAL){
		ChangeState(WS_NORMAL);
		Refresh();
	}
	return true;
}

bool StateWidget::MouseLeftUp(int X,int Y){
	if(state!=WS_SELECT){
		ChangeState(WS_SELECT);
	}
	return true;
}

bool StateWidget::MouseLeftDown(int X,int Y){
	if(state==WS_HOVER){
		ChangeState(WS_HOVER);
	}
	return true;
}

bool StateWidget::KeyUp(SDL_Keycode Key){
	if(state==WS_SELECT){
		ChangeState(WS_NORMAL);
	}
	return true;
}

bool StateWidget::KeyDown(SDL_Keycode Key){
	if(KEY_ACTION_OK(Key)){
		ChangeState(WS_SELECT);
		return true;
	}
	else{
		return false;
	}
}

