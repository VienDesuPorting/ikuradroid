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

#include "kanaoka.h"

KanaOkaeri::KanaOkaeri(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddBGM(new ArchiveIkura(Path+"MIDI"));
	AddVideo(new ArchiveIkura(Path+"DOLOGO.MPG"));

	// Configure textview
	w_textview->Move(0,0);
	w_textview->Resize(640,480);
	w_textview->SetTextPosition(20,20,600,460);
	w_textview->SetAlpha(128);

	/*
	char *ttt=new char[36865];
	for(int i=0;i<36865;i++){
		ttt[i]=kanakey[i];
		printf("%c",kanakey[i]);
	}
	*/

	// Load standard boot script
	RunScript("START.ISF");
}

const uString KanaOkaeri::NativeID(){
	return "KanaOkaeri";
}

const uString KanaOkaeri::NativeName(){
	return "Kana ... Okaeri!";
}

