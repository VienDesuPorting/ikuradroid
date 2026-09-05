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

#include "idisplay.h"

IkuraDisplay::IkuraDisplay(int Width,int Height) : Hotspot(){
	// Configure size
	Resize(Width,Height);

	// Preset hotspot data
	spots=new Group(0);
	hitmap=0;
	defkey=-1;
	selected=-1;
	result=-1;
	target=0;

	// Unset graphics
	isurface[0]=EDL_CreateSurface(Width,Height);
	for(int i=1;i<GBSIZE;i++){
		isurface[i]=0;
	}
}

IkuraDisplay::IkuraDisplay(SDL_Rect Pos) : Hotspot(Pos){
	// Preset hotspot data
	spots=new Group(0);
	hitmap=0;
	defkey=-1;
	selected=-1;
	result=-1;
	target=0;

	// Unset graphics
	isurface[0]=EDL_CreateSurface(Pos.w,Pos.h);
	for(int i=1;i<GBSIZE;i++){
		isurface[i]=0;
	}
}

IkuraDisplay::~IkuraDisplay(){
	// Remove hotspot data
	delete spots;
	if(hitmap){
		SDL_FreeSurface(hitmap);
	}

	// Free graphics
	for(int i=0;i<GBSIZE;i++){
		if(isurface[i]){
			SDL_FreeSurface(isurface[i]);
		}
	}
}

void IkuraDisplay::SetTarget(int Index){
	target=Index;
}

bool IkuraDisplay::Save(Savegame *File){
	Uint32 count=0;
	for(int i=0;i<GBSIZE;i++){
		if(isurface[i]){
			uString name=EDL_Format("display-%d",i);
			File->SaveSurface(name,isurface[i]);
			count++;
		}
	}
	File->SaveUint32("display-size",count);
	return true;
}

bool IkuraDisplay::Load(Savegame *File){
	for(int i=0;i<GBSIZE;i++){
		if(isurface[i]){
			SDL_FreeSurface(isurface[i]);
			isurface[i]=0;
		}
		SDL_Surface *tmpsurface=0;
		uString name=EDL_Format("display-%d",i);
		if(File->LoadSurface(name,&tmpsurface)){
			isurface[i]=tmpsurface;
		}
	}
	if(isurface[target]){
		Blit(isurface[target]);
	}
	return isurface[target];
}

bool IkuraDisplay::FocusEnter(){
	Refresh();
	return true;
}

bool IkuraDisplay::FocusLeave(){
	Refresh();
	return true;
}

/*! \brief Handles input test for hitmap
 *  \param X coordinate relative to native resolution
 *  \param Y coordinate relative to native resolution
 *  \return True if hitmap gobbled the input
 */
bool IkuraDisplay::TestMouse(int X,int Y){
	bool retval=false;
	if(hitmap){
		 // Find hotspots in preloaded hitmap
		Uint8 *pixel = static_cast<Uint8*>(hitmap->pixels)+
		(Y*hitmap->pitch)+(X*hitmap->format->BytesPerPixel);
		if(pixel[1]!=0xFF){
			retval=true;
		}
	}
	else{
		// Find hotspot represented by widgets
		retval=spots->GetWidget(X,Y);
	}
	return retval;
}

bool IkuraDisplay::MouseMove(int X,int Y){
	bool retval=false;
	if(hitmap){
		 // Find hotspots in preloaded hitmap
		Uint8 *pixel = static_cast<Uint8*>(hitmap->pixels)+
		(Y*hitmap->pitch)+(X*hitmap->format->BytesPerPixel);
		if(pixel[1]!=0xFF){
			selected=pixel[1];
			retval=true;
		}
	}
	else{
		// Find hotspot represented by widgets
		Widget *wptr=spots->GetWidget(X,Y);
		if(wptr){
			selected=wptr->GetTag();
			retval=true;
		}
	}

	return retval;
}

bool IkuraDisplay::MouseLeftDown(int X,int Y){

	bool retval=false;
	if(hitmap){
		 // Find hotspots in preloaded hitmap
		Uint8 *pixel = static_cast<Uint8*>(hitmap->pixels)+
		(Y*hitmap->pitch)+(X*hitmap->format->BytesPerPixel);
		if(pixel[1]!=0xFF){
			selected=pixel[1];
			result=selected;
			retval=true;
		}
	}
	else{
		// Find hotspot represented by widgets
		Widget *wptr=spots->GetWidget(X,Y);
		if(wptr){
			selected=wptr->GetTag();
			result=selected;
			retval=true;
		}
	}
	return retval;
}

