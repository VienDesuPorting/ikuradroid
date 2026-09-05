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

#include "dlgbase.h"
#include "../engine/evn.h"

/*! \brief Creates a new dialog object
 *  \param Blockinput Wether to trap all user input
 */
DialogBase::DialogBase(EngineVN *Engine,bool Blockinput) : Widget() {
	fhittable=true;
	engine=Engine;
	block=Blockinput;
	layer=new Group(0);
	defindex=0;
	focus=0;
}

DialogBase::~DialogBase(){
	// Destroy widgets
	delete layer;
}

Widget *DialogBase::GetFocus(){
	return focus;
}

/*! \brief Sets focus to a specific widget
 *  \param WidgetPtr Widget to attain focus
 *  \return True if successfull
 */
bool DialogBase::SetFocus(Widget *WidgetPtr){
	Widget *wptr=WidgetPtr;
	bool retval=engine->SetFocusWidget(this);
	if(retval){
		retval=CheckWidget(wptr);
		if(CheckWidget(wptr)){
			if(focus!=wptr){
				if(!FocusEnter(wptr)){
					wptr->FocusEnter();
				}
				if(CheckWidget(focus)){
					if(!FocusLeave(focus)){
						focus->FocusLeave();
					}
				}
				focus=wptr;
			}
		}
	}
	return retval;
}

/*! \brief Adds precreated widget
 *  \param WidgetPtr Pointer to precreated widget
 *  \returns Pointer to widget
 */
Widget *DialogBase::AddWidget(Widget *WidgetPtr){
	layer->AddWidget(WidgetPtr);
	return WidgetPtr;
}

/*! \brief Remove and destroy a given widget
 *  \param WidgetPtr Pointer to widget to destroy
 */
void DialogBase::DestroyWidget(Widget *WidgetPtr){
	layer->DestroyWidget(WidgetPtr);
}

/*! \brief Remove and destroy all widgets in the dialog
 */
void DialogBase::DestroyWidgets(){
	layer->DestroyGroup();
}

Widget *DialogBase::Select(){
	Widget *retval=focus;
	if(CheckWidget(focus)){
		focus->FocusLeave();
		focus=0;
	}
	return retval;
}

void DialogBase::MoveDialog(int X,int Y){
	Move(X,Y);
	Widget *wptr=layer->GetWidget(0);
	while(wptr){
		wptr->Move(wptr->GetX()+X,wptr->GetY()+Y);
		wptr=wptr->NextPtr;
	}
}

/*! \brief Retrieves widget at given index
 *  \param Index Index starting with 0
 *  \param Y Y coordinate
 *  \returns Pointer to widget at given coordinates or NULL
 */
Widget *DialogBase::GetWidget(int Index){
	return layer->GetWidget(Index);
}

/*! \brief Retrieves widget at given spot
 *  \param X X coordinate
 *  \param Y Y coordinate
 *  \returns Pointer to widget at given coordinates or NULL
 */
Widget *DialogBase::GetWidget(int X,int Y){
	return layer->GetWidget(X,Y);
}

int DialogBase::GetWidgetCount(){
	return layer->GetWidgetCount();
}

/*! \brief Verifies that a given widget is in dialogs layer
 *  \param WidgetPtr Pointer to check
 *  \returns True if pointer was found in dialog layer
 */
bool DialogBase::CheckWidget(Widget *WidgetPtr){
	Widget *wptr=layer->GetWidget(0);
	while(wptr){
		if(wptr==WidgetPtr){
			return true;
		}
		else{
			wptr=wptr->NextPtr;
		}
	}
	return false;
}

bool DialogBase::GetLocalRefresh(SDL_Rect *Cliprect){
	Widget *wptr=layer->GetWidget(0);
	SDL_Rect r;
	while(wptr){
		if(wptr->GetLocalRefresh(&r)){
			Refresh(&r);
		}
		wptr=wptr->NextPtr;
	}
	return Widget::GetLocalRefresh();
}

bool DialogBase::PeekLocalRefresh(SDL_Rect *Cliprect){
	Widget *wptr=layer->GetWidget(0);
	SDL_Rect r;
	while(wptr){
		if(wptr->GetLocalRefresh(&r)){
			Refresh(&r);
		}
		wptr=wptr->NextPtr;
	}
	return Widget::PeekLocalRefresh();
}

bool DialogBase::TestMouse(int X,int Y){
	if(block){
		// Trap all input
		//return true;
		return fvisible;
	}
	else{
		return Widget::TestMouse(X,Y);
	}
}

