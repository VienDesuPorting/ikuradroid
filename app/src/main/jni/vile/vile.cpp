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

#include "vile.h"
#include "common/edl_texture.h"
#include <SDL.h>

#ifdef __ANDROID__
#include <android/log.h>
// Logcat breadcrumbs (adb logcat -s ikuradroid): the file log
// only starts after the video is up, so a relaunch failing inside
// SDL/video init would otherwise die silently. One line per
// main() milestone.
#define VILE_STAGE(...) __android_log_print(ANDROID_LOG_INFO, \
        "ikuradroid", __VA_ARGS__)
#else
#define VILE_STAGE(...) do{}while(0)
#endif

#ifdef VILE_ARCH_MICROSOFT
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    main(0, 0);
}
#endif

int main(int argc,char **argv){


	// The process survives a game exit on Android (the activity
	// stack stays alive), so main() may run again in the same
	// process: drop everything the previous session wrote into
	// the static configuration before reading it.
	Cfg::Reset();
	// Same hygiene for the directory enumerator: a probe interrupted
	// by an exit must not feed a stale DIR* into this session's
	// first probe (it would make one detection fail spuriously).
	EDL_GetFileReset();
	VILE_STAGE("main: enter, config reset");

#ifdef __ANDROID__
	// Touch stays touch end to end (the FINGER* handlers in the
	// event loop below are the canonical input path). SDL also
	// synthesises a mouse stream from touch input; on 2.0.3 that
	// stream kept getting lost between the Java layer and this
	// loop, but under 2.30 it arrives reliably - and the
	// renderer's event watch hands it over already remapped into
	// logical coordinates, so the loop's own window->logical
	// mapping would process it a second time and fire stray
	// clicks in the top-left corner on every tap. Disable the
	// synthesis; real mice keep working through the MOUSE*
	// handlers on desktop.
	SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
#endif

	// Preload game object and set default values
	Uint32 systemflags=SDL_INIT_VIDEO|SDL_INIT_AUDIO;
	ViLE *game=new ViLE();

	bool dryrun=false;
	bool info_title=false;
	bool info_res=false;
	int retval=ERROR_NONE;

	// Preprocess prioritised parameters
	for(int i=1;i<argc;i++){
		if(!strcmp(argv[i],"--config")){
			Cfg::Path::config=argv[++i];
		}
		if(!strcmp(argv[i],"--cwd")){
			Cfg::Path::cwd=argv[++i];
		}
		if(!strcmp(argv[i],"--logfile")){
			Cfg::System::Logfile=argv[++i];
		}
		if(!strcmp(argv[i],"--logcolor")){
			Cfg::System::Logcolor=true;
		}
	}

	// Check if configuration file is in specified cwd
	if(!EDL_ReadableFile(Cfg::Path::config)){
		uString path=Cfg::Path::cwd+uString("/")+Cfg::Path::config;
		if(EDL_ReadableFile(path)){
			LogVerbose("Found configuration in cwd");
			Cfg::Path::config=path;
		}
	}

	// Load configuration file
	if(EDL_ReadableFile(Cfg::Path::config)){
		int vint;
		uString vstring;
		INIFile ini(false);
		if(ini.ReadFile(Cfg::Path::config)){
			// Display settings
			if(ini.Get("display","width",vint)){
				Cfg::Display::Width=vint;
			}
			if(ini.Get("display","height",vint)){
				Cfg::Display::Height=vint;
			}
			if(ini.Get("display","depth",vint)){
				Cfg::Display::Depth=vint;
			}
			if(ini.Get("display","flags",vint)){
				Cfg::Display::Flags=vint;
			}
			if(ini.Get("display","fullscreen",vint)){
				if(vint){
//                                      Cfg::Display::Flags|=SDL_FULLSCREEN;
				}
				else{
//                                      Cfg::Display::Flags&=~SDL_FULLSCREEN;
				}
			}


			// System settings
			if(ini.Get("system","logfile",vstring)){
				Cfg::System::Logfile=vstring;
			}
			if(ini.Get("system","verbose",vint)){
				Cfg::System::Verbose=vint;
			}

			// Paths
			if(ini.Get("path","cwd",vstring)){
				Cfg::Path::cwd=vstring;
			}
			if(ini.Get("path","game",vstring)){
				Cfg::Path::game=vstring;
			}
			if(ini.Get("path","save",vstring)){
				Cfg::Path::save=vstring;
			}


			// Font configuration
			if(ini.Get("font","default_face",vstring)){
				Cfg::Font::default_face=vstring;
			}
			if(ini.Get("font","default_size",vint)){
				Cfg::Font::default_size=vint;
			}
			if(ini.Get("font","default_style",vint)){
				Cfg::Font::default_style=vint;
			}

			// Audio/Video settings
			if(ini.Get("audio","enabled",vint)){
				Cfg::Audio::Enabled=vint;
			}
			if(ini.Get("audio","samplerate",vint)){
				Cfg::Audio::Frequency=vint;
			}
			if(ini.Get("audio","buffersize",vint)){
				Cfg::Audio::Buffersize=vint;
			}
			if(ini.Get("audio","buffercount",vint)){
				Cfg::Audio::Buffercount=vint;
			}
			if(ini.Get("audio","soundfont",vstring)){
				Cfg::Audio::Soundfont=vstring;
			}
			if(ini.Get("video","enabled",vint)){
				Cfg::Video::Enabled=vint;
			}
			if(ini.Get("video","overlay",vint)){
				Cfg::Video::Overlay=vint;
			}
		}
	}

	// Flush logfile
	if(Cfg::System::Logfile.length()){
		if(EDL_ReadableFile(Cfg::System::Logfile)){
			EDL_DeleteFile(Cfg::System::Logfile);
		}
		LogMessage("Logging to %s",Cfg::System::Logfile.c_str());
		if(!EDL_ReadableFile(Cfg::System::Logfile)){
			uString oldfile=Cfg::System::Logfile;
			Cfg::System::Logfile="";
			LogError("Failed to write to %s",oldfile.c_str());
		}
		else{
			EDL_DeleteFile(Cfg::System::Logfile);
		}
	}

	// Parse parameters
	for(int i=1;i<argc;i++){
		// Switches and configuration parameters
		if(!strcmp(argv[i],"--fullscreen")){
			// Preset videoflag
//                      Cfg::Display::Flags|=SDL_FULLSCREEN;
		}
		else if(!strcmp(argv[i],"--size") && i+2<argc){
			// Set resolution
			Cfg::Display::Width=atoi(argv[++i]);
			Cfg::Display::Height=atoi(argv[++i]);
			if(Cfg::Display::Width<1 || Cfg::Display::Height<1){
				LogError("Invalid resolution: %sx%s",argv[i-2],argv[i-1]);
				return ERROR_PARAMETER;
			}
		}
		else if(!strcmp(argv[i],"--depth") && i+1<argc){
			// Set display depth
			Cfg::Display::Depth=atoi(argv[++i]);
		}
		else if(!strcmp(argv[i],"--cwd") && i+1<argc){
			// Set working directory
			Cfg::Path::cwd=argv[++i];
		}
		else if(i+1<argc && (!strcmp(argv[i],"--game") ||
					!strcmp(argv[i],"--loadpath"))){
			// Preset loadpath
			Cfg::Path::game=argv[++i];
		}
		else if(!strcmp(argv[i],"--save") && i+1<argc){
			// Set working directory
			Cfg::Path::save=argv[++i];
		}
		else if(!strcmp(argv[i],"--fontface") && i+1<argc){
			// Set working directory
			Cfg::Font::default_face=argv[++i];
		}
		else if(!strcmp(argv[i],"--fontsize") && i+1<argc){
			// Set working directory
			unsigned int size=atoi(argv[++i]);
			if(size>0 && size<100){
				Cfg::Font::default_size=size;
			}
			else{
				LogError("Invalid font size: %s",argv[i]);
			}
		}
		else if(!strcmp(argv[i],"--keyfile") && i+1<argc){
			// Load file for decryption
			Cfg::System::Keyfile=argv[++i];
			LogVerbose("Encryption key: %s",Cfg::System::Keyfile.c_str());
		}
		else if(!strcmp(argv[i],"--disable-overlay")){
			// Use hardware overlay for video
			Cfg::Video::Overlay=false;
		}
		else if(!strcmp(argv[i],"--disable-video")){
			// Prevent video resources from loading
			Cfg::Video::Enabled=false;
		}
		else if(!strcmp(argv[i],"--disable-audio")){
			// Prevent audio resources from loading
			Cfg::Audio::Enabled=false;
		}
		else if(!strcmp(argv[i],"--disable-midi")){
			// Prevent midi resources from loading
			Cfg::Audio::Soundfont="";
		}
		else if(!strcmp(argv[i],"--disable-menu")){
			// Do not show the main menu
			Cfg::System::Mainmenu=false;
		}
		else if(!strcmp(argv[i],"--cdrom")){
			// Set CDDA source
			uString arg=argv[++i];
			if(arg=="none"){
				Cfg::Audio::CDROM=-1;
			}
			else if(arg=="auto"){
				Cfg::Audio::CDROM=0;
			}
			else{
				Cfg::Audio::CDROM=atoi(arg.c_str());
			}
		}
		else if(!strcmp(argv[i],"--samplerate")){
			int arg=atoi(argv[++i]);
			if(arg>0){
				Cfg::Audio::Frequency=arg;
			}
			else{
				LogError("Invalid audio samplerate: %s",argv[i]);
			}
		}
		else if(!strcmp(argv[i],"--buffersize")){
			int arg=atoi(argv[++i]);
			if(arg>0){
				Cfg::Audio::Buffersize=arg;
			}
			else{
				LogError("Invalid audio frame size: %s",argv[i]);
			}
		}
		else if(!strcmp(argv[i],"--buffercount")){
			int arg=atoi(argv[++i]);
			if(arg>0){
				Cfg::Audio::Buffercount=arg;
			}
			else{
				LogError("Invalid audio frame count: %s",argv[i]);
			}
		}
		else if(!strcmp(argv[i],"--soundfont")){
			FILE *tf=fopen(argv[++i],"rb");
			if(tf){
				Cfg::Audio::Soundfont=argv[i];
				fclose(tf);
			}
			else{
				LogError("Could not open soundfont: %s",argv[i]);
			}
		}


		// Reconfigure system settings
		else if(!strcmp(argv[i],"--verbose")){
			Cfg::System::Verbose=true;
		}

		else if(!strcmp(argv[i],"--info")){
			// Dump information from game object
			int start=i;
			while(i+1<argc && argv[i+1][0]!='-'){
				if(!strcmp(argv[i+1],"title")){
					info_title=true;
					i++;
				}
				else if(!strcmp(argv[i+1],"res")){
					info_res=true;
					i++;
				}
				else if(!strcmp(argv[i+1],"runtime")){
					game->InfoRuntime();
					i++;
				}
			}

			// Print all information when none is specified
			if(start==i){
				info_title=true;
				info_res=true;
				game->InfoRuntime();
			}
		}

		// Run internal command rather than the game itself
		else if(!strcmp(argv[i],"--about")){
			// Dump information from game object
			game->About();
			return ERROR_NONE;
		}
		else if(!strcmp(argv[i],"--list") && i+1<argc){
			uString archive=argv[++i];
			if(game->List(archive)){
				return ERROR_NONE;
			}
			else{
				return ERROR_PARAMETER;
			}

		}
		else if(!strcmp(argv[i],"--extract") && i+1<argc){
			// Extract resources from archive
			uString archive=argv[++i];
			if(i+1==argc || argv[i+1][0]=='-'){
				if(game->Extract(archive)){
					return ERROR_NONE;
				}
			}
			else if(i+1<argc){
				Stringlist filenames;
				while(i+1<argc && argv[i+1][0]!='-'){
					filenames.AddString(argv[++i]);
				}
				if(game->Extract(archive,&filenames)){
					return ERROR_NONE;
				}
			}
			else{
				LogError("No archive specified");
			}
			return ERROR_PARAMETER;
		}
		else if(!strcmp(argv[i],"--decode") && i+1<argc){
			// Decode resource to native format
			if(i+1<argc && argv[i+1][0]!='-'){
				while(i+1<argc && argv[i+1][0]!='-'){
					if(!game->Decode(argv[++i])){
						LogMessage("Failed to decode: %s",argv[i]);
					}
				}
				return ERROR_NONE;
			}
			else{
				LogError("No files to decode");
			}
			return ERROR_PARAMETER;
		}
		else if(!strcmp(argv[i],"--xdec") && i+1<argc){
			// Extract and decode filenames
			uString archive=argv[++i];
			if(i+1==argc || argv[i+1][0]=='-'){
				if(game->XDec(archive)){
					return ERROR_NONE;
				}
			}
			else if(i+1<argc){
				Stringlist filenames;
				while(i+1<argc && argv[i+1][0]!='-'){
					filenames.AddString(argv[++i]);
				}
				if(game->XDec(archive,&filenames)){
					return ERROR_NONE;
				}
			}
			else{
				LogError("No archive specified");
			}
			return ERROR_PARAMETER;
		}
		else if(!strcmp(argv[i],"--archive") && i+1<argc){
			uString archive=argv[++i];
			uString dira=argv[++i];
			Stringlist list;

            DIR *dir = opendir(dira.c_str());
            if(dir)
            {
                struct dirent *ent;
                while((ent = readdir(dir)) != NULL)
                {
                    uString na = dira;
                    na = na  + "/";
                    na = na + ent->d_name;
                    printf("opening %s\n",na.c_str());
                    list.AddString(na);
                }
            }
            else
            {
                fprintf(stderr, "Error opening directory\n");
            }


			if(game->Archive(archive,&list)){
				return ERROR_NONE;
			}
			return ERROR_PARAMETER;
		}
		else if(!strcmp(argv[i],"--help")){
			// Show available options
			game->Help();
			return ERROR_NONE;
		}
#if VILE_FEATURE_UNITTEST
		else if(!strcmp(argv[i],"--unittest")){
			// Parse parameters
			uString par;
			if(i+1<argc && argv[i+1][0]!='-'){
				par=argv[++i];
			}
			if(par=="all"){
				CoreTest();
				RWTest();
				FileTest();
				MiscTest();
				UnitTest::Summary();
			}
			else if(par=="rw"){
				RWTest();
			}
			else if(par=="file"){
				FileTest();
			}
			else if(par=="core"){
				CoreTest();
			}
			else if(par=="misc"){
				MiscTest();
			}
			else{
				LogMessage("Please use: all|core|rw|file|misc");
				return ERROR_PARAMETER;
			}
			return ERROR_NONE;
		}
		else if(!strcmp(argv[i],"--benchmark")){
			// Parse parameters
			uString par;
			Uint32 runs=1000;
			if(i+1<argc && argv[i+1][0]!='-'){
				par=argv[++i];
				if(par.to_int()>10){
					runs=par.to_int();
					par=argv[++i];
				}
			}
			else if(i+1<argc){
				par=argv[i+1];
			}
			if(par=="all"){
				GFXBench GFXBench(runs);
				MiscBench MiscBench(runs);
				Benchmark::Summary();
			}
			else if(par=="gfx"){
				GFXBench GFXBench(runs);
			}
			else if(par=="misc"){
				MiscBench MiscBench(runs);
			}
			else{
				LogMessage("Please use: [RUNS] all|gfx|misc");
				return ERROR_PARAMETER;
			}
			return ERROR_NONE;
		}
#endif
		// Dryrun option
		else if(!strcmp(argv[i],"--dryrun")){
			dryrun=true;
		}

		// Ignored parameters and error for unknown ones
		else if(!strcmp(argv[i],"--config") && i+1<argc){
			i++;
		}
		else if(!strcmp(argv[i],"--logfile") && i+1<argc){
			i++;
		}
		else if(!strcmp(argv[i],"--logcolor")){
		}
		else{
			LogMessage("Unknown parameter: %s",argv[i]);
			return ERROR_PARAMETER;
		}
	}

	// Initialize system and load game
	if(!game->InitSystem(systemflags)){

		LogError("Failed to initialize external libraries");
		VILE_STAGE("main: InitSystem failed");
		retval=ERROR_INITSYSTEM;
	}
	else if(!dryrun && !game->InitVideo()){
		LogError("Failed to initialize video driver");
		VILE_STAGE("main: InitVideo failed");
		retval=ERROR_INITVIDEO;
	}
	else{
		VILE_STAGE("main: SDL and video are up");
		// Set default caption
		uString caption=
			uString(PACKAGE_STRING)+
			uString(" (")+
			uString(PACKAGE_LINE)+
			uString(")");
//              SDL_WM_SetCaption(caption.c_str(),NULL);

#ifdef ANDROID
		//Cfg::Path::cwd = "/sdcard/ikuradroid/Crescendo";
		//Cfg::Path::game = "/sdcard/ikuradroid/Crescendo";
		char buffer[100];
		getcwd(buffer, 100);
		Cfg::Path::cwd  = buffer;
		Cfg::Path::game = buffer;
		uString textEDLfont = Cfg::Path::cwd.c_str();
		textEDLfont = textEDLfont +"/";
		textEDLfont = textEDLfont +Cfg::Font::default_face.c_str();
		Cfg::Font::default_face = textEDLfont;

		// Mirror engine logs to a file in the game directory.
		// Truncated on every launch so a crashing run keeps its full log
		Cfg::System::Logfile = Cfg::Path::cwd + "/ikuradroid_log.txt";
		FILE *logf=fopen(Cfg::System::Logfile.c_str(),"wb");
		if(logf){
			fclose(logf);
		}
		else{
			Cfg::System::Logfile="";
		}
#endif

	    EDLfont=TTF_OpenFont(Cfg::Font::default_face.c_str(),Cfg::Font::default_size);
		LogVerbose("EDLfont: %s",Cfg::Font::default_face.c_str());
		// Set working directory
		LogVerbose("Setting enviroment:");
		LogVerbose("\tWorking directory: %s",Cfg::Path::cwd.c_str());
        LogMessage("\tWorking directory: %s",Cfg::Path::cwd.c_str());
		if(Cfg::Path::cwd.length()){
			if(!EDL_SetDirectory(Cfg::Path::cwd.c_str())){
				if(!EDL_CreateDirectory(Cfg::Path::cwd.c_str())){
					LogError("Cant create: %s",Cfg::Path::cwd.c_str());
				}
				else{
					if(!EDL_SetDirectory(Cfg::Path::cwd.c_str())){
						LogError("Cant read: %s",Cfg::Path::cwd.c_str());
					}
				}
			}
		}

		// Autodetect and load game using path
		LogVerbose("\tGame directory: %s",Cfg::Path::game.c_str());

		EngineVN *engine=game->LoadEngine(Cfg::Path::game);
		VILE_STAGE("main: engine probe %s", engine ? "ok" : "failed");

		if(!engine){
			LogError("Failed to load game resources");
			retval=ERROR_INITGAME;
		}
		// Dump information from loaded engine
		if(engine && info_title){
			game->InfoTitle(engine);
		}
		if(engine && info_res){
			game->InfoResources(engine);
		}

		if(dryrun){
			// Dryrun only ... nothing to do at this stage
		}
		else if(engine==0){
			// Report failed loading
			uString title="No valid game resources";
			uString text;
                        text+="IkuraDroid needs the resources from the original games ";
                        text+="in order to play them. You can either start IkuraDroid ";
			text+="from the root of the game folder, or specify the ";
			text+="game folder from the commandline ";
			text+="\r\n";
			text+="\r\n";
			if(Cfg::Path::game.length()){
				text+="Current game folder:\r\n    ";
				text+=Cfg::Path::game;
			}
			else if(Cfg::Path::cwd.length()){
				text+="Current working directory:\r\n    ";
				text+=Cfg::Path::cwd;
			}

			game->Error(title,text);
		}
		else{
			if(EDL_ReadableFile(Cfg::Path::resource)){
				// An external vilevn.pck still overrides the embedded copy
				LogVerbose("Widget graphics: %s",Cfg::Path::resource.c_str());
			}
			else{
				// Widget graphics ship inside the binary; games no longer
				// need to carry a vilevn.pck in their folder
				LogVerbose("No vilevn.pck - using the embedded widget "
						"graphics pack");
			}
			// Autodetect size if with or height is invalid
			if(!Cfg::Display::Width || !Cfg::Display::Height){
				Cfg::Display::Width=engine->NativeWidth();
				Cfg::Display::Height=engine->NativeHeight();
				LogVerbose("Autodetecting resolution: %dx%d",
						Cfg::Display::Width,Cfg::Display::Height);
				//game->InitVideo();
			}

			// Execute the game
			LogVerbose("Executing game: %s",engine->NativeName().c_str());
			VILE_STAGE("main: RunEngine enter");
			game->RunEngine(engine);
			VILE_STAGE("main: RunEngine done");
		}
	}

	// Deinitialize sdl libraries
	if(game){
		game->Quit();
		delete game;
	}
	VILE_STAGE("main: exit, retval=%d", retval);
	return retval;
}

