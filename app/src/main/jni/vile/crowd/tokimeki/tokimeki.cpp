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

#include "tokimeki.h"

Tokimeki::Tokimeki(uString Path) : EngineCrowd1(640,480) {
	// Load XChange 1 resources
	AddScripts(new ArchiveCrowdSCE(Path+"scene00.bdt"));
	AddVoices(new ArchiveCrowdSNN(Path+"packwav.snn"));
	AddImages(new ArchiveFiles(Path+"*.zbm"));
	AddBGM(new ArchiveFiles(Path+"*.mp3"));

	// Configure textview
	SDL_Surface *swin=LoadImage("pmeswin");
	SDL_Surface *sicon=LoadImage("pmeswins");
	if(swin && sicon){
		// Colorkey and make semitransparent black
		SDL_Surface *owin=EDL_ColorkeySurface(swin,0);
		for(int x=10;x<owin->w-10;x++){
			for(int y=10;y<owin->h-10;y++){
				Uint32 *p=(Uint32*)owin->pixels+y*owin->pitch/4+x;
#if SDL_BYTEORDER==SDL_BIG_ENDIAN
				if(!((*p)&0xFF)){
					*p=128;
				}
#else
				if(!((*p)&0xFF000000)){
					*p=(128<<24);
				}
#endif
			}
		}
		textview->SetTextPosition(124,20,350,85);
		textview->MoveDialog(66,347);
		textview->Resize(owin->w,owin->h);
		textview->Set(owin);
		SDL_FreeSurface(owin);
		SDL_FreeSurface(swin);
	}
	else{
		LogError("Failed to load textview graphics");
	}

	// Configure selection graphics
	selitem_hover=0;
	selitem_normal=0;
	selection->Move(66+124,347+20);
	selection->Resize(350,85);

	// Configure title screen
	SDL_Surface *smain=LoadImage("ptopmenu");
	if(smain){
		// Set background
		int w=smain->w;
		int h=NativeHeight();
		SDL_Rect r={0,smain->h-h,w,h};
		title->Blit(smain,&r);

		// Create transparent item surface
		SDL_Rect rimage={0,0,15,15};
		SDL_Rect rmask={15,0,15,15};
		SDL_Rect rdest={7,7,15,15};
		SDL_Surface *smask=EDL_CopySurface(smain,&rmask);
		SDL_Surface *sitem=EDL_MaskSurface(smain,smask,&rimage);
		SDL_Surface *smenu=EDL_CreateSurface(250,30);
		SDL_Surface *sempty=EDL_CreateSurface(250,30);
		EDL_SetAlpha(smenu,0,0x00);
		EDL_SetAlpha(sempty,0,0x00);
		EDL_BlitSurface(sitem,0,smenu,&rdest);
		SDL_FreeSurface(smask);
		SDL_FreeSurface(sitem);

		// Build menu
		BitmapButton *newbutton=new BitmapButton(110,360,150,40);
		BitmapButton *loadbutton=new BitmapButton(120,405,150,40);
		BitmapButton *optionbutton=new BitmapButton(308,300,150,40);
		BitmapButton *extracgbutton=new BitmapButton(305,342,250,40);
		BitmapButton *extrabgmbutton=new BitmapButton(305,384,250,40);
		BitmapButton *exitbutton=new BitmapButton(370,425,150,40);
		newbutton->SetState(WS_NORMAL,sempty,0);
		newbutton->SetState(WS_HOVER,smenu,0);
		loadbutton->SetState(WS_NORMAL,sempty,0);
		loadbutton->SetState(WS_HOVER,smenu,0);
		optionbutton->SetState(WS_NORMAL,sempty,0);
		optionbutton->SetState(WS_HOVER,smenu,0);
		extracgbutton->SetState(WS_NORMAL,sempty,0);
		extracgbutton->SetState(WS_HOVER,smenu,0);
		extrabgmbutton->SetState(WS_NORMAL,sempty,0);
		extrabgmbutton->SetState(WS_HOVER,smenu,0);
		exitbutton->SetState(WS_NORMAL,sempty,0);
		exitbutton->SetState(WS_HOVER,smenu,0);
		title->SetNewButton(newbutton);
		title->SetLoadButton(loadbutton);
		title->SetOptionButton(optionbutton);
		title->SetExtraCGButton(extracgbutton);
		title->SetExtraBGMButton(extrabgmbutton);
		title->SetExitButton(exitbutton);
		SDL_FreeSurface(smenu);
		SDL_FreeSurface(sempty);
		SDL_FreeSurface(smain);
	}
	else{
		LogError("Failed to load base graphics");
	}
}

Tokimeki::~Tokimeki(){
	if(selitem_hover && selitem_normal){
		SDL_FreeSurface(selitem_hover);
		SDL_FreeSurface(selitem_normal);
	}
}

const uString Tokimeki::NativeID(){
	return "TOKI";
}

const uString Tokimeki::NativeName(){
	return "Tokimeki Check-in!";
}

void Tokimeki::EventNew(){
	LoadCrowdScript("101-20");
	title->SetVisible(false);
}

bool Tokimeki::ShowSelection(Stringlist List){
	bool retval=List.GetCount();
	int space=10;
	int w=selitem_normal->w;
	int h=selitem_normal->h;
	int x=(NativeWidth()-w)/2;
	int y=(NativeHeight()-((h+space)*List.GetCount()))/2;
	SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
	Paint(screen,VL_CHARACTERS);
	for(int i=0;i<List.GetCount();i++){
		SDL_Rect r={x,y,w,h};
		SDL_Surface *s=EDL_CreateSurface(w,h);
		SDL_Surface *u=EDL_CreateSurface(w,h);
		EDL_BlitSurface(screen,&r,s,0);
		EDL_BlitSurface(screen,&r,u,0);
		EDL_BlendSurface(selitem_hover,0,s,0);
		EDL_BlendSurface(selitem_normal,0,u,0);
		EDL_BlendText(List.GetString(i),0xFFFF00FF,s,0);
		EDL_BlendText(List.GetString(i),0xFFFFFFFF,u,0);
		selection->SetSurface(s,u,r,i+1);
		SDL_FreeSurface(u);
		SDL_FreeSurface(s);
		y+=(h+space);
	}
	selection->SetVisible(retval);
	SDL_FreeSurface(screen);
	return retval;
}