/*! \brief Tests wether dialog or any of the embedded widgets takes key focus
 *  \param Key Key code to test with
 *  \return True if keyboard input was accepted
 */
bool DialogBase::TestKey(SDL_Keycode Key){
	bool retval=Widget::TestKey(Key);
	if(CheckWidget(focus)){
		retval=focus->TestKey(Key);
	}
	return retval;
}

bool DialogBase::FocusEnter(){
	bool retval=false;
	if(CheckWidget(focus)){
		if(!(retval=FocusEnter(focus))){
			retval=focus->FocusEnter();
		}
	}
	return retval;
}

bool DialogBase::FocusLeave(){
	bool retval=false;
	if(CheckWidget(focus)){
		if(!(retval=FocusLeave(focus))){
			retval=focus->FocusLeave();
		}
	}
	return retval;
}

bool DialogBase::FocusEnter(Widget *Object){
	return false;
}

bool DialogBase::FocusLeave(Widget *Object){
	return false;
}

/*! \brief Takes input and determines focus before passing event down to widget
 *  \param X X Coordinate relatative to games native resolution
 *  \param Y Y Coordinate relatative to games native resolution
 *  \return True if a widget accepted the input
 */
bool DialogBase::MouseMove(int X,int Y){
	// Set widget focus
	bool retval=false;
	Widget *wptr=GetWidget(X,Y);
	if(wptr && wptr!=focus){
		if(wptr->GetVisible() && wptr->GetHittable()){
			if(!FocusEnter(wptr)){
				wptr->FocusEnter();
			}
			if(CheckWidget(focus)){
				if(!FocusLeave(focus)){
					focus->FocusLeave();
				}
			}
			focus=wptr;
		}
	}
	else if(!wptr && focus){
		if(CheckWidget(focus)){
			if(!FocusLeave(focus)){
				focus->FocusLeave();
			}
		}
		LogDebug("Mouse unsets dialog focus");
		focus=0;
	}

	// Pass down mousemove event
	if(!(retval=MouseMove(wptr,X,Y))){
		if(wptr){
			retval=wptr->MouseMove(X,Y);
		}
	}
	return retval|block;
}

/*! \brief Takes input and passes event down to widget
 *  \param X X Coordinate relatative to games native resolution
 *  \param Y Y Coordinate relatative to games native resolution
 *  \return True if a widget accepted the input
 */
bool DialogBase::MouseLeftDown(int X,int Y){
	bool retval=false;
	Widget *wptr=GetWidget(X,Y);
	if(wptr){
		// Pass down event
		if(!(retval=MouseLeftDown(wptr,X,Y))){
			if(!(retval=InputOk(wptr))){
				retval=wptr->MouseLeftDown(X,Y);
			}
		}
	}
	return retval|block;
}

/*! \brief Takes input and passes event down to widget
 *  \param X X Coordinate relatative to games native resolution
 *  \param Y Y Coordinate relatative to games native resolution
 *  \return True if a widget accepted the input
 */
bool DialogBase::MouseRightDown(int X,int Y){
	bool retval=false;
	Widget *wptr=GetWidget(X,Y);
	if(wptr){
		// Pass down event
		if(!(retval=MouseRightDown(wptr,X,Y))){
			if(!(retval=InputCancel(wptr))){
				retval=wptr->MouseRightDown(X,Y);
			}
		}
	}
	return retval|block;
}

/*! \brief Takes input and passes event down to widget
 *  \param X X Coordinate relatative to games native resolution
 *  \param Y Y Coordinate relatative to games native resolution
 *  \return True if a widget accepted the input
 */
bool DialogBase::MouseLeftUp(int X,int Y){
	bool retval=false;
	Widget *wptr=GetWidget(X,Y);
	if(wptr){
		// Pass down event
		if(!(retval=MouseLeftUp(wptr,X,Y))){
			retval=wptr->MouseLeftUp(X,Y);
		}

		// Unset focus if the event was accepted
		if(retval){
			if(CheckWidget(wptr)){
				MouseMove(X,Y);
			}
		}
	}
	return retval|block;
}

/*! \brief Takes input and passes event down to widget
 *  \param X X Coordinate relatative to games native resolution
 *  \param Y Y Coordinate relatative to games native resolution
 *  \return True if a widget accepted the input
 */
