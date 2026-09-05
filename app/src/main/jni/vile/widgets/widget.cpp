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

#include "widget.h"
#include "group.h"

bool Widget::global_refresh=false;
SDL_Rect Widget::global_update={0,0,0,0};

Widget::Widget(int X,int Y,int Width,int Height){
	setdefaults();
	pos.x=X;
	pos.y=Y;
	pos.w=Width;
	pos.h=Height;
}

Widget::Widget(SDL_Rect Position){
	setdefaults();
	pos=Position;
}

Widget::Widget(){
	// Preset values
	setdefaults();
}

Widget::~Widget(){
	Free();
}

/*! \brief Helper to initialise object values across constructors
 */
void Widget::setdefaults(){
	// Preset values
	PrevPtr=NextPtr=0;
	simage=0;
	tag=0;
	pos.x=0;
	pos.y=0;
	pos.h=0;
	pos.w=0;
	fhittable=false;
	fvisible=true;
	local_refresh=false;
	local_update.x=0;
	local_update.y=0;
	local_update.w=0;
	local_update.h=0;
	filled=false;
	GlobalXY = false;
}

/*! \brief Gets refresh data without resetting them
 *  \param Cliprect Gets populated with update rect
 *  \return True if display needs refresh
 */
bool Widget::PeekGlobalRefresh(SDL_Rect *Cliprect){
	if(global_refresh && Cliprect){
		*Cliprect=global_update;
	}
	return global_refresh;
}

/*! \brief Gets refresh data and then resets them
 *  \param Cliprect Gets populated with update rect
 *  \return True if display needs refresh
 */
bool Widget::GetGlobalRefresh(SDL_Rect *Cliprect){
	bool retval=global_refresh;
	if(global_refresh && Cliprect){
		*Cliprect=global_update;
	}
	global_update.x=0;
	global_update.y=0;
	global_update.w=0;
	global_update.h=0;
	global_refresh=false;
	return retval;
}

/*! \brief Gets widget refresh data
 *  \param Cliprect Gets populated with update rect
 *  \return True if widget needs refresh
 */
bool Widget::PeekLocalRefresh(SDL_Rect *Cliprect){
	if(local_refresh && Cliprect){
		*Cliprect=local_update;
	}
	return local_refresh;
}

/*! \brief Gets widget refresh data and then resets them
 *  \param Cliprect Gets populated with update rect
 *  \return True if widget needs refresh
 */
bool Widget::GetLocalRefresh(SDL_Rect *Cliprect){
	bool retval=local_refresh;
	if(local_refresh && Cliprect){
		*Cliprect=local_update;
	}
	local_update.x=0;
	local_update.y=0;
	local_update.w=0;
	local_update.h=0;
	local_refresh=false;
	return retval;
}

/*! \brief Flags system for graphic refresh
 *  \param Update Rectangle to update
 */
void Widget::Refresh(SDL_Rect *Update){
	// Set flags and assert update rect
	global_refresh=true;
	local_refresh=true;
	if(!Update){
		Update=&pos;
	}

	// Update rects
	if(!global_update.w && !global_update.h){
		// Update entire rect
		global_update=*Update;
	}
	else{
		// Grow global rect to cover Update
		global_update=EDL_AddRect(global_update,*Update);
	}

	if(!local_update.w && !local_update.h){
		local_update=*Update;
	}
	else{
		// Grow local rect to cover Update
		local_update=EDL_AddRect(local_update,*Update);
	}

}

/*! \brief Forces screen redraw
 */
void Widget::Redraw(){
	//SDL_Surface *video=SDL_GetVideoSurface();
	global_refresh=true;
	global_update.x=0;
	global_update.y=0;
	//global_update.w=video->w;
	//global_update.h=video->h;
	global_update.w=800;
	global_update.h=600;
}

/*! \brief Sets a tag value
 *  \param Tag New tag value
 *
 *  Tags do not have any generic meaning; They are a free-form field that
 *  other parts of the engine use to assign data or identify a widget in a
 *  group of widgets.
 */
void Widget::SetTag(Uint32 Tag){
	tag=Tag;
}

/*! \brief Reads tag value
 *  \returns Read tag value
 */
Uint32 Widget::GetTag(){
	return tag;
}

/*! \brief Moves and resizes the widget on the screen
 *  \param Position New onscreen position for widget
 */
void Widget::Move(SDL_Rect Position){
	Move(Position.x,Position.y);
	Resize(Position.w,Position.h);
}

/*! \brief Moves widget on the screen
 *  \param X X coordinate
 *  \param Y Y coordinate
 */
void Widget::Move(int X,int Y){
	if(pos.x!=X || pos.y!=Y){
		pos.x=X;
		pos.y=Y;
		Refresh();
	}
}

/*! \brief Resizes widget on the screen
 *  \param Position New onscreen position for widget
 */
