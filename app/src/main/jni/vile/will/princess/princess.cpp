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

#include "princess.h"
#include "princesstv.h"

PrincessWaltz::PrincessWaltz(uString Path) : EngineWill(800,600) {
	// Add resources

    AddScripts(new ArchiveWillARC(Path+"rus.arc"));
    //AddImages (new ArchiveWillARC(Path+"CHIP_RU.arc"));

	AddBGM(new ArchiveWillARC(Path+"Bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"Voice.arc"));
	AddSE(new ArchiveWillARC(Path+"Se.arc"));

	AddScripts(new ArchiveWillARC(Path+"rio.arc"));
	AddImages(new ArchiveWillARC(Path+"Chip.arc"));

    AddImages(new ArchiveWillARC(Path+"Chip_0.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_1.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_2.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_3.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_4.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_5.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_6.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_A.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_B.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_C.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_D.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_E.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_F.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_G.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_H.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_I.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_J.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_K.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_L.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_M.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_N.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_O.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_P.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_Q.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_R.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_S.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_T.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_U.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_V.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_W.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_X.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_Y.arc"));
    AddImages(new ArchiveWillARC(Path+"Chip_Z.arc"));

	AddOther(new ArchiveWillARC(Path+"Chip.arc"));

	AddVideo(new ArchiveFiles(Path+"angela.dat"));
	AddVideo(new ArchiveFiles(Path+"ED1.dat"));
	AddVideo(new ArchiveFiles(Path+"ED2A.dat"));
	AddVideo(new ArchiveFiles(Path+"ED2B.dat"));
    AddVideo(new ArchiveFiles(Path+"iris.dat"));
    AddVideo(new ArchiveFiles(Path+"OP.dat"));
    AddVideo(new ArchiveFiles(Path+"rizel.dat"));
    AddVideo(new ArchiveFiles(Path+"runrun.dat"));
    AddVideo(new ArchiveFiles(Path+"sizuka.dat"));
    AddVideo(new ArchiveFiles(Path+"suzusiro.dat"));

	// Configure textview
	DestroyWidget(textview);
	textview=new PrincessTextview(this);
	AddWidget(textview,VL_TEXTVIEW);
    textview->SetFontSize(20);
	// Configure textview
	//selection->Move(100,396);
	//selection->Resize(440,70);
	selection->Resize(640/3,480/3);
	selection->Move(640/3,480/3);
	selection->SetAlignment(HA_CENTER,VA_CENTER);

	// Show main menu
	EventGameDialog(VD_TITLE);
}

bool PrincessWaltz::OP21(){
	//Uint8 repeats=GETBYTE(script->buffer+script->index+0);
	//Uint16 fadein=GETWORD(script->buffer+script->index+1);
	script->index+=4;
	aString text;
	while(script->buffer[script->index]){
		text+=script->buffer[script->index++];
	}
	script->index++;
	PlayMusic(text);
	return false;

}

const uString PrincessWaltz::NativeID(){
	return "Princess";
}

const uString PrincessWaltz::NativeName(){
	return "Princess Waltz";
}