bool DialogBase::MouseRightUp(int X,int Y){
	bool retval=false;
	Widget *wptr=GetWidget(X,Y);
	if(wptr){
		// Pass down event
		if(!(retval=MouseRightUp(wptr,X,Y))){
			retval=wptr->MouseRightUp(X,Y);
		}

		// Unset focus if the event was accepted
		if(retval){
			MouseMove(X,Y);
		}
	}
	return retval|block;
}

/*! \brief Distributes keyboard input to a focused widget
 *  \param Key Passed key
 *  \return True if widget accepted the event
 *
 *  Passing false back to the engine drops engine focus, only do this when
 *  focus is being willfully released or when no child widgets can accept
 *  the input.
 */
bool DialogBase::KeyDown(SDL_Keycode Key){
	bool retval=false;
	if(CheckWidget(focus)){
		if(KEY_REFOCUS(Key)){
			// Ignore reserved key
			retval=true;
		}
		else if(KeyDown(focus,Key)){
			// Event was handled by the dialog
			LogDebug("Dialog handled keyboard event");
			retval=true;
		}
		else if(KEY_ACTION_OK(Key) && InputOk(focus)){
			// Event was handled by the dialog
			LogDebug("Dialog handled ok event");
			retval=true;
		}
		else if(KEY_ACTION_CANCEL(Key) && InputCancel(focus)){
			// Event was handled by the dialog
			LogDebug("Dialog handled cancel event");
			retval=true;
		}
		else if(focus->KeyDown(Key)){
			// Event was handled by the widget
			LogDebug("Keyboard event passed to widget");
			retval=true;
		}
		else if(KEY_DIRECTION(Key)){
			// Change focus
			InputDirection(Key);
			retval=true;
		}
		else if(KEY_ACTION_CANCEL(Key)){
			// Escape focus
			LogDebug("Dialog cancelled keyboard focus");
			if(!FocusLeave(focus)){
				focus->FocusLeave();
			}
			focus=0;
		}
		else{
			// Ignore keys per default
			retval=true;
		}
	}
	else if(KEY_DIRECTION(Key) || KEY_REFOCUS(Key)){
		// Search for first available focus
		focus=0;
		for(int i=layer->GetWidgetCount()-1;!focus && i>=0;i--){
			focus=GetWidget(i);
			if(focus){
				if(focus->GetVisible() && focus->GetHittable()){
					if(!FocusEnter(focus)){
						focus->FocusEnter();
					}
					retval=true;
					break;
				}
				else{
					focus=0;
				}
			}
		}
		if(retval){
			LogDebug("Dialog reset focus");
		}
		else{
			LogDebug("Dialog refrained focus");
		}
	}
	else{
		// No valid focus or related keys
		LogDebug("Dialog refrained keyboard event");
	}
	return retval|block;
}

bool DialogBase::KeyUp(SDL_Keycode Key){
	//LogMessage("DIALOG:KEYUP:%d",Key);
	bool retval=true;
	if(CheckWidget(focus)){
		if(KEY_REFOCUS(Key)){
			// Ignore reserved key
		}
		else if(!(retval=KeyUp(focus,Key))){
			// Pass all keyup events to widgets
			retval=focus->KeyUp(Key);
		}
	}
	return retval;
}

