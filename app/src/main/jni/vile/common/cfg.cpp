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

#include "cfg.h"
#include "config.h"

// Preffered video settings
int Cfg::Display::Width=640;
int Cfg::Display::Height=480;
int Cfg::Display::Depth=32;
//int Cfg::Display::Flags=SDL_HWSURFACE|SDL_DOUBLEBUF;
int Cfg::Display::Flags;//=SDL_HWSURFACE;

// Audio settings
int Cfg::Audio::Frequency=44100;
int Cfg::Audio::Buffersize=4096;
int Cfg::Audio::Buffercount=6;

int Cfg::Audio::Channels=2;
int Cfg::Audio::CDROM=-1;
bool Cfg::Audio::Enabled=true;
uString Cfg::Audio::Soundfont="GeneralUser GS FluidSynth v1.43.sf2";

// Video configuration
bool Cfg::Video::Overlay=true;
int Cfg::Video::Framecount=6;
bool Cfg::Video::Enabled=true;

// Default system settings
uString Cfg::System::Logfile="";
uString Cfg::System::Keyfile="";
bool Cfg::System::Logcolor=false;
bool Cfg::System::Verbose=true;
bool Cfg::System::Mainmenu=true;
int Cfg::System::Framerate=25;

// Default paths (Defaults from build enviroment)
uString Cfg::Path::cwd=VILE_PATH_CWD;
uString Cfg::Path::game=VILE_PATH_GAME;
uString Cfg::Path::save=VILE_PATH_SAVE;
uString Cfg::Path::config=VILE_PATH_CFG;
uString Cfg::Path::resource=VILE_PATH_RES;

// Default font configuration
uString Cfg::Font::default_face="default.ttf";
int Cfg::Font::default_size=18;
int Cfg::Font::default_style=0;

// Default color scheme
Uint32 Cfg::Color::DialogBackground=0xFFFFFFC0;
Uint32 Cfg::Color::DialogHeader=0xA00000FF;
Uint32 Cfg::Color::DialogTopic=0x000000FF;
Uint32 Cfg::Color::WidgetBackground=0x00000000;
Uint32 Cfg::Color::WidgetFont=0x404040FF;


/* Re-asserts the very same values as the static initializers above.
 * Keep the two blocks in sync: main() calls this before touching any
 * configuration, so a second engine session inside the same process
 * starts from the factory defaults instead of the leftovers of the
 * previous run (autodetected resolution, font paths, ...). */
void Cfg::Reset(){
	// Preferred video settings
	Cfg::Display::Width=640;
	Cfg::Display::Height=480;
	Cfg::Display::Depth=32;
	Cfg::Display::Flags=0;

	// Audio settings
	Cfg::Audio::Frequency=44100;
	Cfg::Audio::Buffersize=4096;
	Cfg::Audio::Buffercount=6;
	Cfg::Audio::Channels=2;
	Cfg::Audio::CDROM=-1;
	Cfg::Audio::Enabled=true;
	Cfg::Audio::Soundfont="GeneralUser GS FluidSynth v1.43.sf2";

	// Video configuration
	Cfg::Video::Overlay=true;
	Cfg::Video::Framecount=6;
	Cfg::Video::Enabled=true;

	// Default system settings
	Cfg::System::Logfile="";
	Cfg::System::Keyfile="";
	Cfg::System::Logcolor=false;
	Cfg::System::Verbose=true;
	Cfg::System::Mainmenu=true;
	Cfg::System::Framerate=25;

	// Default paths (Defaults from build enviroment)
	Cfg::Path::cwd=VILE_PATH_CWD;
	Cfg::Path::game=VILE_PATH_GAME;
	Cfg::Path::save=VILE_PATH_SAVE;
	Cfg::Path::config=VILE_PATH_CFG;
	Cfg::Path::resource=VILE_PATH_RES;

	// Default font configuration
	Cfg::Font::default_face="default.ttf";
	Cfg::Font::default_size=18;
	Cfg::Font::default_style=0;

	// Default color scheme
	Cfg::Color::DialogBackground=0xFFFFFFC0;
	Cfg::Color::DialogHeader=0xA00000FF;
	Cfg::Color::DialogTopic=0x000000FF;
	Cfg::Color::WidgetBackground=0x00000000;
	Cfg::Color::WidgetFont=0x404040FF;
}