void Widget::Resize(int W,int H){
	if(pos.w!=W || pos.h!=H){
		pos.w=W;
		pos.h=H;
		Refresh();
	}
}

/*! \brief Tells X position of the widget
 *  \returns X position (Relative to native size)
 */
int Widget::GetX(){
	return pos.x;
}

/*! \brief Tells Y position of the widget
 *  \returns Y position (Relative to native size)
 */
int Widget::GetY(){
	return pos.y;
}

/*! \brief Tells the width of the widget
 *  \returns Widget width position (Relative to native size)
 */
int Widget::GetWidth(){
	return pos.w;
}

/*! \brief Tells the height of the widget
 *  \returns Widget height (Relative to native size)
 */
int Widget::GetHeight(){
	return pos.h;
}

/*! \brief Tells the position of the widget
 *  \returns Widget position rect (Relative to native size)
 */
SDL_Rect Widget::GetPosition(){
	return pos;
}

void Widget::Free(){
	if(simage){
		SDL_FreeSurface(simage);
		simage=0;
		Refresh();
	}
}

/*! \brief Sets widget alpha level
 *	\param Alpha Alphalevel (0=Transparent, 255=Opaque)
 */
void Widget::SetAlpha(Uint8 Alpha,SDL_Rect *Dest){
	EDL_SetAlpha(simage,Dest,Alpha);
}

/*! \brief Sets wether widget should respond to mouse input
 *	\param Show Set to false to hide the widget
 */
void Widget::SetHittable(bool Hittable){
	fhittable=Hittable;
}

/*! \brief Tells wether widget responds to mouse input
 *  \returns TRUE if widget is responsive
 */
bool Widget::GetHittable(){
	return fhittable;
}

/*! \brief Sets widget visibility
 *	\param Show Set to false to hide the widget
 */
void Widget::SetVisible(bool Show){
	if(fvisible!=Show){
		fvisible=Show;
		Refresh();
	}
}

/*! \brief Tells current visibility
 *  \returns TRUE if widget is visible
 */
bool Widget::GetVisible(){
	return fvisible;
}

SDL_Surface *Widget::GetSurface(){
	return simage;
}

void Widget::Fill(Uint32 Color,SDL_Rect *DRect){
	Uint8 r=(Color>>24)&0xFF;
	Uint8 g=(Color>>16)&0xFF;
	Uint8 b=(Color>>8)&0xFF;
	Uint8 a=Color&0xFF;
	Fill(r,g,b,a,DRect);
}

void Widget::Fill(Uint8 R,Uint8 G,Uint8 B,Uint8 A,SDL_Rect *DRect){
	// Assert source
	if(DRect && (!DRect->w || !DRect->h)){
		LogError("Cant fill empty rect");
	}
	else if(!DRect && (!pos.w || !pos.h)){
		LogError("Widget has no boundries");
	}
	else{
		//fill=pos;
		fill.x=0;
		fill.y=0;
		fill.w=pos.w;
		fill.h=pos.h;
		if(DRect){
			fill.x=DRect->x;
			fill.y=DRect->y;
			fill.w=DRect->w;
			fill.h=DRect->h;
		}
		color = {R,G,B,A};

		filled = true;
		Refresh();
	}
}

/*! \brief Converts widget surface to a copy of the source
 *	\param Src Surface to copy from
 *	\param SrcRect Rectangle to copy from the surface (NULL to match position)
 *	\param DRect Rectangle to copy to the surface (NULL for entire surface)
 */
void Widget::Set(SDL_Surface *Src,SDL_Rect *SRect,SDL_Rect *DRect){
	if(Src){
		// Find destination rect
		int w=pos.w;
		int h=pos.h;
		if(DRect){
			w=DRect->w;
			h=DRect->h;
		}
		else if(SRect){
			w=SRect->w;
			h=SRect->h;
		}
		else{
			w=Src->w;
			h=Src->h;
		}

		// Check for resize and reset graphics
		if(w!=pos.w || h!=pos.h){
			Resize(w,h);
		}
		Free();
		Blit(Src,SRect,DRect);
	}
}

/*! \brief Alphablends the given SDL_Rect to the widget surface
 *	\param Src Surface to copy from
 *	\param SrcRect Rectangle to copy from the surface (NULL to match position)
 *	\param DRect Rectangle to copy to the surface (NULL for entire surface)
 */
void Widget::Blend(SDL_Surface *Src,SDL_Rect *SRect,SDL_Rect *DRect){
	if(Src){
		// Assert target image
		if(!simage){
			simage=EDL_CreateSurface(pos.w,pos.h);
		}
		// Copy surface
		if(simage){
			EDL_BlendSurface(Src,SRect,simage,DRect);
			Refresh();
		}
	}
}

