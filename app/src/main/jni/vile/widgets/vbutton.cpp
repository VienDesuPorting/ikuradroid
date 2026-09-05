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

#include "vbutton.h"

ValueButton::ValueButton() : StateWidget(){
	fhittable=true;
	value=false;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		ssurfaces[i]=0;
		usurfaces[i]=0;
	}
}

ValueButton::ValueButton(SDL_Rect Pos) : StateWidget(Pos){
	fhittable=true;
	value=false;
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		ssurfaces[i]=0;
		usurfaces[i]=0;
	}
}

ValueButton::~ValueButton(){
	for(int i=0;i<WIDGET_STATE_SIZE;i++){
		if(ssurfaces[i]){
			SDL_FreeSurface(ssurfaces[i]);
		}
		if(usurfaces[i]){
			SDL_FreeSurface(usurfaces[i]);
		}
	}
}

void ValueButton::ChangeState(WIDGET_STATE State){
	StateWidget::ChangeState(State);

	if(State<WIDGET_STATE_SIZE){
		if(value){
			if(ssurfaces[State]){
				Blit(ssurfaces[State]);
			}
		}
		else{
			if(usurfaces[State]){
				Blit(usurfaces[State]);
			}
		}
	}
}


void ValueButton::SetValue(bool Value){
	value=Value;
	ChangeState(state);
}

bool ValueButton::GetValue(){
	return value;
}

/*! \brief Sets graphics for a specific state
 *  \param State Which set of graphics to use
 *	\param Src Source surface
 *  \param X X coordinate
 *  \param Y Y coordinate
 *  \param W Width
 *  \param H Height
 */
void ValueButton::SetState(int State,
		SDL_Surface *SSurface,int SX,int SY,int SW,int SH,
		SDL_Surface *USurface,int UX,int UY,int UW,int UH){
	SDL_Rect srect={SX,SY,SW,SH};
	SDL_Rect urect={UX,UY,UW,UH};
	SetState(State,SSurface,&srect,USurface,&urect);
}

/*! \brief Sets graphics for a specific state
 *  \param State Which set of graphics to use
 *	\param Src Source surface
 *  \param SRect Part of source to copy
 */
void ValueButton::SetState(int State,
		SDL_Surface *SSurface,SDL_Rect *SRect,
		SDL_Surface *USurface,SDL_Rect *URect){

	if(State<WIDGET_STATE_SIZE && SSurface){
		// Assert destination surface
		if(ssurfaces[State]){
			SDL_FreeSurface(ssurfaces[State]);
		}
		if(SRect){
			ssurfaces[State]=EDL_CreateSurface(SRect->w,SRect->h);
		}
		else{
			ssurfaces[State]=EDL_CreateSurface(GetWidth(),GetHeight());
		}
		// Copy graphics
		EDL_BlitSurface(SSurface,SRect,ssurfaces[State],0);
	}
	if(State<WIDGET_STATE_SIZE && USurface){
		// Assert destination surface
		if(usurfaces[State]){
			SDL_FreeSurface(usurfaces[State]);
		}
		if(URect){
			usurfaces[State]=EDL_CreateSurface(URect->w,URect->h);
		}
		else{
			usurfaces[State]=EDL_CreateSurface(GetWidth(),GetHeight());
		}

		// Copy graphics
		EDL_BlitSurface(USurface,URect,usurfaces[State],0);
	}

	// Update widget graphics
	if(State==state){
		if(value){
			if(ssurfaces[state]){
                Free();
				Blit(ssurfaces[state]);
			}
		}
		else{
			if(usurfaces[state]){
                Free();
				Blit(usurfaces[state]);
			}
		}
	}

}

