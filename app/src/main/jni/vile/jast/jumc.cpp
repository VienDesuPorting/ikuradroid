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

#include "jumc.h"

JUMC::JUMC(uString Path) : EngineJAST(640,480) {
	// Add resources
	AddImages(new ArchiveJAST(Path+"data/images.pck"));
	AddImages(new ArchiveJAST(Path+"data/frames.pck"));
	AddImages(new ArchiveJAST(Path+"data/common.pck"));
	AddScripts(new ArchiveJAST(Path+"data/data.pck"));
	AddBGM(new ArchiveFiles(Path+"music/*.mP3"));

	// Set unspecified title
	title=JT_COMMON;

	// Load main menu
	main=new JUMCMain(this);
	AddWidget(main,VL_DIALOG);
	EventGameDialog(VD_TITLE);
}

JUMC::~JUMC(){
}

bool JUMC::StartGame(JUMC_TITLE Title){
	bool retval=false;
	if(Title==JT_SOS){
		SDL_Surface *frame=LoadImage("soscls");
		title=Title;
		SetFrame(frame);
		LoadJASTScript("cs101");
		SDL_FreeSurface(frame);
		retval=true;
	}
	if(Title==JT_3SIS){
		SDL_Surface *frame=LoadImage("siscls");
		title=Title;
		SetFrame(frame);
		LoadJASTScript("sk_101");
		SDL_FreeSurface(frame);
		retval=true;
	}
	if(Title==JT_RUN){
		SDL_Surface *frame=LoadImage("runcls");
		title=Title;
		SetFrame(frame);
		LoadJASTScript("mt_0101");
		SDL_FreeSurface(frame);
		retval=true;
	}
	return retval;
}

void JUMC::EventGameDialog(VN_DIALOGS Dialog){
	if(VD_TITLE){
		StopMusic();
		main->SetVisible(true);
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

const uString JUMC::NativeID(){
	switch(title){
		case JT_SOS:		return "SOS";
		case JT_3SIS:		return "3SIS";
		case JT_RUN:		return "RUN";
		case JT_COMMON:		return "JUMC";
		default:			return "JUMC";
	};
}

const uString JUMC::NativeName(){
	switch(title){
		case JT_SOS:		return "Season of the Sakura";
		case JT_3SIS:		return "3 Sisters Story";
		case JT_RUN:		return "Runaway City";
		case JT_COMMON:		return "JAST USA Memorial Collection";
		default:			return "JAST USA Memorial Collection";
	};
}