bool ViLE::ProbeMayclub(uString Path){
	LogVerbose("Probing for Mayclub:");
	return (ProbeResource(Path,"may0.dat") &&
			ProbeResource(Path,"may0.lst") &&
			ProbeResource(Path,"cg/graphic.pak") &&
			ProbeResource(Path,"wave/n02_88.wav"));
}

bool ViLE::ProbeNocturnal(uString Path){
	LogVerbose("Probing for Nocturnal Illusion:");
	return (ProbeResource(Path,"mug0.dat") &&
			ProbeResource(Path,"mug0.lst") &&
			ProbeResource(Path,"grp") &&
			ProbeResource(Path,"bgw"));
}

bool ViLE::ProbeCrescendo(uString Path){
	// Check for key files
	LogVerbose("Probing for Crescendo:");
	return ProbeSUF(Path,"CRES");
}

bool ViLE::ProbeHDR(uString Path){
	// Check for key files
	LogVerbose("Probing for Heart De Roommate:");
	return ProbeSUF(Path,"HEART");
}

bool ViLE::ProbeVirgin(uString Path){
	// Check for key files
	LogVerbose("Probing for Virgin Roster:");
	return ProbeSUF(Path,"BLEED");
}

bool ViLE::ProbeSagara(uString Path){
	// Check for key files
	LogVerbose("Probing for Sagara Family:");
	return ProbeSUF(Path,"sagara");
}

