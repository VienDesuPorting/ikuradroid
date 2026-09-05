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

#include "xchange3.h"

XChange3::XChange3(uString Path) : EngineCrowd3(800,600) {
	// Load XChange 3 resources
	AddScripts(new ArchiveCrowdSCE(Path+"xc3.sce"));
	AddVoices(new ArchiveCrowdPCK(Path+"voice/xc3.pck"));
	AddImages(new ArchiveFiles(Path+"cg/*"));
	AddBGM(new ArchiveFiles(Path+"bgm/*"));

	// Configure textview
	SDL_Surface *smain=LoadImage("pmain.cwp");
	if(smain){
		// Get metrics
		SDL_Rect rfg={0,0,smain->w/2,160};
		SDL_Rect rbg={rfg.w,0,rfg.w,rfg.h};
		int x=(NativeWidth()-rfg.w)/2;
		int y=(NativeHeight()-rfg.h)-20;

		// Colorkey source graphics
		Uint32 cfg=0xFF00FFFF;
		Uint32 cbg=0x000000FF;
		SDL_Surface *sfg=EDL_ColorkeySurface(smain,cfg,&rfg);
		SDL_Surface *sbg=EDL_ColorkeySurface(smain,cbg,&rbg);

		// Create textview object and release surfaces
		LogVerbose("Configuring textview: %dx%dx%dx%d",x,y,sbg->w,sbg->h);
		textview->SetTextPosition(96,45,500,95);
		textview->MoveDialog(x,y);
		textview->Resize(sbg->w,sbg->h);
		textview->Set(sbg);
		textview->Blend(sfg);
		SDL_FreeSurface(smain);
		SDL_FreeSurface(sbg);
		SDL_FreeSurface(sfg);
	}
	else{
		LogError("Cannot find textview image: pmain.cwp");
	}

	// Configure selection widget for xchange 3
	SDL_Surface *ssel=LoadImage("pmain.cwp");
	selitem_normal=0;
	selitem_hover=0;
	if(ssel){
		// Copy source graphics
		int sw=700;
		int sh=41;
		SDL_Surface *selitem_alpha=EDL_CreateSurface(sw,sh);
		selitem_normal=EDL_CreateSurface(sw,sh);
		selitem_hover=EDL_CreateSurface(sw,sh);
		SDL_Rect rnormal={452,160,sw,sh};
		SDL_Rect rhover={452,201,sw,sh};
		SDL_Rect ralpha={452,242,sw,sh};
		EDL_BlitSurface(ssel,&rnormal,selitem_normal,0);
		EDL_BlitSurface(ssel,&rhover,selitem_hover,0);
		EDL_BlitSurface(ssel,&ralpha,selitem_alpha,0);

		// Copy alphamask and clean up
		for(int x=0;x<ralpha.w;x++){
			for(int y=0;y<ralpha.h;y++){
				Uint32 *ap=(Uint32*)selitem_alpha->pixels+y*
					selitem_alpha->pitch/4+x;
				Uint32 *np=(Uint32*)selitem_normal->pixels+y*
					selitem_normal->pitch/4+x;
				Uint32 *hp=(Uint32*)selitem_hover->pixels+y*
					selitem_hover->pitch/4+x;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				*np=((*np)&0xFFFFFF00)|(((*ap)>>24)&0xFF);
				*hp=((*hp)&0xFFFFFF00)|(((*ap)>>24)&0xFF);
#else
				*np=((*np)&0x00FFFFFF)|(((*ap)&0xFF)<<24);
				*hp=((*hp)&0x00FFFFFF)|(((*ap)&0xFF)<<24);
#endif
			}
		}
		SDL_FreeSurface(selitem_alpha);
		SDL_FreeSurface(ssel);

		// Configure selection widget
		selection->Resize(800,600);
	}

	// Configure title screen
	SDL_Surface *smenu=LoadImage("pmenu.cwp");
	if(smenu){
		// Create button widgets
		int bh=42;
		int bw=150;
		BitmapButton *newbutton=new BitmapButton(641,6,bw,bh);
		BitmapButton *loadbutton=new BitmapButton(641,56,bw,bh);
		BitmapButton *extracgbutton=new BitmapButton(641,105,bw,bh);
		BitmapButton *extrabgmbutton=new BitmapButton(641,154,bw,bh);
		BitmapButton *extrascenebutton=new BitmapButton(641,202,bw,bh);
		BitmapButton *optionbutton=new BitmapButton(641,253,bw,bh);
		BitmapButton *exitbutton=new BitmapButton(706,348,75,bh);

		// Configure button graphics
		newbutton->SetState(WS_NORMAL,smenu,641,6,bw,bh);
		newbutton->SetState(WS_HOVER,smenu,0,601,bw,bh);
		loadbutton->SetState(WS_NORMAL,smenu,641,56,bw,bh);
		loadbutton->SetState(WS_HOVER,smenu,0,644,bw,bh);
		extracgbutton->SetState(WS_NORMAL,smenu,641,105,bw,bh);
		extracgbutton->SetState(WS_HOVER,smenu,0,687,bw,bh);
		extrabgmbutton->SetState(WS_NORMAL,smenu,641,154,bw,bh);
		extrabgmbutton->SetState(WS_HOVER,smenu,0,730,bw,bh);
		extrascenebutton->SetState(WS_NORMAL,smenu,641,202,bw,bh);
		extrascenebutton->SetState(WS_HOVER,smenu,151,601,bw,bh);
		optionbutton->SetState(WS_NORMAL,smenu,641,253,bw,bh);
		optionbutton->SetState(WS_HOVER,smenu,151,644,bw,bh);
		exitbutton->SetState(WS_NORMAL,smenu,706,348,75,bh);
		exitbutton->SetState(WS_HOVER,smenu,226,687,75,bh);

		// Add buttons to the titlescreen and clean up
		title->Blit(smenu);
		title->SetNewButton(newbutton);
		title->SetLoadButton(loadbutton);
		title->SetOptionButton(optionbutton);
		title->SetExtraCGButton(extracgbutton);
		title->SetExtraBGMButton(extrabgmbutton);
		title->SetExtraSceneButton(extrascenebutton);
		title->SetExitButton(exitbutton);
		SDL_FreeSurface(smenu);
	}
}

XChange3::~XChange3(){
	if(selitem_hover && selitem_normal){
		SDL_FreeSurface(selitem_hover);
		SDL_FreeSurface(selitem_normal);
	}
}

const uString XChange3::NativeID(){
	return "XC3";
}

const uString XChange3::NativeName(){
	return "XChange 3";
}

void XChange3::EventNew(){
	LoadCrowdScript("00-000");
	title->SetVisible(false);
}

bool XChange3::ShowSelection(Stringlist List){
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


