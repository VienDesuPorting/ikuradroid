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

#include "idols.h"

Idols::Idols(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddBGM(new ArchiveViLE(Path+"MIDI"));
	AddVideo(new ArchiveFiles(Path+"SEKIRA64.avi"));
	AddOther(new ArchiveIkura(Path+"DATA"));
	CreateTextview();

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Idols::NativeID(){
	return "IDOLS";
}

const uString Idols::NativeName(){
	return "Idols Galore!";
}

void Idols::CreateTextview(){
	// Load textwindow graphic and prepare widget
	w_textview->SetTextPosition(25,45,585,85);
	SDL_Surface *tmpsurface=LoadImage("M_WIN.GGP");

	if(tmpsurface){
		// Set window graphics
		w_textview->Resize(tmpsurface->w,tmpsurface->h);
		w_textview->Set(tmpsurface);

		// Clean up temporary surface
		SDL_FreeSurface(tmpsurface);
	}
	else{
		LogError("Failed to load textview graphics");
	}
}

/*TODO:
	Handle some new text situations in the IkuraDecoder class iop_pm:

	"2B 17 00 03 01 00 FF 92 BE 01 01 5A 00 13 04 01 00 01 00 FF FF FF 00"
	ends up spitting out 0x64 "Z" character and gives an error for an invalid
	sound resource 0x03. I'm not really sure what this is intended to do yet.
	It could have something to do with Kyoka's name displaying in pink? The "Z"
	character always shows up right before she speaks. It seems to also appear
	before Megumi (name in blue) or Aya (name in pink) speak, so that guess
	seems pretty likely. Voices aren't working either, and this is probably
	related.

	Perhaps look at opcode 0x6F usage. It doesn't seem important, but who knows?

	Character names are not being printed. Since they aren't using images,
	they don't "just work" like they have in other games.

	The game plays pretty well. I'll save fixing the text problems mentioned
	above for someone else, as my attempts at tweaking the iop_pm function and
	understanding intended behavior have proven frustrating.
 */