bool ViLE::ProbeSnow(uString Path){
	// Check for key files
	LogVerbose("Probing for Snow Sakura:");
	return ProbeSUF(Path,"yuki");
}

bool ViLE::ProbeKanaOkaeri(uString Path){
	// Check for key files
	LogVerbose("Probing for Kana Okaeri:");
	return ProbeSUF(Path,"kanaoka");
}

bool ViLE::ProbeKana(uString Path){
	// Check for key files
	LogVerbose("Probing for Kana:");
	return ProbeSUF(Path,"kana");
}

bool ViLE::ProbeHitomi(uString Path){
	// Check for key files
	LogVerbose("Probing for Hitomi -My Stepsister-:");
	return ProbeSUF(Path,"hitomi");
}

bool ViLE::ProbeCatGirl(uString Path){
	// Check for key files
	LogVerbose("Probing for Cat Girl Alliance:");
	return ProbeSUF(Path,"koneko");
}

bool ViLE::ProbeIdols(uString Path){
	// Check for key files
	LogVerbose("Probing for Idols Galore!:");
	bool retval=false;
	if(ProbeSUF(Path,"mesia")){
		retval=true;
	}
	else if(ProbeResource(Path,"isf") && ProbeResource(Path,"mesiaus.suf")){
		// The download edition has an annoying typo in the descriptor file
		INIFile *inifile=new INIFile(false);
		if(inifile->ReadFile(EDL_Realname(Path+"mesiaus.suf"))){
			uString gamekey,gametitle="unknown";
			if(inifile->Get("Key",gamekey)){
				if(ProbeString(gamekey,"mesiaius")){
					retval=true;
				}
			}
		}
	}
	return retval;
}

