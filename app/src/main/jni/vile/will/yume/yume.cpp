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
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
 */

#include "yume.h"

YumeMiruKusuri::YumeMiruKusuri(uString Path) : EngineWill(800,600) {
	// Add resources
	AddScripts(new ArchiveWillARC(Path+"Rus.arc"));
	AddBGM(new ArchiveWillARC(Path+"Bgm.arc"));
	AddVoices(new ArchiveWillARC(Path+"Voice.arc"));
	AddSE(new ArchiveWillARC(Path+"Se.arc"));
	AddScripts(new ArchiveWillARC(Path+"Rio.arc"));

	AddImages(new ArchiveWillARC(Path+"Rus.arc"));
	AddImages(new ArchiveWillARC(Path+"Chip.arc"));
	AddOther(new ArchiveWillARC(Path+"Chip.arc"));
	AddVideo(new ArchiveFiles(Path+"A_ED.dat"));
	AddVideo(new ArchiveFiles(Path+"M_ED.dat"));
	AddVideo(new ArchiveFiles(Path+"N_ED.dat"));
	//AddVideo(new ArchiveFiles(Path+"op.dat"));

	// Configure textview
	DestroyWidget(textview);
	textview=new YumeTextview(this);
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

const uString YumeMiruKusuri::NativeID(){
	return "Yume";
}

const uString YumeMiruKusuri::NativeName(){
	return "Yume Miru Kusuri";
}


