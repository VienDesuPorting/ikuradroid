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

#include "tbutton.h"

/*! \brief Creates a new textbutton and autosizes to the text
 *  \param X Onscreen coordinate
 *  \param Y Onscreen coordinate
 *  \param Caption Text label to display and size to
 */
TextButton::TextButton(int X,int Y,uString Caption)
		: StateWidget(X,Y,0,0) {
	int w,h;
	EDL_SizeText(Caption,Cfg::Font::default_size,&w,&h);
	Resize(w*1.25,h*1.25);
	fhittable=true;
	horizontal=HA_CENTER;
	vertical=VA_CENTER;
	fontsize=Cfg::Font::default_size;
	bfill=true;
	hinvert=false;
	caption=Caption;
	SetColorDefault();

	autogenerate();
}

TextButton::TextButton(int X,int Y,int Width,int Height,uString Caption)
		: StateWidget(X,Y,Width,Height) {
	fhittable=true;
	horizontal=HA_CENTER;
	vertical=VA_CENTER;
	fontsize=Cfg::Font::default_size;
	bfill=true;
	hinvert=false;
	caption=Caption;
	SetColorDefault();
	autogenerate();
}

TextButton::TextButton(SDL_Rect Dst,uString Caption)
		: StateWidget(Dst) {
	fhittable=true;
	horizontal=HA_CENTER;
	vertical=VA_CENTER;
	fontsize=Cfg::Font::default_size;
	bfill=true;
	hinvert=false;
	caption=Caption;
	SetColorDefault();
	autogenerate();
}

TextButton::TextButton(SDL_Rect Dst) : StateWidget(Dst) {
	fhittable=true;
	horizontal=HA_CENTER;
	vertical=VA_CENTER;
	bfill=true;
	hinvert=false;
	SetColorDefault();
}

TextButton::TextButton() : StateWidget() {
	fhittable=true;
	horizontal=HA_CENTER;
	vertical=VA_CENTER;
	bfill=true;
	hinvert=false;
	SetColorDefault();
}

/*! \brief Enables or disables the legacy translucent backing strip
 *
 *  Textless menu items of the original engines draw their captions on
 *  a translucent black strip even when the configured background color
 *  is fully transparent. Items that must blend into an already
 *  translucent window (LMM in-window choices) can disable that
 *  backing altogether.
 */
void TextButton::SetBackgroundFill(bool Enable){
	bfill=Enable;
	autogenerate();
}
/*! \brief Enables the PC-style inverted highlight strip
 *
 *  CP-era Will games highlight the hovered choice by inverting the
 *  composed row (dark window to a light bar, white caption to black)
 *  instead of painting a translucent fill. The hovered rectangle is
 *  queued during rendering and inverted by EDL_FlushHoverInverts()
 *  after the whole widget pass has composed the frame.
 */
void TextButton::SetHoverInvert(bool Enable){
	hinvert=Enable;
	autogenerate();
}


/*! \brief Autoaligns the text in the widget
 *  \param Horizontal Horizontal alignment
 *  \param Vertical Vertical alignment
 *
 *  Please note that the alignment code uses the widget position and size
 *  to calculate coordinates. Do not set alignment before this position is
 *  known or the text will be misplaced.
 *
 *  A possible workaround for this is to set width and height to NULL which
 *  autoresizes the label using the text size.
 */
void TextButton::SetAlignment(HALIGN Horizontal,VALIGN Vertical){
	horizontal=Horizontal;
	vertical=Vertical;
	autogenerate();
}

void TextButton::SetFontSize(int Size){
	fontsize=Size;
	autogenerate();
}

void TextButton::SetColorDefault(){
	// Load default color scheme
	colorfg[WS_NORMAL]=0xA0A0A0FF;
	colorfg[WS_HOVER]=0xFFFFFFFF;
	colorfg[WS_SELECT]=0xFF0000FF;
	colorfg[WS_DISABLE]=0x808080FF;
	colorbg[WS_NORMAL]=0x00000000;
	colorbg[WS_HOVER]=0x00000000;
	colorbg[WS_SELECT]=0x00000000;
	colorbg[WS_DISABLE]=0x00000000;
}