bool ViLE::ProbeJUMC(uString Path){
	// Check for key files
	LogVerbose("Probing for JAST USA Memorial Collection:");
	return (ProbeResource(Path,"data/images.pck") &&
			ProbeResource(Path,"data/data.pck") &&
			ProbeResource(Path,"data/frames.pck"));
}

bool ViLE::ProbeDiviDead(uString Path){
	// Check for key files
	LogVerbose("Probing for DiviDead:");
	return (ProbeResource(Path,"sg.dl1") &&
			ProbeResource(Path,"wv.dl1"));
}

bool ViLE::ProbeTrueLove(uString Path){
	// Check for key files
	LogVerbose("Probing for True love:");

	return (ProbeResource(Path,"EFF") &&
			ProbeResource(Path,"MRS") &&
			(ProbeResource(Path,"DATE") ||
			 ProbeResource(Path,"DATG")));
}

bool ViLE::ProbeYumeMiruKusuri(uString Path){
	// Check for key files
	LogVerbose("Probing for Yume Miru Kusuri:");
	return (ProbeResource(Path,"Rio.arc") &&
			ProbeResource(Path,"Chip.arc") &&
			(ProbeSize(Path+"Rio.arc",1828524) ||
			 ProbeSize(Path+"Rio.arc",1828457)));
}

bool ViLE::ProbePrincessWaltz(uString Path){
	// Check for key files
	LogVerbose("Probing for Princess Waltz:");
	return (ProbeResource(Path,"rio.arc") &&
			ProbeResource(Path,"Chip.arc") &&
			(ProbeSize(Path+"rio.arc",1828524) ||
			 ProbeSize(Path+"rio.arc",3381310)));
}

bool ViLE::ProbeStarrySky(uString Path){
	// Check for key files
	LogVerbose("Probing for Starry Sky:");
	return (ProbeResource(Path,"Rio.arc") &&
			ProbeResource(Path,"Chip.arc") &&
			(ProbeSize(Path+"Rio.arc",1375521) ||
			 ProbeSize(Path+"Rio.arc",3381310)));
}


bool ViLE::ProbeCriticalPoint(uString Path){
	// Check for key files
	LogVerbose("Probing for Critical Point:");
	return (ProbeResource(Path,"rio.arc") &&
			ProbeResource(Path,"chip.arc") &&
			ProbeSize(Path+"rio.arc",986745)||
            ProbeSize(Path+"Rio.arc",4445247));
}

bool ViLE::ProbeLittleMyMaid(uString Path){
	// Check for key files (2005 Sweet Basil re-release edition)
	LogVerbose("Probing for Little My Maid:");
	return (ProbeResource(Path,"rio.arc") &&
			ProbeResource(Path,"chip.arc") &&
			ProbeSize(Path+"Rio.arc",9575745));
}

bool ViLE::ProbeTokimeki(uString Path){
	// Check for key files
	LogVerbose("Probing for Tokimeki Check-in!:");
	return (ProbeResource(Path,"scene00.bdt") &&
			ProbeResource(Path,"353-103.zbm") &&
			ProbeResource(Path,"indexw.dat"));
}

bool ViLE::ProbeXChange1(uString Path){
	// Check for key files
	LogVerbose("Probing for XChange 1:");
	return (ProbeResource(Path,"scene00.bdt") &&
			ProbeResource(Path,"207-30l2.zbm") &&
			ProbeResource(Path,"index.dat"));
}

bool ViLE::ProbeXChange3(uString Path){
	// Check for key files
	LogVerbose("Probing for XChange 3:");
	return (ProbeResource(Path,"xc3.sce") &&
			ProbeResource(Path,"voice/xc3.pck"));
}

/*! \brief Checks existence of resource file and logs verbosely
 *  \param Path Directory
 *  \param Resource Filename path relative to Path
 *  \return True if file exists
 */
bool ViLE::ProbeResource(uString Path,uString Resource){
	uString path=EDL_Realname(Path+Resource);


	bool retval=EDL_ReadableFile(path);
	LogVerbose("\t%-40s%s",path.c_str(),retval?"PASS":"FAIL");
	return retval;
}

/*! \brief Checks if two strings matches and logs verbosely
 *  \param S1 String
 *  \param S2 String to compare with
 *  \return True if the strings matches (case insensitive)
 */
bool ViLE::ProbeString(uString S1,uString S2){
	S1=EDL_Lower(S1);
	S2=EDL_Lower(S2);
	bool retval=(S1==S2);
	uString msg=S1+uString(" == ")+S2;
	LogVerbose("\t%-40s%s",msg.c_str(),retval?"PASS":"FAIL");
	return retval;
}

/*! \brief Checks size of a file and logs verbosely
 *  \param Path Path to file
 *  \param Size Expected size
 *  \return True if file exists and size matches
 */
bool ViLE::ProbeSize(uString Path,unsigned int Size){
	bool retval=false;
	int compare=-1;
	uString path=EDL_Realname(Path);
	FILE *test=fopen(path.c_str(),"rb");
	if(test){
		fseek(test,0,SEEK_END);
		compare=ftell(test);
		retval=(compare==(int)Size);
		fclose(test);
	}
	LogVerbose("\t%-40s%d/%d",path.c_str(),compare,Size);
	return retval;
}

/*! \brief Confirms a IkuraGDL title using a SUF file
 *  \param Path Folder to load resources from
 *  \param Key Key to confirm
 *  \return True if title resources was confirmed
 */
bool ViLE::ProbeSUF(uString Path,uString Key){
	// Check for key files
	bool retval=0;
	bool isf=ProbeResource(Path,"GGD") && ProbeResource(Path,"ISF");
	bool drs=ProbeResource(Path,"drsgrp") && ProbeResource(Path,"drssnr");
	if(isf || drs){
		// Get SUF file
		uString sufname;
                if(ProbeResource(Path,"game.suf"))                      sufname="game.suf";
                else if(ProbeResource(Path,Key+".suf"))         sufname=Key+".suf";
                else if(ProbeResource(Path,Key+"d.suf"))                sufname=Key+"d.suf";
                else if(ProbeResource(Path,Key+"us.suf"))       sufname=Key+"us.suf";
                else if(ProbeResource(Path,Key+"ml.suf"))       sufname=Key+"ml.suf";

		// Confirm title
		if(sufname.length()){
			INIFile *inifile=new INIFile(false);
			LogVerbose("Parsing SUF: %s",sufname.c_str());
			if(inifile->ReadFile(EDL_Realname(Path+sufname))){
				uString gamekey,gametitle="unknown";
				if(inifile->Get("Key",gamekey)){
					if(ProbeString(gamekey,Key) ||
                                                ProbeString(gamekey,Key+"d") ||
						ProbeString(gamekey,Key+"us") ||
						ProbeString(gamekey,Key+"ml")){
						retval=true;
					}
				}
			}
			delete inifile;
		}
	}
	return retval;
}

