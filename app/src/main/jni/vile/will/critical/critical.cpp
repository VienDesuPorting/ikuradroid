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
	// CP-era SAY op: [u16 name-color id][name][text] - no 0x20 separator,
	// no sq/ch words
	say_idword=true;

	// CP event graphics (EV###.WIP) are two-layer WIPFs: frame 0 is
	// a black chroma base, frame 1 the actual scene at its own hint
	compose_background=true;

	// CP scripts place characters via the OP48 x/y arguments: y parks
	// the 424-px sprites flush on the bottom menubar, x centers each
	// sprite per character. Plain full-screen layers, no slots
	sprite_positions=true;
	for(int i=0;i<3;i++){
		overlay[i]->Move(0,0);
	}

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

	// The selection widget is laid out by LayoutTextSelection()
	// every time OP02 presents a choice - no defaults needed here

	// Show main menu
	EventGameDialog(VD_TITLE);
}

const uString CriticalPoint::NativeID(){
	return "Critical";
}

const uString CriticalPoint::NativeName(){
	return "Critical Point";
}

/*! PC choice layout: the captions print inside the dialog window on
 *  the uniform text rows - the same slots the message text uses -
 *  centered in the strip, plain white without backing plates.
 *  Hovered rows are highlighted as the PC original does: the strip
 *  is RGB-inverted, so the dark chrome turns into a light bar and
 *  the white caption turns black. Touch handling follows the LMM
 *  scheme: the selection owns the chrome rectangle, missed presses
 *  are swallowed and every row extends across the full 24px pitch
 *  so there are no dead gaps between items
 */
void CriticalPoint::LayoutTextSelection(Stringlist *items){
	selection->SetFontSize(Cfg::Font::default_size);
	selection->SetAlignment(HA_CENTER,VA_CENTER);
	selection->SetColors(0x58585880,0xFFFFFFFF,0x00000000,0xFFFFFFFF);
	selection->SetHoverInvert(true);
	selection->SetBackgroundFill(false);
	// Own the chrome rectangle - the same dock the CriticalTextview
	// computes for the window (bottom-anchored above the menubar,
	// horizontally centered)
	int boxx=(NativeWidth()-CP_WINDOW_W)/2;
	int boxy=NativeHeight()-CP_MENUBAR_H-CP_WINDOW_H;
	selection->Move(boxx,boxy);
	selection->Resize(CP_WINDOW_W,CP_WINDOW_H);
	selection->SetSwallowMisses(true);
	uString text;
	for(int i=0;items->GetString(i,&text);i++){
		// Row i covers [30+24i,50+24i] padded to the full 24px pitch so
		// consecutive rows touch
		SDL_Rect area={boxx+CP_ROW_XPAD,boxy+CP_ROW_TOP+i*CP_ROW_PITCH,
				CP_ROW_W,CP_ROW_PITCH};
		selection->SetText(text,area,i);
	}
}


