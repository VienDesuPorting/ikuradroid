/*
 * IkuraDroid - Little My Maid engine module
 * Copyright (c) 2026, VienDesu! Porting Team
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

#include "littlemaid.h"
#include "lmmtv.h"

LittleMyMaid::LittleMyMaid(uString Path) : EngineWill(640,480) {
	// LMM scripts use the CP-era grammar: short argument blocks for
	// OP4A/OP21/OP22/OP25/OP26/OP0B/OP23 plus the LMM trap lengths
	script_v2=true;
	// LMM-only trap: OP02 choice items carry a leading u16 parameter
	// (byte-verified; CP items go straight to the caption)
	choice_params=true;

	// Add resources (ARC-8, same container family as Critical Point)
	AddBGM(new ArchiveWillARC(Path+"Bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"Voice.arc"));
	AddSE(new ArchiveWillARC(Path+"Se.arc"));
	AddScripts(new ArchiveWillARC(Path+"Rio.arc"));
	AddImages(new ArchiveWillARC(Path+"Chip.arc"));
	AddOther(new ArchiveWillARC(Path+"Chip.arc"));

	// Configure textview (dialog window from winbase0 + winip0 mask)
	DestroyWidget(textview);
	textview=new LMMTextview(this);
	AddWidget(textview,VL_TEXTVIEW);

	// Boot the script chain (START.SCR -> BRAND.SCR -> MAINMENU.SCR)
	EventGameDialog(VD_TITLE);
}

/*! LMM choice layout (byte-verified scripts + PC reference screenshot)
 *
 *  The original prints the choice captions as plain white lines inside
 *  the dialog window: left-aligned at x=167, first line top y=32 with a
 *  ~20px pitch, and the SELECT caption above them at (111,7). The item
 *  rows stretch across the window body for comfortable touch input.
 */
void LittleMyMaid::LayoutTextSelection(Stringlist *items){
	// Touch usability: the PC original stacks 22px rows with no gaps,
	// which is fine for a mouse but unforgiving for a fingertip. Rows
	// are spaced 36px apart with 28px hit areas (8px dead gap between
	// items); menus of 4+ items fall back to tighter 28px rows to
	// stay inside the dialog window
	int count=0;
	uString text;
	for(;items->GetString(count,&text);count++);
	int step=count>3?28:36;
	int height=step-8;
	selection->SetFontSize(Cfg::Font::default_size);
	selection->SetAlignment(HA_LEFT,VA_CENTER);
	selection->SetColors(0x58585880,0xFFFFFFFF,0x00000000,0xFFFFFFFF);
	selection->SetBackgroundFill(false);
	// The dialog widget must own the window rectangle or touches
	// never reach the items: EngineBase::GetWidgetAt() hit-tests the
	// dialog itself (Widget::TestMouse) before DialogBase looks up
	// the buttons. The window spans the top 145 pixels (winbase0)
	selection->Move(0,0);
	selection->Resize(640,145);
	selection->SetSwallowMisses(true);
	for(int i=0;items->GetString(i,&text);i++){
		SDL_Rect area={167,26+i*step,368,height};
		selection->SetText(text,area,i);
	}
	((LMMTextview*)textview)->PrintSelectTitle();
}

/*! Hides the SELECT caption once an item is picked
 */
void LittleMyMaid::OnSelectClosed(){
	((LMMTextview*)textview)->HideSelectTitle();
}

const uString LittleMyMaid::NativeID(){
	return "LMMaid";
}

const uString LittleMyMaid::NativeName(){
	return "Little My Maid";
}