bool IkuraDisplay::KeyDown(SDL_Keycode Key){
LogError("KeyDown");

	bool retval=false;
	if(KEY_DIRECTION(Key)){
		retval=true;
		if(selected<0){
			// Set default key selection
			selected=defkey;
			Refresh();
		}
		else{
			// Map next selection
			//keymap.SetUint8(Index,Key,Next);
			int s=selected;
			switch(Key){
				case SDLK_LEFT:		s=keymap.GetUint8(s,2); 	break;
				case SDLK_UP:		s=keymap.GetUint8(s,0); 	break;
				case SDLK_RIGHT:	s=keymap.GetUint8(s,3); 	break;
				case SDLK_DOWN:		s=keymap.GetUint8(s,1); 	break;
				default:										break;
			}
			LogTest("Mapped keyboard %d -> %d",selected,s);
			if(s!=selected){
				selected=s;
				Refresh();
			}
		}
	}
	else if(KEY_ACTION_OK(Key)){
		if(selected>=0){
			result=selected;
			retval=true;
		}
	}
	return retval;
}

/*! \brief Gets pointer to internal surface
 *  \param Index Index of internal surface
 *  \return Pointer to internal surface
 *
 *  Do NOT free the returned surface as it is managed my the display class!
 */
SDL_Surface *IkuraDisplay::GetSurface(int Index){
	SDL_Surface *retval=0;
	if(Index>=0 && Index<GBSIZE && isurface[Index]){
		retval=isurface[Index];
	}
	return retval;
}

int IkuraDisplay::GetSelected(bool Unset){
	int retval=selected;
	if(Unset){
		selected=-1;
		result=-1;
	}
	return retval;
}

int IkuraDisplay::GetResult(bool Unset){
	int retval=result;
	if(Unset){
		selected=-1;
		result=-1;
	}
	return retval;
}

void IkuraDisplay::SetKeyDefault(int Index){
	defkey=Index;
}

void IkuraDisplay::SetKeyMap(int Index,int Key,int Next){
	keymap.SetUint8(Index,Key,Next);
}

void IkuraDisplay::SetMap(SDL_Surface *Hitmap){
	if(hitmap){
		SDL_FreeSurface(hitmap);
	}
	hitmap=EDL_CopySurface(Hitmap,0);
}

void IkuraDisplay::SetSpot(int Index,SDL_Rect Area){
	SetSpot(Index,Area.x,Area.y,Area.w,Area.h);
}

void IkuraDisplay::SetSpot(int Index,int X,int Y,int W,int H){
	if(Index>=0){
		DropSpot(Index);
		Hotspot *wptr=new Hotspot();
		wptr->Move(X,Y);
		wptr->Resize(W,H);
		wptr->SetTag(Index);
		spots->AddWidget(wptr);
	}
}

void IkuraDisplay::DropSpot(int Index){
	if(Index>=0){
		int cnt=0;
		Widget *wptr=spots->GetWidget(cnt++);
		while(wptr){
			if(wptr->GetTag()==(unsigned int)Index){
				spots->DestroyWidget(wptr);
				break;
			}
			wptr=spots->GetWidget(cnt++);
		}
	}
}

void IkuraDisplay::DropMap(){
	SetMap(NULL);
}

void IkuraDisplay::Clear(){
	int cnt=0;
	Widget *wptr=spots->GetWidget(cnt++);
	while(wptr){
		spots->DestroyWidget(wptr);
		wptr=spots->GetWidget(cnt++);
	}
}

/*! \brief Loads a named resource into specified buffer
 *  \param Index Destination buffer index
 *  \param Resource Graphic resource to load
 */
void IkuraDisplay::SetSurface(int Index,SDL_Surface *Surface){
	if(Index>=0 && Index<GBSIZE && Surface){
		// Load to one of the background buffers
		if(isurface[Index]){
			SDL_FreeSurface(isurface[Index]);
		}
		isurface[Index]=EDL_CopySurface(Surface,0);

		// Copy to display
		if(Index==target){
			Blit(isurface[Index]);
		}
	}
}

/*! \brief Configures the specified buffer surface
 *  \param Index Destination buffer index
 *  \param Width Width of the destination buffer
 *  \param Height Height of the destination buffer
 */
void IkuraDisplay::SetSurface(int Index,int Width,int Height){

	if(Index>0 && Index<GBSIZE){
		if(isurface[Index]){
			SDL_FreeSurface(isurface[Index]);
		}
		isurface[Index]=EDL_CreateSurface(Width,Height);
	}

}

/*! \brief Blits a vram surface to another
 *  \param SIndex Source vram index
 *  \param SRect Rectangle to copy from source
 *  \param DIndex Destination vram index
 *  \param DRect Destination rectangle
 */