void ViLE::RunEngine(EngineVN *engine){
    engine->updateRenderWindow(window);


	uString caption=engine->NativeName();

	if(caption!=PACKAGE_STRING){
		uString pstring=uString(PACKAGE_STRING);
		uString pcode=uString(PACKAGE_LINE);
		caption=pstring+uString(" (")+pcode+uString("): ")+caption;
	}
	else{
		uString pstring=uString(PACKAGE_STRING);
		uString pcode=uString(PACKAGE_LINE);
		caption=pstring+uString(" (")+pcode+uString(")");
	}

	SDL_SetWindowTitle(window,caption.c_str());
    SDL_SetWindowPosition(window,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED);
	// Discard old and fake events
	SDL_Event event;
	while(SDL_PollEvent(&event));

	// Load system settings
	engine->EventLoadSystem();


	float x = 400;
	float y = 300;
	float xold = 0;
	float yold = 0;
	float xdown = 0;
	float ydown = 0;

	// The renderer letterboxes the game picture through
	// SDL_RenderSetLogicalSize, but SDL input events keep arriving in raw
	// window coordinates (screenSurface is NULL, so GetRelativeX/Y pass
	// them through untouched). Map window coordinates into the engine's
	// logical (native) space right here, mirroring the renderer's math.
	// The scale/offset are recomputed for every single event instead of
	// being captured once: a window or logical-viewport change after the
	// capture (surface resize, recreated renderer, a second session) used
	// to leave the input mapped with stale numbers.
	#define VILE_MAP_INPUT(ix,iy,gx,gy) do{ \
		int logicalw_=0,logicalh_=0,winw_=0,winh_=0; \
		SDL_RenderGetLogicalSize(EDLRenderer,&logicalw_,&logicalh_); \
		SDL_GetWindowSize(window,&winw_,&winh_); \
		float vscale_=0,voffx_=0,voffy_=0; \
		if(logicalw_>=1 && logicalh_>=1 && winw_>0 && winh_>0){ \
		float sx_=(float)winw_/logicalw_; \
		float sy_=(float)winh_/logicalh_; \
		vscale_=(sx_<sy_)?sx_:sy_; \
		voffx_=((float)winw_-logicalw_*vscale_)/2.f; \
		voffy_=((float)winh_-logicalh_*vscale_)/2.f; \
		} \
		float fx_=(ix),fy_=(iy); \
		if(vscale_>0){ \
		fx_=(fx_-voffx_)/vscale_; \
		fy_=(fy_-voffy_)/vscale_; \
		if(fx_<0)fx_=0; \
		if(fy_<0)fy_=0; \
		if(fx_>logicalw_-1)fx_=logicalw_-1; \
		if(fy_>logicalh_-1)fy_=logicalh_-1; \
		} \
		(gx)=(int)fx_; \
		(gy)=(int)fy_; \
	}while(0)

	// Finger events arrive normalised against the window; the window
	// size is taken live right here as well, so no captured state is
	// involved anywhere in the touch path
	#define VILE_MAP_INPUT_FINGER(nx,ny,gx,gy) do{ \
		int winfw_=0,winfh_=0; \
		SDL_GetWindowSize(window,&winfw_,&winfh_); \
		VILE_MAP_INPUT((nx)*winfw_,(ny)*winfh_,gx,gy); \
	}while(0)

#ifdef IKURADROID_AUTODRIVE
        // TEMPORARY test hook (headless CI-style drive): if
        // IKURADROID_AUTODRIVE is set, a helper thread pushes synthetic
        // clicks/keys so the choice pipeline can be verified without a
        // human. Remove after diagnosis.
        if(getenv("IKURADROID_AUTODRIVE")){
                static SDL_Thread *autodrive_thread=SDL_CreateThread(
                                [](void*data)->int{
                                        SDL_Delay(3000);
                                        // Title menu: click the first menu
                                        // spot ("New Game", IH region 0)
                                        SDL_Event down,up;
                                        SDL_zero(down);
                                        SDL_zero(up);
                                        down.type=SDL_MOUSEBUTTONDOWN;
                                        down.button.button=SDL_BUTTON_LEFT;
                                        down.button.x=567;
                                        down.button.y=283;
                                        down.button.state=SDL_PRESSED;
                                        down.button.clicks=1;
                                        SDL_PushEvent(&down);
                                        up.type=SDL_MOUSEBUTTONUP;
                                        up.button.button=SDL_BUTTON_LEFT;
                                        up.button.x=567;
                                        up.button.y=283;
                                        up.button.state=SDL_RELEASED;
                                        up.button.clicks=1;
                                        SDL_PushEvent(&up);
                                        // Alternate clicks: title spot
                                        // ("New Game", skip demo), choice
                                        // item 0, item 1, background
                                        SDL_Delay(2000);
                                        int spots[][2]={{567,283},{567,283},{567,283},{567,283},{567,283},{567,283},{320,194},{320,278},{400,240}};
                                        int i=0;
                                        for(int n=0;n<2000;n++){
                                                SDL_Delay(250);
                                                SDL_zero(down);
                                                SDL_zero(up);
                                                int x=spots[i%9][0];
                                                int y=spots[i%9][1];
                                                i++;
                                                down.type=SDL_MOUSEBUTTONDOWN;
                                                down.button.button=SDL_BUTTON_LEFT;
                                                down.button.x=x;
                                                down.button.y=y;
                                                down.button.state=SDL_PRESSED;
                                                down.button.clicks=1;
                                                SDL_PushEvent(&down);
                                                up.type=SDL_MOUSEBUTTONUP;
                                                up.button.button=SDL_BUTTON_LEFT;
                                                up.button.x=x;
                                                up.button.y=y;
                                                up.button.state=SDL_RELEASED;
                                                up.button.clicks=1;
                                                SDL_PushEvent(&up);
                                        }
                                        return 0;
                                },"ikuradroid-autodrive",0);
                (void)autodrive_thread;
        }
