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

#include "selection.h"
#include "../engine/evn.h"

Selection::Selection(EngineVN *Engine) : DialogBase(Engine,false){
	bgscolor=0x000000FF;
	fgscolor=0xFFFFFFFF;
	bgucolor=0x000000FF;
	fgucolor=0xA0A0A0FF;
	halign=HA_LEFT;
	valign=VA_TOP;
	fontsize=18;
	bfill=true;
	swallow=false;
	hinvert=false;
}

Selection::~Selection(){
}

void Selection::Clear(){
	DestroyWidgets();
}

void Selection::SetFontSize(int Fontsize){
	fontsize=Fontsize;
}

void Selection::SetAlignment(HALIGN Horizontal,VALIGN Vertical){
	halign=Horizontal;
	valign=Vertical;
}

void Selection::SetColors(Uint32 BGSColor,Uint32 FGSColor,
		Uint32 BGUColor,Uint32 FGUColor){
	bgscolor=BGSColor;
	fgscolor=FGSColor;
	bgucolor=BGUColor;
	fgucolor=FGUColor;
}

/*! \brief Enables or disables the translucent strips behind text items
 *
 *  Forwarded to every text item created afterwards. The PC originals
 *  of some Will releases (LMM) print their choice captions directly
 *  onto the already translucent dialog window without any backing
 *  graphics.
 */
void Selection::SetBackgroundFill(bool Enable){
	bfill=Enable;
}
/*! \brief Enables or disables swallowing of missed presses
 *
 *  When enabled, presses that land inside the dialog rectangle but
 *  outside every menu item are consumed instead of falling through
 *  to the engine background. This keeps a stray tap from advancing
 *  or skipping text while a choice is pending.
 */
void Selection::SetSwallowMisses(bool Enable){
	swallow=Enable;
}

/*! \brief Enables the PC-style inverted hover strip for all items
 *
 *  Critical Point highlights the hovered choice by inverting the
 *  composed row instead of drawing a translucent fill. Forwarded
 *  to every text item created afterwards.
 */
void Selection::SetHoverInvert(bool Enable){
	hinvert=Enable;
}

/*! \brief Routes a press to the item under the cursor
 *
 *  Lets DialogBase dispatch the press to the focused item, then
 *  consumes misses that land inside the dialog rectangle when
 *  SetSwallowMisses() was enabled.
 */
bool Selection::MouseLeftDown(int X,int Y){
	bool retval=DialogBase::MouseLeftDown(X,Y);
	if(!retval && swallow && !GetWidget(X,Y)){
		retval=Widget::TestMouse(X,Y);
	}
	return retval;
}



/*! \brief Registers two graphical surfaces as a item
 *  \param Selected Surface to use when selection is hovered
 *  \param Unselected Surface to use when selection is not hovered
 *  \param Rect Screen coordinates
 *  \param ID ID to be called back to EventSelect
 */
void Selection::SetSurface(SDL_Surface *Selected,
		SDL_Surface *Unselected,SDL_Rect Rect,int ID){
	// Assert index
	if(ID==-1){
		ID=GetWidgetCount();
	}

	// Create widget
	BitmapButton *button=new BitmapButton();
	button->Resize(Rect.w,Rect.h);
	button->Move(Rect.x,Rect.y);
	button->SetTag(ID);
	if(Selected){
		button->SetState(WS_HOVER,Selected,0);
	}
	if(Unselected){
		button->SetState(WS_NORMAL,Unselected,0);
	}
	AddWidget(button);
}

/*! \brief Adds a textual menu item
 *  \param Caption Text string to display
 *  \param Rect Onscreen position
 *  \param ID Identification code
 *
 *  The ID will be passed as an argument to the EventSelect() event.
 */
void Selection::SetText(uString Caption,SDL_Rect Area,int ID){
	if(Caption.length()){
		// Assert index
		if(ID==-1){
			ID=GetWidgetCount();
		}

		// Create widget
		TextButton *button=new TextButton(Area,Caption);
		button->SetColorBackground(WS_HOVER,bgscolor);
		button->SetColorForeground(WS_HOVER,fgscolor);
		button->SetColorBackground(WS_NORMAL,bgucolor);
		button->SetColorForeground(WS_NORMAL,fgucolor);
		button->SetFontSize(fontsize);
	button->SetAlignment(halign,valign);
	button->SetBackgroundFill(bfill);
	button->SetHoverInvert(hinvert);
	button->SetTag(ID);
		AddWidget(button);
	}
}

/*! \brief Populates a menu from a list of strings
 *  \param Captions Text strings to display
 *  \param BGSColor RGBA value of hovered background
 *  \param FGSColor RGBA value for hovered text
 *  \param BGUColor RGBA value of normal background
 *  \param FGUColor RGBA value for normal text
 */
void Selection::SetText(Stringlist *Captions){
	// Gather statistics
	int count=0;
	uString tmpstr;
	for(int i=0;true;i++){
		if(Captions->GetString(i,&tmpstr)){
			count++;
		}
		else{
			break;
		}
	}

	// Build dialog widgets
	if(count){
		int itemheight=pos.h/(count+1);
		for(int i=0;true;i++){
			if(Captions->GetString(i,&tmpstr)){
				int x=pos.x;
				int y=pos.y+(itemheight/2)+(i*itemheight);
				int w=pos.w;
				int h=itemheight;
				TextButton *button=new TextButton(x,y,w,h,tmpstr);
				button->SetColorBackground(WS_HOVER,bgscolor);
				button->SetColorForeground(WS_HOVER,fgscolor);
				button->SetColorBackground(WS_NORMAL,bgucolor);
				button->SetColorForeground(WS_NORMAL,fgucolor);
				button->SetFontSize(fontsize);
	button->SetAlignment(halign,valign);
	button->SetBackgroundFill(bfill);
	button->SetHoverInvert(hinvert);
	button->SetTag(GetWidgetCount());
				AddWidget(button);
			}
			else{
				break;
			}
		}
	}
}

int Selection::GetFocusItem(){
	int retval=-1;
	Widget *widget=GetFocus();
	if(widget){
		retval=widget->GetTag();
	}
	return retval;
}

/*! \brief Focuses the indexed menu item
 */
void Selection::SetFocusItem(int ID){
	int count=GetWidgetCount();
	if(count){
		Widget *item=GetWidget(count-1);
		while(item && item->GetTag()!=ID){
			item=item->PrevPtr;
		}
		if(!item){
			item=GetWidget(count-1);
		}
		SDL_Rect ipos={item->GetX(),item->GetY(),
			item->GetWidth(),item->GetHeight()};
		if(ipos.w && ipos.h){
			// Get coordinates and warp cursor
			int x=ipos.x+(ipos.w/2);
			int y=ipos.y+(ipos.h/2);
			engine->CursorWarp(x,y);

			// Focus item
			SetFocus(item);
		}
	}
}

bool Selection::InputOk(Widget *Object){
	bool retval=false;
	if(Object){
		engine->EventSelect(Object->GetTag());
		retval=true;
	}
	return retval;
}


