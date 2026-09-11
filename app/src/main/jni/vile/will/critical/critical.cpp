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

#include "critical.h"
#include "criticaltv.h"

CriticalPoint::CriticalPoint(uString Path) : EngineWill(640,480) {
	// CP-era script grammar: shorter argument blocks for OP4A/OP21/OP25/OP8A
	// ("4A 1A 00", "21 01 <bgm>", "25 <u32> <se>", "8A 01 00")
	script_v2=true;

	// CP event graphics (EV###.WIP) are two-layer WIPFs: frame 0 is
	// a black chroma base, frame 1 the actual scene at its own hint
	compose_background=true;

	// Add resources
	AddBGM(new ArchiveWillARC(Path+"Bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"Voice.arc"));
	AddSE(new ArchiveWillARC(Path+"Se.arc"));
	AddScripts(new ArchiveWillARC(Path+"Rio.arc"));
	AddImages(new ArchiveWillARC(Path+"Chip.arc"));
	AddOther(new ArchiveWillARC(Path+"Chip.arc"));
	//AddVideo(new ArchiveWillARC(Path+"ENDING.dat"));
	//AddVideo(new ArchiveFiles(Path+"peach.dat"));

	// Configure textview
	DestroyWidget(textview);
	textview=new CriticalTextview(this);
	AddWidget(textview,VL_TEXTVIEW);

	// Configure textview
	//selection->Move(100,396);
	//selection->Resize(440,70);
	selection->Resize(640/3,480/3);
	selection->Move(640/3,480/3);
	selection->SetAlignment(HA_CENTER,VA_CENTER);

	// Show main menu
	EventGameDialog(VD_TITLE);
}

const uString CriticalPoint::NativeID(){
	return "Critical";
}

const uString CriticalPoint::NativeName(){
	return "Critical Point";
}

/*! PC choice layout, pixel-measured against the PC original: the
 *  captions print inside the dialog chrome, centered on the text
 *  area (x=40..570), 24px apart starting at y=28, plain white
 *  without backing plates. Touch handling follows the LMM scheme:
 *  the selection owns the chrome rectangle, missed presses are
 *  swallowed and every row doubles as a full-width hit area so
 *  there are no dead gaps between items
 */
void CriticalPoint::LayoutTextSelection(Stringlist *items){
	selection->SetFontSize(Cfg::Font::default_size);
	selection->SetAlignment(HA_CENTER,VA_CENTER);
	selection->SetColors(0x58585880,0xFFFFFFFF,0x00000000,0xFFFFFFFF);
	selection->SetBackgroundFill(false);
	selection->Move(0,0);
	selection->Resize(616,152);
	selection->SetSwallowMisses(true);
	uString text;
	for(int i=0;items->GetString(i,&text);i++){
		SDL_Rect area={40,28+i*24,530,24};
		selection->SetText(text,area,i);
	}
}


