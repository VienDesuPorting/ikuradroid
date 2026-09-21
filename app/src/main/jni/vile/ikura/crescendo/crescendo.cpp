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

#include "crescendo.h"

Crescendo::Crescendo(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddBGM(new ArchiveIkura(Path+"MIDI"));
	AddVideo(new ArchiveFiles(Path+"dologo.mpg"));
	AddVideo(new ArchiveFiles(Path+"DOLOGO.MPG"));
	AddOther(new ArchiveFiles(Path+"*.suf"));

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Crescendo::NativeID(){
	return "Crescendo";
}

const uString Crescendo::NativeName(){
	return "Crescendo";
}

/*! \brief Ignore textview configuration for this game
 *  \param Data Ignored
 *  \param Length Ignored
 *  \return False
 */
bool Crescendo::iop_wp(const Uint8 *Data,int Length){
	return false;
}