#endif

	while(engine){
		// Tick engine
		engine->EventHostTick();
		// Handle events
		while(engine && SDL_PollEvent(&event)){
			if(event.type==SDL_MOUSEMOTION){
				// Basic mousemove event
				int gx,gy;
				VILE_MAP_INPUT(event.motion.x,event.motion.y,gx,gy);
				engine->EventHostMouseMove(
							screenSurface,gx,gy);
			}
			else if(event.type==SDL_MOUSEBUTTONDOWN){
				int gx,gy;
				VILE_MAP_INPUT(event.button.x,event.button.y,gx,gy);
			LogTest("Host mouse down: window=(%d,%d) "
			                "logical=(%d,%d) button=%d",
			                event.button.x,event.button.y,
			                gx,gy,event.button.button);
				// Left mouse button
				if(event.button.button==SDL_BUTTON_RIGHT){
					engine->EventHostMouseRightDown(
							screenSurface,gx,gy);
				}
				else{
					engine->EventHostMouseLeftDown(
							screenSurface,gx,gy);
				}
			}
			else if(event.type==SDL_MOUSEBUTTONUP){
				int gx,gy;
				VILE_MAP_INPUT(event.button.x,event.button.y,gx,gy);
			LogTest("Host mouse up: window=(%d,%d) "
			                "logical=(%d,%d) button=%d",
			                event.button.x,event.button.y,
			                gx,gy,event.button.button);
				// Left mouse button
				if(event.button.button==SDL_BUTTON_RIGHT){
					engine->EventHostMouseRightUp(
						screenSurface,gx,gy);
				}
				else{
					engine->EventHostMouseLeftUp(
							screenSurface,gx,gy);
				}
			}
			else if(event.type==SDL_FINGERMOTION){
				// Touch events (tfinger is normalised 0..1 against the window)
				int gx,gy;
				VILE_MAP_INPUT_FINGER(event.tfinger.x,
				                event.tfinger.y,gx,gy);
				engine->EventHostMouseMove(
				                        screenSurface,gx,gy);
			}
			else if(event.type==SDL_FINGERDOWN){
				// Android touches arrive here directly: SDL 2.0.3 used to
				// synthesise a mouse stream from them, but that stream is
				// what kept getting lost between the Java layer and this
				// loop on some devices. Touch stays touch end to end now.
				int gx,gy;
				VILE_MAP_INPUT_FINGER(event.tfinger.x,
				                event.tfinger.y,gx,gy);
                                LOGCAT("ikuradroid input: finger down norm=(%.3f,%.3f) logical=(%d,%d)",event.tfinger.x,event.tfinger.y,gx,gy);
				engine->EventHostMouseMove(
				                        screenSurface,gx,gy);
				engine->EventHostMouseLeftDown(
				                        screenSurface,gx,gy);
			}
			else if(event.type==SDL_FINGERUP){
				int gx,gy;
				VILE_MAP_INPUT_FINGER(event.tfinger.x,
				                event.tfinger.y,gx,gy);
                                LOGCAT("ikuradroid input: finger up norm=(%.3f,%.3f) logical=(%d,%d)",event.tfinger.x,event.tfinger.y,gx,gy);
				engine->EventHostMouseLeftUp(
				                        screenSurface,gx,gy);
			}
			else if(event.type==SDL_KEYDOWN){
				engine->EventHostKeyDown(event.key.keysym.sym);
			}
			else if(event.type==SDL_KEYUP){
				engine->EventHostKeyUp(event.key.keysym.sym);
			}
			else if(event.type==SDL_QUIT){
#ifdef __ANDROID__
				// Android has no window close button: a QUIT is
				// always the app UI (the M3 menu quit) or the
				// system asking for a straight exit, so take it
				// without the desktop-style "Exit game?" prompt.
				// The prompt still exists for the games' own menu
				// exits: StdHalt confirms first, then pushes this
				// very QUIT with the shutdown flag already set.
				bool exit_confirmed=true;
#else
				bool exit_confirmed=engine->GetShutdown();
#endif
				if(exit_confirmed){
					// Shut down engine and break out of loop
					engine->EventSaveSystem();
					delete engine;
					engine=0;
				}
				else{
					// Request shutdown
					engine->EventGameDialog(VD_SHUTDOWN);
				}
			}
	      //  SDL_RenderDrawPoint(EDLRenderer, x, y);

		}

		// Purge deleted objects
		Group::Purge();
	}
	#undef VILE_MAP_INPUT
	//*/
}

EngineVN *ViLE::LoadEngine(uString Path){
	// Assert trailing separator
	Path=EDL_StripPath(Path);
	int i=Path.length();
	if(i && Path[i-1]!='/' && Path[i-1]!='\\'){
#ifdef VILE_ARCH_MICROSOFT
		Path+="\\";
#else
		Path+="/";
#endif
	}
	//Path= "/home/nobodycares/proect/ViLE/bin/";
	// Try to load a game
	EngineVN *engine=0;
	if(0){
	}
#ifdef VILE_SUPPORT_WINDY
        else if(ProbeNocturnal(Path))                           engine=new Nocturnal(Path);
        else if(ProbeMayclub(Path))                                     engine=new Mayclub(Path);
#endif
#ifdef VILE_SUPPORT_JAST
        else if(ProbeJUMC(Path))                                        engine=new JUMC(Path);
#endif
#ifdef VILE_SUPPORT_CWARE
        else if(ProbeDiviDead(Path))                            engine=new DiviDead(Path);
#endif
#ifdef VILE_SUPPORT_WILL
        else if(ProbeYumeMiruKusuri(Path))                      engine=new YumeMiruKusuri(Path);
        else if(ProbePrincessWaltz(Path))                       engine=new PrincessWaltz(Path);
        else if(ProbeStarrySky(Path))                       engine=new StarrySky(Path);

        else if(ProbeCriticalPoint(Path))                       engine=new CriticalPoint(Path);
        else if(ProbeLittleMyMaid(Path))                        engine=new LittleMyMaid(Path);
#endif
#ifdef VILE_SUPPORT_TLOVE
        else if(ProbeTrueLove(Path)){                   engine=new Truelove(Path);}
#endif
#ifdef VILE_SUPPORT_IKURA
        else if(ProbeCrescendo(Path))                           engine=new Crescendo(Path);
        else if(ProbeHDR(Path))                                 engine=new Heartdr(Path);
        else if(ProbeVirgin(Path))                                  engine=new virgin(Path);
        else if(ProbeSagara(Path))                                      engine=new Sagara(Path);
        else if(ProbeSnow(Path))                                        engine=new Snow(Path);
        else if(ProbeKanaOkaeri(Path))                          engine=new KanaOkaeri(Path);
        else if(ProbeKana(Path))                                        engine=new Kana(Path);
        else if(ProbeHitomi(Path))                                      engine=new Hitomi(Path);
        else if(ProbeCatGirl(Path))                                     engine=new CatGirl(Path);
        else if(ProbeIdols(Path))                                       engine=new Idols(Path);
#endif
#ifdef VILE_SUPPORT_CROWD
        else if(ProbeTokimeki(Path))                            engine=new Tokimeki(Path);
        else if(ProbeXChange1(Path))                            engine=new XChange1(Path);
        else if(ProbeXChange3(Path))                            engine=new XChange3(Path);
#endif
        else                                                                            ;
	return engine;
}

/*! \brief Initializes (or resizes) video the display surface
 *  \return True if the video surface could be created
 *
 *  This method will use the configuration settings, and can be executed
 *  multiple times in case you want to reload the settings and resize the
 *  display.
 */

bool ViLE::InitVideo(){
	// Assert configuration
	bool retval=false;
	static Uint32 ow=0;
	static Uint32 oh=0;
	Uint32 w=Cfg::Display::Width;
	Uint32 h=Cfg::Display::Height;
	Uint32 d=Cfg::Display::Depth;
	Uint32 f=Cfg::Display::Flags;
	if(!w || !h){
		w=DEFAULT_WIDTH;
		h=DEFAULT_HEIGHT;
	}

	if(w && h && ow==w && oh==h){
		// Changing size to the same size ...
		retval=true;
	}

	window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN/*|SDL_WINDOW_FULLSCREEN_DESKTOP*/);

    EDLRenderer= SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
#ifdef IKURADROID_AUTODRIVE
    // TEMPORARY headless fallback: the SDL dummy video driver exposes no
    // accelerated renderer, so the automated drive test runs on software.
    if(!EDLRenderer){
        EDLRenderer= SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE|SDL_RENDERER_TARGETTEXTURE);
    }
#endif

   /* int real_width, real_height;
    SDL_GetWindowSize(window, &real_width, &real_height);
    printf( "real_width %d real_height %d\n", real_width, real_height );
    SDL_Rect viewport;
    viewport.x=(real_width-w)/2;
    viewport.y=(real_height-h)/2;
    viewport.w=real_width;
    viewport.h=real_height;

    SDL_RenderSetLogicalSize(EDLRenderer, w, h);*/

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");


    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    }
    else if( EDLRenderer == NULL )
    {
        // A dead renderer used to leave the engine running "blind":
        // the loop kept going with every draw call failing silently,
        // which shows up as an eternal black screen. Treat it as a
        // video failure so the session exits cleanly instead.
        printf( "Renderer could not be created! SDL_Error: %s\n", SDL_GetError() );
        VILE_STAGE("InitVideo: renderer creation failed");
    }
	else
    {
        retval=true;
		ow=w;
		oh=h;
    }


	return retval;
}

