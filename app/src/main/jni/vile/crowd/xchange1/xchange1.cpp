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

#include "xchange1.h"

XChange1::XChange1(uString Path) : EngineCrowd1(640,480) {
	// Load XChange 1 resources
	AddScripts(new ArchiveCrowdSCE(Path+"scene00.dat"));
	AddImages(new ArchiveFiles(Path+"*.zbm"));
	AddBGM(new ArchiveFiles(Path+"*.mp3"));

	// Configure textview
	SDL_Surface *swin=LoadImage("p_meswin");
	SDL_Surface *sicon=LoadImage("p_m_icon");
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
	SDL_Surface *smain=LoadImage("000-10a1");
	SDL_Surface *smenu=LoadImage("p_menu");
	if(smain && smenu){
		// Colorkey and invert menu graphics
		SDL_Surface *omenu=EDL_ColorkeySurface(smenu,0);
		SDL_Surface *imenu=EDL_CreateSurface(omenu->w,omenu->h);
		for(int x=0;x<omenu->w;x++){
			for(int y=0;y<omenu->h;y++){
				Uint32 *i=(Uint32*)imenu->pixels+y*imenu->pitch/4+x;
				Uint32 *o=(Uint32*)omenu->pixels+y*omenu->pitch/4+x;
#if SDL_BYTEORDER==SDL_BIG_ENDIAN
				*i=(*o)^0xFFFFFF00;
#else
				*i=(*o)^0x00FFFFFF;
#endif
			}
		}

		// Create solid background and build menu
		int bx=414;
		int bw=140;
		int bh=24;
		SDL_Rect rmenu={400,300,omenu->w,omenu->h};
		EDL_BlendSurface(omenu,0,smain,&rmenu);
		BitmapButton *newbutton=new BitmapButton(bx,312,bw,bh);
		BitmapButton *loadbutton=new BitmapButton(bx,334,bw,bh);
		BitmapButton *extracgbutton=new BitmapButton(bx,357,bw,bh);
		BitmapButton *extrabgmbutton=new BitmapButton(bx,380,bw,bh);
		BitmapButton *exitbutton=new BitmapButton(bx,406,bw,bh);
		newbutton->SetState(WS_NORMAL,omenu,14,14,bw,bh);
		newbutton->SetState(WS_HOVER,imenu,14,14,bw,bh);
		loadbutton->SetState(WS_NORMAL,omenu,14,36,bw,bh);
		loadbutton->SetState(WS_HOVER,imenu,14,36,bw,bh);
		extracgbutton->SetState(WS_NORMAL,omenu,14,59,bw,bh);
		extracgbutton->SetState(WS_HOVER,imenu,14,59,bw,bh);
		extrabgmbutton->SetState(WS_NORMAL,omenu,14,82,bw,bh);
		extrabgmbutton->SetState(WS_HOVER,imenu,14,82,bw,bh);
		exitbutton->SetState(WS_NORMAL,omenu,14,106,bw,bh);
		exitbutton->SetState(WS_HOVER,imenu,14,106,bw,bh);
		title->Blit(smain);
		title->SetNewButton(newbutton);
		title->SetLoadButton(loadbutton);
		title->SetExtraCGButton(extracgbutton);
		title->SetExtraBGMButton(extrabgmbutton);
		title->SetExitButton(exitbutton);
		SDL_FreeSurface(smain);
		SDL_FreeSurface(smenu);
		SDL_FreeSurface(omenu);
		SDL_FreeSurface(imenu);
	}
	else{
		LogError("Failed to load base graphics");
	}
}

XChange1::~XChange1(){
	if(selitem_hover && selitem_normal){
		SDL_FreeSurface(selitem_hover);
		SDL_FreeSurface(selitem_normal);
	}
}

const uString XChange1::NativeID(){
	return "XC1";
}

const uString XChange1::NativeName(){
	return "XChange 1";
}

void XChange1::EventNew(){
	LoadCrowdScript("101-11");
	title->SetVisible(false);
}

bool XChange1::ShowSelection(Stringlist List){
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


