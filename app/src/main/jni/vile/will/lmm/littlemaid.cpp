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

	// Centered selection box
	selection->Resize(640/3,480/3);
	selection->Move(640/3,480/3);
	selection->SetAlignment(HA_CENTER,VA_CENTER);

	// Boot the script chain (START.SCR -> BRAND.SCR -> MAINMENU.SCR)
	EventGameDialog(VD_TITLE);
}

const uString LittleMyMaid::NativeID(){
	return "LMMaid";
}

const uString LittleMyMaid::NativeName(){
	return "Little My Maid";
}