bool ViLE::InitSystem(Uint32 Systemflags){
	// Initialize SDL and load application object
	window = NULL;
	//The surface contained by the window
	screenSurface = NULL;
    renderer = NULL;

	bool retval=false;
	if (SDL_Init(Systemflags)<0) {

		LogError("SDL Initialization error: %s",SDL_GetError());
	}
	else if(TTF_Init()<0){
		LogError("Could not init TrueType: %s\n",TTF_GetError());
	}
	else if(IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG)==0){
		LogError("Failed to link to any image libraries: %s\n",IMG_GetError());
	}
	else{
   // avcodec_register_all();
  //  av_register_all();

		retval=true;
	}
#ifdef VILE_BUILD_W32CONSOLE
	RedirectIOToConsole();
#endif
	return retval;
}

void ViLE::Quit(){
	// Close internal systems
	Media::Close();

	// Close down dependencies
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void ViLE::Help(){
	// Show available options
	About();
        LogMessage("Usage: ikuradroid [OPTIONS|COMMAND]");
	LogMessage("");
	LogMessage("Display settings:");
	LogMessage("\t--fullscreen\t\t\tForces fullscreen mode");
	LogMessage("\t--size WIDTH HEIGHT\t\tSet screen size");
	LogMessage("\t--depth 8|16|24|32\t\tSet bit per pixel for screen");
	LogMessage("\t--fontface PATH\t\t\tDefault truetype font file");
	LogMessage("\t--fontsize SIZE\t\t\tDefault font size (pixel height)");
	LogMessage("");
	LogMessage("Audio configuration:");
	LogMessage("\t--disable-audio\t\t\tDo not load any audio resources");
	LogMessage("\t--disable-midi\t\t\tDo not load MIDI tracks or soundfonts");
	LogMessage("\t--cdrom INDEX\t\t\tSet source for CDDA audio (=auto)");
	LogMessage("\t--samplerate SAMPLERATE\t\tSamples per second (=44100)");
	LogMessage("\t--buffersize BYTES\t\tBytes per frame (=1024)");
	LogMessage("\t--buffercount NUMBER\t\tBuffered frames (=4)");
	LogMessage("\t--soundfont FILENAME\t\tSoundfont for MIDI synthesizer");
	LogMessage("");
	LogMessage("Video configuration:");
	LogMessage("\t--disable-video\t\t\tDo not load any video resources");
	LogMessage("\t--disable-overlay\t\tDisable hardware video rendering");
	LogMessage("");
	LogMessage("Other:");
	LogMessage("\t--config PATH\t\t\tSpecify configuration file");
	LogMessage("\t--cwd FOLDER\t\t\tSet current working directory");
	LogMessage("\t--game FOLDER\t\t\tLoad game from specified path");
	LogMessage("\t--save FOLDER\t\t\tPath for savegames");
	LogMessage("\t--disable-menu\t\t\tDo not show application menu bar");
	LogMessage("\t--logcolor\t\t\tFormat log output with colors");
	LogMessage("\t--logfile PATH\t\t\tSpecify output log file");
	LogMessage("\t--keyfile PATH\t\t\tLoad encryption key from file");
	LogMessage("\t--verbose\t\t\tPrint additional runtime information");
	LogMessage("\t--dryrun\t\t\tLoad as usual but prevent from starting");
	LogMessage("\t--info [title|res|runtime]\tDisplay runtime information");
	LogMessage("");
	LogMessage("Commands:");
#if VILE_FEATURE_UNITTEST
	LogMessage("\t--unittest all||...\t\tUnit-test core class' functionality");
	LogMessage("\t--benchmark all|...\t\tBenchmark core class' performance");
#endif
	LogMessage("\t--list ARC\t\t\tLists resources in archive");
	LogMessage("\t--extract ARC [RES [RES ..]]\tExtract resource from archive");
	LogMessage("\t--decode INFILE [INFILE ..]\tDecode resources");
	LogMessage("\t--xdec ARC [RES [RES ..]]\tExtract AND decode resources");
	LogMessage("\t--archive ARC INFILE [INFILE..]\tArchives resources");
	LogMessage("\t--help\t\t\t\tThis helpfull message");
        LogMessage("\t--about\t\t\t\tAbout IkuraDroid");
	LogMessage("");
	LogMessage("");
	LogMessage("You can also use the following hotkeys at runtime:");
	LogMessage("\tALT+1\t\t\t\tNo graphic filters");
	LogMessage("\tALT+3\t\t\t\tHQ2X graphic filter");
	LogMessage("\tALT+4\t\t\t\tHQ3X graphic filter");
	LogMessage("\tALT+5\t\t\t\tHQ4X graphic filter");
	LogMessage("\tALT+ENTER\t\t\tToggle fullscreen mode");
	LogMessage("\tALT+F4\t\t\t\tExit");
	LogMessage("\tF5\t\t\t\tLoad game");
	LogMessage("\tF6\t\t\t\tSave game");
	LogMessage("\tF7\t\t\t\tOptions dialog");
	LogMessage("\tF8\t\t\t\tTitle screen");
	LogMessage("\tF9\t\t\t\tExit");
	LogMessage("\tF10\t\t\t\tDump screenshot to file");
	LogMessage("\tF11\t\t\t\tToggle debug output");
	LogMessage("");
}

void ViLE::About(){
	LogMessage("");
	LogMessage("Visual Library Engine");
	LogMessage("%s (%s, Rev:%s)",PACKAGE_STRING,PACKAGE_LINE,SCM_VERSION);
	LogMessage("2011 (C) ViLE Team <http://vilevn.org>");
	LogMessage("Released under GNU GPLv3 <http://gnu.org/licenses/gpl.html>");
	LogMessage("");
}

void ViLE::InfoTitle(EngineVN *Engine){
	LogMessage("Game information:");
	LogMessage("\tID:%s",Engine->NativeID().c_str());
	LogMessage("\tName:%s",Engine->NativeName().c_str());
	LogVerbose("\tDirectory: %s",Cfg::Path::game.c_str());
	LogMessage("\tResolution:%dx%d",
			Engine->NativeWidth(),
			Engine->NativeHeight());
}

void ViLE::InfoResources(EngineVN *Engine){
	Stringlist list;
	LogMessage("Resource information:");
	if(Engine->GetVoices(&list)){
		LogMessage("\tVoices:%s",list.Enumerate().c_str());
	}
	if(Engine->GetScripts(&list)){
		LogMessage("\tScripts:%s",list.Enumerate().c_str());
	}
	if(Engine->GetImages(&list)){
		LogMessage("\tImages:%s",list.Enumerate().c_str());
	}
	if(Engine->GetBGM(&list)){
		LogMessage("\tBGM:%s",list.Enumerate().c_str());
	}
	if(Engine->GetSE(&list)){
		LogMessage("\tSE:%s",list.Enumerate().c_str());
	}
	if(Engine->GetVideo(&list)){
		LogMessage("\tVideo:%s",list.Enumerate().c_str());
	}
	if(Engine->GetOther(&list)){
		LogMessage("\tOther:%s",list.Enumerate().c_str());
	}
}

void ViLE::InfoRuntime(){
	// Dump compiletime information
	LogMessage("Version information");
	LogMessage("\tCompiled: %s",COMPILATION_DATE);
	LogMessage("\tVersion: %s (%s)",PACKAGE_STRING,PACKAGE_LINE);
	LogMessage("\tRevision: %s",SCM_VERSION);
#if VILE_ARCH_MICROSOFT
	LogMessage("\tArch: Windows");
#elif VILE_ARCH_LINUX
	LogMessage("\tArch: Linux");
#elif VILE_ARCH_FREEBSD
	LogMessage("\tArch: FreeBSD");
#else
	LogMessage("\tArch: UNSUPPORTED");
#endif

	// List enabled engines
	LogMessage("\r\nEnabled sub-engines");
#if VILE_SUPPORT_WINDY
	LogMessage("\tWindy");
#endif
#if VILE_SUPPORT_IKURA
	LogMessage("\tIkuraGDL");
#endif
#if VILE_SUPPORT_JAST
	LogMessage("\tJAST Engine");
#endif
#if VILE_SUPPORT_CROWD
	LogMessage("\tCrowd Engine 3");
#endif

	// List linked library version numbers
	LogMessage("\r\nLinked libraries");
	LogMessage("\tSDL:\t\t%d.%d.%d",
			SDL_MAJOR_VERSION,
			SDL_MINOR_VERSION,
			SDL_PATCHLEVEL);
	LogMessage("\tSDL_image:\t%d.%d.%d",
			SDL_IMAGE_MAJOR_VERSION,
			SDL_IMAGE_MINOR_VERSION,
			SDL_IMAGE_PATCHLEVEL);
	LogMessage("\tSDL_ttf:\t%d.%d.%d",
			SDL_TTF_MAJOR_VERSION,
			SDL_TTF_MINOR_VERSION,
			SDL_TTF_PATCHLEVEL);
#if VILE_FEATURE_FLUIDSYNTH
	LogMessage("\tFluidsynth:\t%d.%d.%d",
			FLUIDSYNTH_VERSION_MAJOR,
			FLUIDSYNTH_VERSION_MINOR,
			FLUIDSYNTH_VERSION_MICRO);
#endif

	LogMessage("");
}

bool ViLE::Archive(uString Archive,Stringlist *List){
	Resources resman;
	int count=List->GetCount();
    LogMessage("IkuraDroid::Archive count %d ",count);
	for(int i=0;i<count;i++){
		resman.AddResource(List->GetString(i));
	}
	return resman.Write(Archive);
}

bool ViLE::Decode(uString InFile){
	bool retval=false;
	Resources resman;
	resman.AddResource(InFile);
	SDL_Surface *image=resman.GetImage(InFile);
	SDL_Surface **animation=resman.GetAnimation(InFile);
	uString OutFile=EDL_FileDirectory(InFile)+EDL_FileName(InFile)+".bmp";
	if(animation){
		char buffer[OutFile.length()+10];
		LogMessage("Converting animation resource to bitmaps: %s -> %s",
				InFile.c_str(),OutFile.c_str());
		for(int i=0;animation[i];i++){
			sprintf(buffer,"%s_%02d.bmp",OutFile.c_str(),i+1);
			SDL_SaveBMP(animation[i],buffer);
			SDL_FreeSurface(animation[i]);
			retval=true;
		}
		delete [] animation;
	}
	else if(image){
		LogMessage("Converting image resource to bitmap: %s -> %s",
				InFile.c_str(),OutFile.c_str());
		SDL_SaveBMP(image,OutFile.c_str());
		SDL_FreeSurface(image);
		retval=true;
	}
	else{
		LogMessage("Not a decodable resource: %s",InFile.c_str());
	}
	return retval;
}

/*! \brief Lists resources in a game archive
 *  \param Archive Archive to list
 *  \return True if any resources where found
 */
bool ViLE::List(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			LogMessage("%s",list.GetString(i).c_str());
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Lists resources in a game archive
 *  \param Archive Archive to list
 *  \return True if any resources where found
 */
bool ViLE::Extract(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			uString name=list.GetString(i);
			RWops *res=resman.GetResource(name);
			if(res){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts a resource from a game archive
 *  \param Archive Archive to extract from
 *  \param List Resources to extract
 *  \return True if output file was successfully written
 */
bool ViLE::Extract(uString Archive,Stringlist *List){
	bool retval=false;
	Resources resman;
	if(List && resman.AddResource(Archive)>0){
		int c=List->GetCount();
		for(int i=0;i<c;i++){
			uString name=List->GetString(i);
			RWops *res=resman.GetResource(name);
			if(res){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
			else{
				LogMessage("Invalid resource: %s",name.c_str());
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts AND decodes resources from an archive
 *  \param Archive Archive to extract from
 *  \return True if any resources where found
 */
bool ViLE::XDec(uString Archive){
	bool retval=false;
	Resources resman;
	if(resman.AddResource(Archive)>0){
		Stringlist list;
		int c=resman.EnumerateResources(&list);
		retval=c>0;
		for(int i=0;i<c;i++){
			char buffer[32];
			uString name=list.GetString(i);
			SDL_Surface **animation;
			SDL_Surface *image;
			RWops *res;
			if((animation=resman.GetAnimation(name))){
				LogMessage("Converting animation to frames: %s",name.c_str());
				for(int i=0;animation[i];i++){
					if(i==0 && animation[1]==0){
						sprintf(buffer,"%s.bmp",name.c_str());
					}
					else{
						sprintf(buffer,"%s_%02d.bmp",name.c_str(),i+1);
					}
					SDL_SaveBMP(animation[i],buffer);
					SDL_FreeSurface(animation[i]);
					retval=true;
				}
				delete [] animation;
			}
			else if((image=resman.GetImage(name))){
				LogTest("Converting %s to bitmap",name.c_str());
				name+=".bmp";
				SDL_SaveBMP(image,name.c_str());
				SDL_FreeSurface(image);
				retval=true;
			}
			else if((res=resman.GetResource(name))){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					delete res;
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

/*! \brief Extracts AND decodes resources from an archive
 *  \param Archive Archive to extract from
 *  \param List Resources to extract
 *  \return True if any resources where found
 */
bool ViLE::XDec(uString Archive,Stringlist *List){
	bool retval=false;
	Resources resman;
	if(List && resman.AddResource(Archive)>0){
		int c=List->GetCount();
		for(int i=0;i<c;i++){
			char buffer[32];
			uString name=List->GetString(i);
			SDL_Surface **animation;
			SDL_Surface *image;
			RWops *res;
			if((animation=resman.GetAnimation(name))){
				LogMessage("Converting animation to frames: %s",name.c_str());
				for(int i=0;animation[i];i++){
					if(i==0 && animation[1]==0){
						sprintf(buffer,"%s.bmp",name.c_str());
					}
					else{
						sprintf(buffer,"%s_%02d.bmp",name.c_str(),i+1);
					}
					SDL_SaveBMP(animation[i],buffer);
					SDL_FreeSurface(animation[i]);
					retval=true;
				}
				delete [] animation;
			}
			else if((image=resman.GetImage(name))){
				LogTest("Converting %s to bitmap",name.c_str());
				name+=".bmp";
				SDL_SaveBMP(image,name.c_str());
				SDL_FreeSurface(image);
				retval=true;
			}
			else if((res=resman.GetResource(name))){
				res->Seek(0,SEEK_END);
				int size=res->Tell();
				if(size>0){
					Uint8 *buffer=new Uint8[size];
					res->Seek(0,SEEK_SET);
					res->Read(buffer,size);
					delete res;
					FILE *output=fopen(name.c_str(),"wb");
					if(output){
						LogMessage("Writing %d bytes to %s",
								size,name.c_str());
						fwrite(buffer,1,size,output);
						fclose(output);
						retval=true;
					}
					else{
						LogError("Could not write target: %s",
								name.c_str());
					}
					delete [] buffer;
				}
			}
		}
	}
	else{
		LogError("Not a valid archive: %s",Archive.c_str());
	}
	return retval;
}

void ViLE::Error(uString Title,uString Message){
	// Report failed loading
	EngineVN *evn=new EngineVN(640,480);

	while(evn->DestroyAnimation());
	Widget *widget=new Widget(0,0,640,480);
	widget->Fill(0xFFFFFFFF);
	Fatal *fatal=new Fatal(evn,Title,Message);
	evn->DestroyLayer(VL_DIALOG);
	evn->AddWidget(widget,VL_BACKGROUND);
	evn->AddWidget(fatal,VL_DIALOG);
	RunEngine(evn);
}


