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

#include "kana.h"

Kana::Kana(uString Path) : DRSDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"DRSSNR"));
	AddImages(new ArchiveIkura(Path+"DRSGRP"));
	AddSE(new ArchiveIkura(Path+"DRSSEF"));
	AddVideo(new ArchiveFiles(Path+"OP.DOM"));

	// Configure textview
	w_textview->Move(0,0);
	w_textview->Resize(640,480);
	w_textview->SetTextPosition(20,20,600,460);
	w_textview->SetAlpha(128);

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Kana::NativeID(){
	return "Kana";
}

const uString Kana::NativeName(){
	return "Kana ~ Little Sister";
}