void IkuraDisplay::BlitSurface(int SIndex,
		SDL_Rect *SRect,int DIndex,SDL_Rect *DRect){
	if(SIndex<GBSIZE && DIndex<GBSIZE && isurface[SIndex]){
		// Assert surfaces
		if(!isurface[DIndex]){
			isurface[DIndex]=EDL_CreateSurface(pos.w,pos.h);
		}

		// Assert minimum size for destination
		SDL_Rect trect={0,0,isurface[DIndex]->w,isurface[DIndex]->h};
		if(DRect){
			trect.x=DRect->x;
			trect.y=DRect->y;
			trect.w=DRect->w;
			trect.h=DRect->h;
		}
		if(isurface[DIndex]->w<trect.w || isurface[DIndex]->h<trect.h){
			SDL_Surface *ts=EDL_CreateSurface(trect.x+trect.w,trect.y+trect.h);
			EDL_BlitSurface(isurface[DIndex],0,ts,0);
			SDL_FreeSurface(isurface[DIndex]);
			isurface[DIndex]=ts;
		}

		// Perform blit
		EDL_BlitSurface(isurface[SIndex],SRect,isurface[DIndex],&trect);

		// Blend to display
		if(DIndex==target){
			Blit(isurface[DIndex]);
		}
	}
	else{
		LogError("Invalid blit: %d -> %d",SIndex,DIndex);
	}
}

void IkuraDisplay::BlendSurface(int SIndex,
		SDL_Rect *SRect,int DIndex,SDL_Rect *DRect){
	if(SIndex<GBSIZE && DIndex<GBSIZE && isurface[SIndex]){
		// Assert surfaces
		if(!isurface[DIndex]){
			isurface[DIndex]=EDL_CreateSurface(pos.w,pos.h);
		}

		// Assert minimum size for destination
		SDL_Rect trect={0,0,isurface[DIndex]->w,isurface[DIndex]->h};
		if(DRect){
			trect.x=DRect->x;
			trect.y=DRect->y;
			trect.w=DRect->w;
			trect.h=DRect->h;
		}
		if(isurface[DIndex]->w<trect.w || isurface[DIndex]->h<trect.h){
			SDL_Surface *ns=EDL_CreateSurface(trect.x+trect.w,trect.y+trect.h);
			EDL_BlitSurface(isurface[DIndex],0,ns,0);
			SDL_FreeSurface(isurface[DIndex]);
			isurface[DIndex]=ns;
		}

		// Perform blit
		EDL_BlendSurface(isurface[SIndex],SRect,isurface[DIndex],DRect);

		// Copy to diplay
		if(DIndex==target){
			Blit(isurface[DIndex]);
		}
	}
	else{
		LogError("Invalid blit: %d -> %d",SIndex,DIndex);
	}
}


/*! \brief Fills a vram surface
 *  \param Index Destination vram buffer
 *  \param R Red color value (0-255)
 *  \param G Green color value (0-255)
 *  \param B Blue color value (0-255)
 */
void IkuraDisplay::FillSurface(int Index,Uint8 R,Uint8 G,Uint8 B){
	if(Index<GBSIZE){
		// Fill target
		if(!isurface[Index]){
			isurface[Index]=EDL_CreateSurface(pos.w,pos.h);
		}
		EDL_SetBox(isurface[Index],0,0,isurface[Index]->w,
				isurface[Index]->h,R,G,B,0xFF);

		// Fill display accordingly
		if(Index==target){
			Fill(R,G,B,0xFF);
		}
	}
}

/*
void IkuraDisplay::ShakeSurface(int Count,int X,int Y,int Duration){
	SDL_Rect srect={0,0,GetWidth(),GetHeight()};
	SDL_Rect erect={X,Y,srect.w,srect.h};
	for(int i=0;i<Count;i++){
		//AnimateWidget(new Slide(pos,display,srect,erect,Duration/2));
		//AnimateWidget(new Slide(pos,display,erect,srect,Duration/2));
	}
}

void IkuraDisplay::FadeSurface(int Source,
		SDL_Rect *Src,SDL_Rect *Dst,int Duration){
	if(Source>0 && Source<GBSIZE && isurface[Source]){
		// Get rects
		SDL_Rect src={0,0,isurface[Source]->w,isurface[Source]->h};
		SDL_Rect dst={0,0,isurface[0]->w,isurface[0]->h};
		if(Src){
			src=*Src;
		}
		if(Dst){
			dst=*Dst;
		}

		// Blit and fade
		if(isurface[0]){
			EDL_BlitSurface(isurface[Source],&src,isurface[0],&dst);
		}
		AnimateWidget(new Fade(dst,isurface[Source],src,Duration));
	}
}

void IkuraDisplay::FadeExternal(SDL_Surface *Source,
		SDL_Rect Src,SDL_Rect Dst,int Duration){
	if(isurface[0]){
		EDL_BlitSurface(Source,&Src,isurface[0],&Dst);
	}
	AnimateWidget(new Fade(Dst,Source,Src,Duration));
}
 */


