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

#include "hitomi.h"

Hitomi::Hitomi(uString Path) : IkuraDecoder(800,600){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddVideo(new ArchiveFiles(Path+"LOGO.MPG"));
	AddOther(new ArchiveIkura(Path+"DATA"));
	CreateTextview();

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Hitomi::NativeID(){
	return "HITOMI";
}

const uString Hitomi::NativeName(){
	return "Hitomi -My Stepsister-";
}

void Hitomi::CreateTextview(){
	// Load textwindow graphic and prepare widget
	w_textview->SetTextPosition(15,40,770,140);
	SDL_Surface *tmpsurface=LoadImage("M_WIN.GG3");
	SDL_Surface *autobutton=LoadImage("M_AUTO.GG3");
	SDL_Surface *skipbutton=LoadImage("M_SKIP.GG3");
	SDL_Surface *menubutton=LoadImage("M_MENU.GG3");
	
	if(tmpsurface && autobutton && skipbutton && menubutton){
		// Set window graphics
		w_textview->Resize(tmpsurface->w,tmpsurface->h);
		w_textview->Set(tmpsurface);
		
		// Size of individual buttons
		int bw=72;
		int menuw=88;
		int bh=40;
		
		// Skip checkbox
		ValueButton *w_skip=new ValueButton();
		SDL_Rect skipclear={0,bh*4,bw,bh};
		SDL_Rect skiphover={0,bh*2,bw,bh};
		SDL_Rect skipset={0,bh*3,bw,bh};
		SDL_Rect skipnormal={0,bh,bw,bh};
		SDL_Rect skip={0,0,bw,bh};
		w_skip->Move(552+bw,0);
		w_skip->Resize(bw,bh);
		w_skip->SetState(WS_NORMAL,skipbutton,&skip,skipbutton,&skipnormal);
		w_skip->SetState(WS_HOVER,skipbutton,&skip,skipbutton,&skiphover);
		w_skip->SetState(WS_SELECT,skipbutton,&skip,skipbutton,&skipset);
		w_skip->SetState(WS_DISABLE,skipbutton,&skip,skipbutton,&skipclear);
		w_textview->SetSkipButton(w_skip);

		// Autobutton
		BitmapButton *w_auto=new BitmapButton();
		w_auto->Move(552,0);
		w_auto->Resize(bw,bh);
		w_auto->SetState(WS_NORMAL,autobutton,0,bh*1,bw,bh);
		w_auto->SetState(WS_HOVER,autobutton,0,bh*2,bw,bh);
		w_auto->SetState(WS_SELECT,autobutton,0,bh*3,bw,bh);
		w_auto->SetState(WS_DISABLE,autobutton,0,bh*4,bw,bh);
		w_textview->SetAutoButton(w_auto);

		// Menubutton
		BitmapButton *w_option=new BitmapButton();
		w_option->Move(552+bw*2,0);
		w_option->Resize(menuw,bh);
		w_option->SetState(WS_NORMAL,menubutton,0,bh*1,menuw,bh);
		w_option->SetState(WS_HOVER,menubutton,0,bh*2,menuw,bh);
		w_option->SetState(WS_SELECT,menubutton,0,bh*3,menuw,bh);
		w_option->SetState(WS_DISABLE,menubutton,0,bh*4,menuw,bh);
		w_textview->SetOptionButton(w_option);
		
		// Clean up temporary surfaces
		SDL_FreeSurface(skipbutton);
		SDL_FreeSurface(autobutton);
		SDL_FreeSurface(menubutton);
		SDL_FreeSurface(tmpsurface);
	}
	else{
		LogError("Failed to load textview graphics");
	}
}

/*TODO: 
	Get the title screen working. Game currently boots and immediately starts a new game.
	Reconfigure the menu button to pop up the sub-menu
	Implement the graph view
	Get the animated "next page" indicator in the bottom-right working
	Derive a method to set save names
	Figure out why you cannot save at decision points
*/
