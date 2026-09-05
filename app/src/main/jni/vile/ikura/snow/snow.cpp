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

#include "snow.h"

Snow::Snow(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddBGM(new ArchiveIkura(Path+"MIDI"));
	AddVideo(new ArchiveFiles(Path+"DOLOGO.MPG"));
	AddVideo(new ArchiveFiles(Path+"OP.MPG"));
	CreateTextview();

	// Load standard boot script
	RunScript("START.ISF");
}

void Snow::CreateTextview(){
	/*
	SetTextDisplacement(10,40);
	SDL_Surface *tmpsurface=LoadImage("m_win.gg3");
	if(tmpsurface){
		// Load textwindow graphic and get size
		ResizeTextview(tmpsurface->w,tmpsurface->h);
		SetTextviewBackground(tmpsurface);
		SDL_FreeSurface(tmpsurface);
	}

	// Load and assign overlay graphics for skip
	SDL_Surface *tmpbuttons=LoadImage("m_skip.gg3");
	if(tmpbuttons){
		int bw=tmpbuttons->w/4;
		int bh=tmpbuttons->h;
		SDL_Rect rdst={376,368,bw,bh};
		SDL_Rect rnormal={bw*1,0,bw,bh};
		SDL_Rect rhover={bw*2,0,bw,bh};
		SDL_Rect rselect={bw*3,0,bw,bh};
		SDL_Rect rdisable={bw*0,0,bw,bh};
		SetTextviewValue(IW_TEXTVIEW_SKIP,tmpbuttons,rdst,
				rnormal,rhover,rselect,rdisable,
				rselect,rselect,rselect,rdisable);
		SDL_FreeSurface(tmpbuttons);
	}

	// Load and assign overlay graphics for automode
	tmpbuttons=LoadImage("m_auto.gg3");
	if(tmpbuttons){
		int bw=tmpbuttons->w/4;
		int bh=tmpbuttons->h;
		SDL_Rect rdst={448,368,bw,bh};
		SDL_Rect rnormal={bw*1,0,bw,bh};
		SDL_Rect rhover={bw*2,0,bw,bh};
		SDL_Rect rselect={bw*3,0,bw,bh};
		SDL_Rect rdisable={bw*0,0,bw,bh};
		SetTextviewButton(IW_TEXTVIEW_AUTO,tmpbuttons,rdst,
				rnormal,rhover,rselect,rdisable);
		SDL_FreeSurface(tmpbuttons);
	}

	// Load and assign overlay graphics for menu
	tmpbuttons=LoadImage("m_menu.gg3");
	if(tmpbuttons){
		int bw=tmpbuttons->w;
		int bh=tmpbuttons->h/4;
		SDL_Rect rdst={520,368,bw,bh};
		SDL_Rect rnormal={0,bh*1,bw,bh};
		SDL_Rect rhover={0,bh*2,bw,bh};
		SDL_Rect rselect={0,bh*3,bw,bh};
		SDL_Rect rdisable={0,bh*0,bw,bh};
		SetTextviewButton(IW_TEXTVIEW_MENU,tmpbuttons,rdst,
				rnormal,rhover,rselect,rdisable);
		SDL_FreeSurface(tmpbuttons);
	}
	*/
}

