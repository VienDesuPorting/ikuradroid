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

#include "starry.h"

StarrySky::StarrySky(uString Path) : EngineWill(800,600) {
	// Add resources
	AddBGM(new ArchiveWillARC(Path+"Bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"Voice.arc"));
	AddSE(new ArchiveWillARC(Path+"Se.arc"));

	AddScripts(new ArchiveWillARC(Path+"Rio.arc"));

	AddImages(new ArchiveWillARC(Path+"Rus.arc"));
	AddImages(new ArchiveWillARC(Path+"Chip.arc"));
	AddOther(new ArchiveWillARC(Path+"Chip.arc"));

    AddImages(new ArchiveWillARC(Path+"Chip_S.arc"));
    AddOther(new ArchiveWillARC(Path+"Chip_S.arc"));

    AddImages(new ArchiveWillARC(Path+"Effect.arc"));
    AddOther(new ArchiveWillARC(Path+"Effect.arc"));

	//AddVideo(new ArchiveFiles(Path+"A_ED.dat"));
	//AddVideo(new ArchiveFiles(Path+"M_ED.dat"));
	//AddVideo(new ArchiveFiles(Path+"N_ED.dat"));
	//AddVideo(new ArchiveFiles(Path+"op.dat"));

	// Configure textview
	DestroyWidget(textview);
	textview=new StarryTextview(this);
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

const uString StarrySky::NativeID(){
	return "Starry";
}

const uString StarrySky::NativeName(){
	return "Starry sky";
}


