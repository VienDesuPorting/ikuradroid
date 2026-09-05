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

#include "saveslab.h"

SaveSlab::SaveSlab(int X,int Y,int Width,int Height) : Widget(X,Y,Width,Height){
	// Unset surfaces
	sbackground=0;
	soverlay=0;
	sthumb=0;
	sdate=0;
	stext=0;
	// Set defaults
	selected=false;
	fhittable=true;
}

SaveSlab::SaveSlab(SDL_Rect Position) : Widget(Position){
	// Unset surfaces
	sbackground=0;
	soverlay=0;
	sthumb=0;
	sdate=0;
	stext=0;
	// Set defaults
	selected=false;
	fhittable=true;
}

SaveSlab::~SaveSlab(){
	Flush();
}

void SaveSlab::Flush(){
	if(sbackground){
		SDL_FreeSurface(sbackground);
		sbackground=0;
	}
	if(soverlay){
		SDL_FreeSurface(soverlay);
		soverlay=0;
	}
	if(sthumb){
		SDL_FreeSurface(sthumb);
		sthumb=0;
	}
	if(sdate){
		SDL_FreeSurface(sdate);
		sdate=0;
	}
	if(stext){
		SDL_FreeSurface(stext);
		stext=0;
	}
}

void SaveSlab::update(){
    Free();
	if(sbackground){
		Blit(sbackground);
		if(sthumb){
			Blend(sthumb,0,&rthumb);
		}
		if(stext){
			Blend(stext,0,&rtext);
		}
	}
	else{
		if(sthumb){
			Blit(sthumb,0,&rthumb);
		}
		if(stext){
			Blit(stext,0,&rtext);
		}
	}

	if(selected && soverlay){
		Blend(soverlay);
	}
}

void SaveSlab::SetSelected(bool Selected){
	if(Selected!=selected){
		selected=Selected;
		update();
	}
}

/*! \brief Forces a value on the backgrounds alpha channel
 */
void SaveSlab::SetBackgroundAlpha(Uint8 Alpha){
	if(sbackground){
		EDL_SetAlpha(sbackground,0,Alpha);
		update();
	}
}

/*! \brief Sets widget background
 *  \param Src Source graphics
 *  \param SRect Source rectangle
 */
void SaveSlab::SetBackground(SDL_Surface *Src,SDL_Rect *SRect){
	if(Src){
		SDL_Rect srect={0,0,Src->w,Src->h};
		if(SRect){
			srect.x=SRect->x;
			srect.y=SRect->y;
			srect.w=SRect->w;
			srect.h=SRect->h;
		}
		if(sbackground){
			SDL_FreeSurface(sbackground);
		}
		sbackground=EDL_CreateSurface(srect.w,srect.h);
		EDL_BlitSurface(Src,&srect,sbackground,0);
		update();
	}
}

/*! \brief Sets graphics to be blended in when widget is selected
 *  \param Src Source graphics
 *  \param SRect Source rectangle
 */
void SaveSlab::SetOverlay(SDL_Surface *Src,SDL_Rect *SRect){
	if(Src){
		SDL_Rect srect={0,0,Src->w,Src->h};
		if(SRect){
			srect.x=SRect->x;
			srect.y=SRect->y;
			srect.w=SRect->w;
			srect.h=SRect->h;
		}
		if(soverlay){
			SDL_FreeSurface(soverlay);
		}

		soverlay=EDL_CreateSurface(srect.w,srect.h);
		EDL_BlitSurface(Src,&srect,soverlay,0);
		update();
	}
}

/*! \brief Explicitly sets the thumbnail graphics
 *  \param Src Source graphics
 *  \param SRect Source rectangle
 *  \param DRect Destination rectangle
 */
void SaveSlab::SetThumb(SDL_Surface *Src,SDL_Rect *SRect,SDL_Rect *DRect){
	if(Src){
		// Register destination
		if(DRect){
			rthumb.x=DRect->x;
			rthumb.y=DRect->y;
			rthumb.w=DRect->w;
			rthumb.h=DRect->h;
		}
		else{
			rthumb.x=0;
			rthumb.y=0;
			rthumb.w=Src->w;
			rthumb.h=Src->h;
		}

		// Create graphics
		if(sthumb){
			SDL_FreeSurface(sthumb);
		}
		sthumb=EDL_CreateSurface(rthumb.w,rthumb.h);
		EDL_BlitSurface(Src,SRect,sthumb,0);
		update();
	}

}

void SaveSlab::SetDate(Uint64 Unixtime,SDL_Rect *DRect){
}

void SaveSlab::SetText(uString Message,SDL_Rect *DRect){
	if(Message.length()){
		// Register destination
		if(DRect){
			rtext.x=DRect->x;
			rtext.y=DRect->y;
			rtext.w=DRect->w;
			rtext.h=DRect->h;
		}
		else if(stext){
			rtext.x=0;
			rtext.y=0;
			rtext.w=stext->w;
			rtext.h=stext->h;
		}
		else{
			rtext.x=0;
			rtext.y=0;
			rtext.w=pos.w;
			rtext.h=pos.h;
		}

		if(stext){
			SDL_FreeSurface(stext);
		}
		stext=EDL_CreateMultilineText(Message,0xFF0000FF,rtext.w,rtext.h);

		// Update graphics
		update();
	}
}