/*! \brief Blits the given SDL_rect to the widget surface
 *	\param Src Surface to copy from
 *	\param SrcRect Rectangle to copy from the surface (NULL to match position)
 *	\param DRect Rectangle to copy to the surface (NULL for entire surface)
 */
void Widget::Blit(SDL_Surface *Src,SDL_Rect *SRect,SDL_Rect *DRect){
	if(Src){
		// Assert target image
        if(!simage){
			simage=EDL_CreateSurface(pos.w,pos.h);
		}
		// Copy surface
		if(simage){
			EDL_BlitSurface(Src,SRect,simage,DRect);
			Refresh();
		}
	}
}

void Widget::Dumpfile(uString Filename){
	if(simage){
		if(!Filename.length()){
			char fb[32];
			int cnt=0;
			FILE *fd=0;
			do{
				sprintf(fb,"ViLE%04d.BMP",cnt++);
				fd=fopen(fb,"r");
				if(fd){
					fclose(fd);
				}
			}while(fd);
			Filename=fb;
		}

		// Dump display to file
		LogMessage("Dumping graphics:%s",Filename.c_str());
		SDL_SaveBMP(simage,Filename.c_str());
	}
}

/*! \brief Gets called (very) roughly every 100ms
 */
void Widget::Tick(){
}


void Widget::Render(){
    if(simage){
        EDLTexture *stexture = EDL_CreateTexture(pos.w,pos.h);
        stexture->lockTexture();
        stexture->copyPixels( simage->pixels );
        stexture->unlockTexture();
        stexture->render(pos.x,pos.y);
        stexture->free();
		stexture=0;
    }
    else if (filled)
        boxRGBA(EDLRenderer,pos.x,pos.y,pos.x+pos.w,pos.y+pos.h,color.r,color.g,color.b,color.a);

}


/*! \brief Tests wether coordinates are within the bounds of the widget
 *	\param X Coordinate relative to the native gamesurface
 *	\param Y Coordinate relative to the native gamesurface
 *	\returns TRUE if widget would accept this input
 */


void Widget::SetGlobalXY(bool glxy)
{
    GlobalXY = glxy;
}


bool Widget::TestMouse(int X,int Y){
    if (GlobalXY==true)
        return true;

	int x1=pos.x;
	int y1=pos.y;
	int x2=x1+pos.w;
	int y2=y1+pos.h;

	return fvisible && fhittable && (X>=x1 && X<=x2 && Y>=y1 && Y<=y2);
}

/*! \brief Tests wether the widget is solid (Non-translucent)
 *  \return True if the widget covers what is beneath
 *
 *  This method was added to version 0.4.10 in order to facilitate smarter
 *  composite drawing- and caching techinques. It does not propogate so it
 *  is simple to manage, but its usage is currently experimental.
 */
bool Widget::TestSolid(){
	bool retval=simage && simage->w>=pos.w && simage->h>=pos.h;
	if(retval){
		Uint8 r,g,b,a;
		EDL_GetPixel(simage,1,1,&r,&g,&b,&a);
		if(retval&=(a==0xFF)){
			//EDL_GetPixel(simage,pos.w-1,1,&r,&g,&b,&a);
		}
		if(retval&=(a==0xFF)){
			//EDL_GetPixel(simage,1,pos.h-1,&r,&g,&b,&a);
		}
		if(retval&=(a==0xFF)){
			//EDL_GetPixel(simage,pos.w-1,pos.h-1,&r,&g,&b,&a);
		}
		if(retval&=(a==0xFF)){
			//EDL_GetPixel(simage,pos.w/2,pos.h/2,&r,&g,&b,&a);
			retval&=(a==0xFF);
		}
	}
	return retval;
}

/*! \brief Tests wether the widget accepts keyboard input
 *  \return True if the widget can take keyboard focus
 *
 *  This method was added to version 0.4.10 to improve control and
 *  distribution of keyboard input. Its introduction means that any widgets
 *  that wants keyboard input needs to override it.
 */
bool Widget::TestKey(SDL_Keycode Key){
	return false;
}

bool Widget::FocusEnter(){
	return false;
}

bool Widget::FocusLeave(){
	return false;
}

/*! \brief MouseMove event for widgets
 *  \param X X coordinate
 *  \param Y Y coordinate
 *  \return True if widget gobbles input, false if it was ignored
 */
bool Widget::MouseMove(int X,int Y){
	return false;
}

bool Widget::MouseLeftDown(int X,int Y){
	return false;
}

bool Widget::MouseRightDown(int X,int Y){
	return false;
}

bool Widget::MouseLeftUp(int X,int Y){
	return false;
}

bool Widget::MouseRightUp(int X,int Y){
	return false;
}

bool Widget::KeyUp(SDL_Keycode Key){
	return false;
}

bool Widget::KeyDown(SDL_Keycode Key){
	return false;
}