/*! \brief Defines a foreground color (Typically font)
 *  \param State State to get color for
 *  \param Color Color value
 */
void TextButton::SetColorForeground(WIDGET_STATE State,Uint32 Color){
	if(State<=WS_DISABLE){
		colorfg[State]=Color;
		if(State==state){
			autogenerate();
		}
	}
}

/*! \brief Gets a foreground color
 *  \param State State to get color for
 *  \return Color structure
 */
Uint32 TextButton::GetColorForeground(WIDGET_STATE State){
	Uint32 retval=0xFF00FFFF;
	if(State<=WS_DISABLE){
		retval=colorfg[State];
	}
	return retval;
}

/*! \brief Defines a background color
 *  \param State State to get color for
 *  \param Color Color value
 */
void TextButton::SetColorBackground(WIDGET_STATE State,Uint32 Color){
	if(State<=WS_DISABLE){
		colorbg[State]=Color;
		if(State==state){
			autogenerate();
		}
	}
}

/*! \brief Gets a foreground color
 *  \param State State to get color for
 *  \return Color structure
 */
Uint32 TextButton::GetColorBackground(WIDGET_STATE State){
	Uint32 retval=0xFF00FFFF;
	if(State<=WS_DISABLE){
		retval=colorbg[State];
	}
	return retval;
}

void TextButton::ChangeState(WIDGET_STATE State){
	StateWidget::ChangeState(State);
	if(State<WIDGET_STATE_SIZE){
		autogenerate();
	}
}

void TextButton::Resize(int Width,int Height){
	if(pos.w!=Width || pos.h!=Height){
		Widget::Resize(Width,Height);
		autogenerate();
	}
}

void TextButton::autogenerate(){
	if(state<WIDGET_STATE_SIZE){
		WIDGET_STATE es=(hinvert && state==WS_HOVER)?WS_NORMAL:state;

		// Assert a size
		if(pos.w==0 || pos.h==0){
			int w,h;
			if(EDL_SizeText(caption,fontsize,&w,&h)){
				Resize(w,h);
			}
		}
		// Create graphics
		SDL_Surface *txt=EDL_CreateText(caption,colorfg[es],pos.w,fontsize);
		if(txt){
			SDL_Rect td={0,0,txt->w,txt->h};
			if(horizontal==HA_CENTER){
				td.x=(pos.w-txt->w)/2;
			}
			if(horizontal==HA_RIGHT){
				td.x=pos.w-txt->w;
			}
			if(vertical==VA_CENTER){
				td.y=(pos.h-txt->h)/2;
			}
			if(vertical==VA_BOTTOM){
				td.y=pos.h-txt->h;
			}
			SDL_Surface *surface=EDL_CreateSurface(pos.w,pos.h);
			if(colorbg[es]&0xFF){
				Uint8 r=(colorbg[es]>>24)&0xFF;
				Uint8 g=(colorbg[es]>>16)&0xFF;
				Uint8 b=(colorbg[es]>>8)&0xFF;
				Uint8 a=colorbg[es]&0xFF;
				EDL_SetBox(surface,0,0,pos.w,pos.h,r,g,b,a);

				EDL_BlendSurface(txt,0,surface,&td);
			}
	else if(bfill){
		Fill(0,0,0,128);
		EDL_BlitSurface(txt,0,surface,&td);
	}
	else{
		EDL_BlitSurface(txt,0,surface,&td);
	}
			Refresh();
			Set(surface);

			SDL_FreeSurface(surface);
			SDL_FreeSurface(txt);
		}
	}
}

/*! \brief Draws the button and queues the hover strip inversion
 *
 *  The inversion must happen after every underlying widget (scene,
 *  dialog chrome, captions) has been painted, so the rectangle is
 *  queued here and executed by the renderer at the end of the
 *  widget pass.
 */
void TextButton::Render(){
	Widget::Render();
	if(hinvert && state==WS_HOVER && GetVisible()){
		EDL_QueueHoverInvert(pos);
	}
}