bool DialogBase::InputDirection(SDL_Keycode Key){
	bool retval=false;
	if(KEY_DIRECTION(Key) && CheckWidget(focus)){
		// Find current items center position
		Widget *test=0;
		Widget *candidate=0;
		int fx1,fy1,fx2,fy2;

		// Vertical navigation
		if(KEY_DIRECTION_V(Key)){
			fy1=focus->GetY();
			fy2=fy1+focus->GetHeight();
			fx1=fx2=focus->GetX()+(focus->GetWidth()/2);
			for(int i=0;!candidate && i<pos.w;i+=5){
				int lfx1=EDL_MAX(fx1-i,0);
				int lfx2=EDL_MIN(fx2+i,pos.x+pos.w);
				for(int j=0;(test=GetWidget(j));j++){
					int tx1=test->GetX();
					int ty1=test->GetY();
					int tx2=tx1+test->GetWidth();
					int ty2=ty1+test->GetHeight();
					bool l1=Key==SDLK_UP?(fy1>ty1):(fy2<ty2);
					bool l2=(lfx1<tx1 && tx1<lfx2);
					bool l3=(lfx1<tx2 && tx2<lfx2);
					bool l4=(lfx1>tx1 && tx2>lfx2);
					if(test!=focus && l1 && (l2 || l3 || l4)){
						if(test->GetVisible() && test->GetHittable()){
							// We got an eligable candidate
							if(candidate){
								// Compare object to current candidate
								int cy1=candidate->GetY();
								int cy2=cy1+candidate->GetHeight();
								if(Key==SDLK_UP?(ty2>cy2):(ty1<cy1)){
									candidate=test;
								}
							}
							else{
								candidate=test;
							}
						}
					}
				}
			}
		}

		// Horisontal navigation
		if(KEY_DIRECTION_H(Key)){
			fx1=focus->GetX();
			fx2=fx1+focus->GetWidth();
			fy1=fy2=focus->GetY()+(focus->GetHeight()/2);
			for(int i=0;!candidate && i<pos.h;i+=5){
				int lfy1=EDL_MAX(fy1-i,0);
				int lfy2=EDL_MIN(fy2+i,pos.y+pos.h);
				for(int j=0;(test=GetWidget(j));j++){
					int tx1=test->GetX();
					int ty1=test->GetY();
					int tx2=tx1+test->GetWidth();
					int ty2=ty1+test->GetHeight();
					bool l1=Key==SDLK_LEFT?(fx1>tx1):(fx2<tx2);
					bool l2=(lfy1<ty1 && ty1<lfy2);
					bool l3=(lfy1<ty2 && ty2<lfy2);
					bool l4=(lfy1>ty1 && ty2>lfy2);
					if(test!=focus && l1 && (l2 || l3 || l4)){
						if(test->GetVisible() && test->GetHittable()){
							// We got an eligable candidate
							if(candidate){
								// Compare object to current candidate
								int cx1=candidate->GetX();
								int cx2=cx1+candidate->GetWidth();
								if(Key==SDLK_LEFT?(tx2>cx2):(tx1<cx1)){
									candidate=test;
								}
							}
							else{
								candidate=test;
							}
						}
					}
				}
			}
		}

		// Refocus
		if(candidate){
			if(!FocusLeave(focus)){
				focus->FocusLeave();
			}
			if(!FocusEnter(candidate)){
				candidate->FocusEnter();
			}
			focus=candidate;
			retval=true;
		}
	}
	return retval;
}

/*! \brief Dialog event that is called when an OK button has been pressed
 *  \param Object Object that recieved the event
 *  \return True if the event was accepted
 */
bool DialogBase::InputOk(Widget *Object){
	return false;
}

/*! \brief Dialog event that is called when an CANCEL button has been pressed
 *  \param Object Object that recieved the event
 *  \return True if the event was accepted
 */
bool DialogBase::InputCancel(Widget *Object){
	return false;
}

/*! \brief Input event for widgets embedded in a dialog (Dialog event)
 *  \param Object Pointer to widget (NULL for loss of focus)
 *  \param X X coordinate
 *  \param Y Y coordinate
 *  \return True if dialog accepted the event
 */
bool DialogBase::MouseMove(Widget *Object,int X,int Y){
	return false;
}

bool DialogBase::MouseLeftUp(Widget *Object,int X,int Y){
	return false;
}

bool DialogBase::MouseRightUp(Widget *Object,int X,int Y){
	return false;
}

bool DialogBase::MouseLeftDown(Widget *Object,int X,int Y){
	return false;
}

bool DialogBase::MouseRightDown(Widget *Object,int X,int Y){
	return false;
}

bool DialogBase::KeyUp(Widget *Object,SDL_Keycode Key){
	return false;
}

bool DialogBase::KeyDown(Widget *Object,SDL_Keycode Key){
	return false;
}

void DialogBase::Tick(){
	Widget *wptr=layer->GetWidget(0);
	Widget *pptr=0;
	while(CheckWidget(wptr)){
		// Distribute event
		wptr->Tick();

		// Check for dropped items and iterate
		if(CheckWidget(wptr)){
			pptr=wptr;
			wptr=wptr->NextPtr;
		}
		else if(CheckWidget(pptr)){
			wptr=pptr->NextPtr;
			pptr=0;
		}
		else{
			LogError("Invalid stackpointer in dialog");
			break;
		}
	}
}

 void DialogBase::Render(){
     if(fvisible){
		// Copy own graphics as background
		Widget::Render();

		// Copy children widgets graphics over our own
		Widget *wptr=layer->GetWidget(layer->GetWidgetCount()-1);
		while(wptr){
			if(wptr->GetVisible()){
				wptr->Render();
			}
			wptr=wptr->PrevPtr;
		}
	}
 }
