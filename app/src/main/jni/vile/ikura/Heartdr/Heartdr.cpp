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

#include "Heartdr.h"

Heartdr::Heartdr(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
    //AddVoices(new ArchiveViLE("VOICE3"));

	AddSE(new ArchiveIkura(Path+"SE"));
	//AddBGM(new ArchiveIkura(Path+"WMSC2"));
    AddBGM(new ArchiveViLE(Path+"WMSC"));

	AddBGM(new ArchiveIkura(Path+"MIDI"));
	AddVideo(new ArchiveFiles(Path+"dologo.mpg"));
	AddVideo(new ArchiveFiles(Path+"DOLOGO.MPG"));
	AddOther(new ArchiveFiles(Path+"*.suf"));

	// Skinned selection items (PC style: white captions with a drop
	// shadow inside the message frame, hovered rows on a pink strip)
	w_select->SetColors(0xF8A0B0FF,0xFFFFFFFF,0x00000000,0xFFFFFFFF);
	w_select->SetAlignment(HA_LEFT,VA_CENTER);
	w_select->SetFontSize(18);
	w_select->SetShadow(true);
	w_select->SetSwallowMisses(true);

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Heartdr::NativeID(){
	return "Heartdr";
}

const uString Heartdr::NativeName(){
	return "Heartdr";
}

/*! \brief Loads the skinned message window (window.gg2)
 *
 *  The base implementation loads the window graphics referenced by
 *  the WP opcode. After the skin is in place the text area is
 *  fitted inside the frame. Colors and style stay with the script:
 *  START.ISF registers font 0 as white plus bold with a (2,2)
 *  shadow, which is exactly how the PC original prints here.
 */
bool Heartdr::iop_wp(const Uint8 *Data,int Length){
	bool retval=IkuraDecoder::iop_wp(Data,Length);
	// Body lines start 45px below the window top on the PC original;
	// the printer leading accounts for the remaining few pixels.
	w_textview->SetTextPosition(18,41,574,95);
	return retval;
}

/*! \brief Maps choice rects relative to the command window
 *
 *  START.ISF shares the message window geometry with the command
 *  window (CW 12,320,640x152) and CSET specifies item rects inside
 *  it.
 */
SDL_Rect Heartdr::MapChoiceRect(Uint32 X,Uint32 Y,Uint32 W,Uint32 H){
	SDL_Rect rect;
	rect.x=(Sint16)(cmdrect.x+(Sint16)X);
	rect.y=(Sint16)(cmdrect.y+(Sint16)Y);
	rect.w=(Sint16)W;
	rect.h=(Sint16)H;
	return rect;
}

