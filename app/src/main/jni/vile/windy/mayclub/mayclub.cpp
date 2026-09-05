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

#include "mayclub.h"

Mayclub::Mayclub(uString Path) : EngineWindy(640,480) {
	// Set defaults
	sbuffer0=0;
	sbuffer1=0;
	sbuffer2=0;

	// Load english censorship patches (Optional)
	//AddScripts(new ArchiveFiles(Path+"A04.S"));
	//AddScripts(new ArchiveFiles(Path+"AH1.S"));
	//AddScripts(new ArchiveFiles(Path+"OPEN.S"));

	// Load english legacy patches
	AddScripts(new ArchiveFiles(Path+"B03.S"));
	AddScripts(new ArchiveFiles(Path+"B03B.S"));
	AddScripts(new ArchiveFiles(Path+"B06.S"));
	AddScripts(new ArchiveFiles(Path+"B07.S"));
	AddScripts(new ArchiveFiles(Path+"BE2.S"));
	AddScripts(new ArchiveFiles(Path+"G15.S"));

	// Load english legacy resources
	AddScripts(new ArchiveWindy(Path+"may0.dat"));
	AddOther(new ArchiveFiles(Path+"memory.dat"));

	// Load new japanese resources
	AddImages(new ArchiveS21(Path+"cg/graphic.pak"));
	AddSE(new ArchiveS21(Path+"se/se.pak"));
	AddVoices(new ArchiveS21(Path+"voice/voice.pak"));
	AddBGM(new ArchiveFiles(Path+"wave/*.wav"));

	// Create and assign specialized textview object
	w_tv=new MCTextview(this);
	AddWidget(w_tv,VL_TEXTVIEW);

	// Create and assign voice syncronizer
	whisper=new MCWhisper(this);
	whisper->AddResource(new ArchiveS21(Path+"snr/Scenario.pak"));

	// Preload graphics for selection
	char bbitmap[32];
	char bmask[32];
	for(int i=1;i<5;i++){
		sprintf(bbitmap,"SEL2_%d.bet",i);
		sprintf(bmask,"SEL2_%d_.bet",i);
		SDL_Surface *selbitmap=LoadImage(bbitmap);
		SDL_Surface *selmask=LoadImage(bmask);
		if(selbitmap && selmask){
			sselection[i-1]=EDL_MaskSurface(selbitmap,selmask,0);
			SDL_FreeSurface(selmask);
			SDL_FreeSurface(selbitmap);
		}
		else{
			LogError("Failed to load selection graphics: SEL2_%d.bet",i);
			sselection[i-1]=0;
		}
	}

	// Map cgresources
	int i=0;
	RegisterCG(i++,"a03");
	RegisterCG(i++,"a04");
	RegisterCG(i++,"a06");
	RegisterCG(i++,"a07");
	RegisterCG(i++,"a08");
	RegisterCG(i++,"a09");
	RegisterCG(i++,"a10");
	RegisterCG(i++,"b12");
	RegisterCG(i++,"b13");
	RegisterCG(i++,"b14");
	RegisterCG(i++,"b16");
	RegisterCG(i++,"b17a");
	RegisterCG(i++,"b17b");
	RegisterCG(i++,"b17c");
	RegisterCG(i++,"b18");
	RegisterCG(i++,"b19");
	RegisterCG(i++,"c21");
	RegisterCG(i++,"c22a");
	RegisterCG(i++,"c22b");
	RegisterCG(i++,"c23");
	RegisterCG(i++,"c24");
	RegisterCG(i++,"c25");
	RegisterCG(i++,"c26");
	RegisterCG(i++,"d28");
	RegisterCG(i++,"d29");
	RegisterCG(i++,"d30a");
	RegisterCG(i++,"d30b");
	RegisterCG(i++,"d31");
	RegisterCG(i++,"d32a");
	RegisterCG(i++,"d32b");
	RegisterCG(i++,"d33a");
	RegisterCG(i++,"d33b");
	RegisterCG(i++,"d34");
	RegisterCG(i++,"e36");
	RegisterCG(i++,"e37");
	RegisterCG(i++,"e38a");
	RegisterCG(i++,"e38b");
	RegisterCG(i++,"e39");
	RegisterCG(i++,"e40");
	RegisterCG(i++,"e41");
	RegisterCG(i++,"f36");
	RegisterCG(i++,"f37");
	RegisterCG(i++,"f38");
	RegisterCG(i++,"f39");
	RegisterCG(i++,"f40");
	RegisterCG(i++,"g42");
	RegisterCG(i++,"g43a");
	RegisterCG(i++,"g43b");
	RegisterCG(i++,"g43c");

	// Map story resources
	i=0;
	RegisterScene(i++,"ah1");
	RegisterScene(i++,"ah2");
	RegisterScene(i++,"bh1");
	RegisterScene(i++,"bh2");
	RegisterScene(i++,"ch1");
	RegisterScene(i++,"dh1");
	RegisterScene(i++,"dh2");
	RegisterScene(i++,"eh1");
	RegisterScene(i++,"eh2");
	RegisterScene(i++,"ee1");		// Double check
	RegisterScene(i++,"gh1");
	RegisterScene(i++,"hh1");
	RegisterScene(i++,"hh2");
	RegisterScene(i++,"ih1");
	RegisterScene(i++,"ih2");

	// Load common routines
	LoadWindyRoutines("MAY_SUB.S");

	// Load main menu
	main=new MCMain(this);
	AddWidget(main,VL_DIALOG);
	EventGameDialog(VD_TITLE);
}

Mayclub::~Mayclub(){
	// Free graphics
	for(int i=0;i<4;i++){
		if(sselection[i]){
			SDL_FreeSurface(sselection[i]);
		}
	}

	// Delete buffers
	if(sbuffer0){
		SDL_FreeSurface(sbuffer0);
		sbuffer0=0;
	}
	if(sbuffer1){
		SDL_FreeSurface(sbuffer1);
		sbuffer1=0;
	}
	if(sbuffer2){
		SDL_FreeSurface(sbuffer2);
		sbuffer2=0;
	}
}

bool Mayclub::EventBackgroundMouseRightUp(int X,int Y){
	// Show options dialog
	w_tv->SetVisible(!w_tv->GetVisible());
	return true;
}

void Mayclub::EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag){
	// Check for valid textmodes
	int x=0,y=0,w=0,h=0;
	if(Mode==0x0003 || Mode==0x000B){
		if(X==0x0002 && Y==0x0013){
			// Normal text mode
			x=80;
			y=384;
			w=480;
			h=84;
		}
		else{
			LogDebug("Unknown text position for mayclub: %d(%d,%d)",Mode,X,Y);
		}
	}
	else{
		LogError("Unknown textmode for mayclub: %d(%d,%d)",Mode,X,Y);
	}

	// Change textmode if applicable
	if(x || y || w || h){
		w_tv->ClearText();
		w_tv->SetTextPosition(x,y,w,h);
	}
}

/*! \brief Override routines to match updated gui
 *  \param Address Address of external routine
 *  \return True if the call was handled internally
 *
 *  In addition to accommodating the changes in gui, we also override the
 *  blitting routines to avoid meaningless loops (Ie. built-in graphical
 *  transition effects).
 */
bool Mayclub::EventExternal(Uint16 Address){
	bool retval=false;

	// Override slow looping routines
	if(Address==0x4100){
		w_bg->Blit(sbuffer1);
		retval=true;
	}
	else if(Address==0x4104){
		if(!sbuffer1){
			sbuffer1=EDL_CreateSurface(NativeWidth(),NativeHeight());
		}
		SDL_Rect full={0,0,sbuffer1->w,sbuffer1->h};
		EDL_SetBox(sbuffer1,0,0,full.w,full.h,0,0,0,0xFF);
		w_bg->Blit(sbuffer1);
		retval=true;
	}
	else if(Address==0x4110){
		w_bg->Blit(sbuffer1);
		retval=true;
	}
	else if(Address==0x4114){
		w_bg->Blit(sbuffer2);
		retval=true;
	}
	else if(Address==0x4562){
		w_bg->Blit(sbuffer1);
		retval=true;
	}
	else if(Address==0x4B52){
		w_bg->Blit(sbuffer1);
		w_bg->Blend(sbuffer2);
		retval=true;
	}

	// Override mayclub specific gui routines
	else if(Address==0x4144){
		// Update time and date display
		Uint16 month=vars.GetUint16(0x0127);
		Uint16 day=vars.GetUint16(0x0128);
		Uint16 time=vars.GetUint16(0x0129);
		Uint16 dayofweek=vars.GetUint16(0x012B);
		MCTextview *w_mctv=(MCTextview*)w_tv;
		w_mctv->SetTime(month,day,time,dayofweek);
		retval=true;
	}
	else if(Address==0x415C){
		// Update credits display
		Uint16 credits=vars.GetUint16(0x0126);
		MCTextview *w_mctv=(MCTextview*)w_tv;
		w_mctv->SetTickets(credits);
		retval=true;
	}

	// Report unknown adresses (Debugging only)
	else if(Address!=0x4120 &&
			Address!=0x412C &&
			Address!=0x4130 &&
			Address!=0x4134 &&
			Address!=0x4140 &&
			Address!=0x4148 &&
			Address!=0x414C &&
			Address!=0x48C0 &&
			Address!=0x60B0){
		LogDebug("Calling mayclub external: 0x%04x",Address);
	}
	return retval;
}

void Mayclub::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_TITLE){
		DestroyLayer(VL_CHOICES);
		StopMusic();
		StopSound();
		main->SetVisible(true);
		PlayMusic("01");
	}
	else if(Dialog==VD_SAVE){
		AddWidget(new MCSave(this),VL_DIALOG);
	}
	else if(Dialog==VD_LOAD){
		AddWidget(new MCLoad(this),VL_DIALOG);
	}
	else if(Dialog==VD_LOG){
		AddWidget(new MCBack(this,w_tv),VL_DIALOG);
	}
	else if(Dialog==VD_CREDITS){
		// Scroll staffroll
		SDL_Surface *surface=LoadImage("STAFF.BET");
		if(surface){
			Sequences->RollPaddedVertical(surface);
			Sequences->LogoRandom(0,0,0);
			SDL_FreeSurface(surface);
		}
		else{
			LogError("Failed to load staffroll");
		}

		// Fade in titlescreen
		if(main){
			SDL_Rect dst={0,0,NativeWidth(),NativeHeight()};
			AddAnimation(new Fade(dst,main->GetSurface(),dst,5000));
		}
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

void Mayclub::EventNew(){
	// Preset values
	EngineWindy::EventNew();
	vars.SetUint16(0x0126,0x0010);    // Credits
	vars.SetUint16(0x0127,0x0002);    // Month
	vars.SetUint16(0x0128,0x0010);    // Day
	vars.SetUint16(0x0129,0x0002);    // Time of day
	vars.SetUint16(0x0206,0xffff);    // Cached background
	vars.SetUint16(0x020c,0xffff);    // Cached foreground

	// Prepare textview
	MCTextview *w_mctv=(MCTextview*)w_tv;
	w_mctv->SetTickets(0);
	w_mctv->SetTime(2,16,0,3);

	// Load initialization code
	LoadWindyScript("OPEN.S",0x0245);
	SetTransition();
}

bool Mayclub::EventSave(int Index){
	bool retval=EngineWindy::EventSave(Index);
	if(retval){
		Savegame *save=new Savegame(NativeID(),Index);
		save->Read();
		if(sbuffer1){
			save->SaveSurface("screen-background",sbuffer1);
		}
	}
	return retval;
}

bool Mayclub::EventLoad(int Index){
	bool retval=EngineWindy::EventLoad(Index);
	if(retval){
		// Load mayclub specific data
		Savegame *load=new Savegame(NativeID(),Index);
		load->Read();
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-background",&tmps)){
			if(sbuffer1){
				SDL_FreeSurface(sbuffer1);
			}
			sbuffer1=tmps;
		}
		delete load;

		// Update credits, time and date display
		Uint16 month=vars.GetUint16(0x0127);
		Uint16 day=vars.GetUint16(0x0128);
		Uint16 time=vars.GetUint16(0x0129);
		Uint16 dayofweek=vars.GetUint16(0x012B);
		Uint16 credits=vars.GetUint16(0x0126);
		MCTextview *w_mctv=(MCTextview*)w_tv;
		w_mctv->SetTime(month,day,time,dayofweek);
		w_mctv->SetTickets(credits);

		// Make sure the mainscreen is hidden
		main->SetVisible(false);
	}
	return retval;
}

uString Mayclub::GetImagename(uString Imagename){
	uString tmp=EDL_Searchname(Imagename);
	if(tmp=="z65y")			return "z65n";
	else					return Imagename;
}

bool Mayclub::LoadGraphics(uString Resource,Uint16 Index,Uint16 Type){
	// Load image according to format
	bool retval;
	uString resource=GetImagename(Resource);
	SDL_Surface *surface=LoadImage(resource.c_str());
	if((retval=surface)){
		// Force off alpha??
		//SDL_SetAlpha(surface,0,0xFF);
        SDL_SetSurfaceAlphaMod(surface,0xFF);

		// Get name of mask bitmap
		uString name=resource;
		uString ext;
		for(int i=name.length();i>0;i--){
			if(name[i-1]=='.'){
				ext=name.substr(i-1);
				name=name.substr(0,i-1);
				break;
			}
			else if(name[i-1]=='/' || name[i-1]=='\\'){
				break;
			}
		}

		// Mask image if possible
		name+="_";
		name+=ext;
		SDL_Surface *mask=LoadImage(name);
		if(mask){
			SDL_Surface *tmp=EDL_MaskSurface(surface,mask,0);
			SDL_FreeSurface(mask);
			SDL_FreeSurface(surface);
			surface=tmp;
		}

		// Sent to destination
		if(Index==0x0000){
			if(sbuffer0){
				SDL_FreeSurface(sbuffer0);
			}
			sbuffer0=surface;
		}
		else if(Index==0x0001){
			if(sbuffer1){
					SDL_FreeSurface(sbuffer1);
				}
				sbuffer1=surface;
			}
		else if(Index==0x0002){
			if(sbuffer2){
				SDL_FreeSurface(sbuffer2);
			}
			sbuffer2=surface;
		}
		else{
			LogError("Unknown target surface: %04x (%d)",Index,Type);
			SDL_FreeSurface(surface);
		}
	}
	else{
		LogError("Failed to load surface: %s",Resource.c_str());
	}
	return retval;
}

void Mayclub::FillGraphics(Uint16 Index){
	int w=NativeWidth();
	int h=NativeHeight();
	SDL_Surface *source=EDL_CreateSurface(w,h);
	EDL_SetBox(source,0,0,w,h,0,0,0,0xFF);
	if(Index==0x0000){
		if(sbuffer0){
			SDL_FreeSurface(sbuffer0);
		}
		sbuffer0=source;
	}
	else if(Index==0x0001){
		if(sbuffer1){
			SDL_FreeSurface(sbuffer1);
		}
		sbuffer1=source;
	}
	else if(Index==0x0002){
		if(sbuffer2){
			SDL_FreeSurface(sbuffer2);
		}
		sbuffer2=source;
	}
	else{
		LogError("Invalid fill target: 0x%x",Index);
		SDL_FreeSurface(source);
	}
}

const uString Mayclub::NativeID(){
	return "May";
}

const uString Mayclub::NativeName(){
	return "Mayclub VR Dating SX";
}

bool Mayclub::PlayMusic(uString Name){
	// Assert prefix
	bool retval=true;
	if(Name.length()==1){
		Name=uString("0")+Name;
	}
	if(Name.length() && Name[0]!='n' && Name[0]!='N'){
		Name=uString("n")+Name;
	}

	// Drop postfix if any
	for(int i=Name.length();i>0;i--){
		if(Name[i]=='_'){
			Name=Name.substr(0,i);
			break;
		}
	}

	// Parse a basename
	uString base=Name;
	for(int i=Name.length();i>0;i--){
		if(base[i]=='.'){
			base=base.substr(0,i);
			break;
		}
	}

	// Try various name combinations
	if(!EngineVN::QueueMusic(Name)){
		Name=base;
		Name+="_88p.wav";
		if(!EngineVN::QueueMusic(Name)){
			Name=base;
			Name+="_88.wav";
			if(!EngineVN::QueueMusic(Name)){
				Name=base;
				Name+="_55.wav";
				if(!EngineVN::QueueMusic(Name)){
					retval=false;
				}
			}
		}
	}
	return retval;
}

Widget *Mayclub::LoadSelection(Stringlist Strings){
	int selcount=Strings.GetCount();
	if(selcount>3){
		selcount=3;
	}
	if(selcount>=0 && sselection[selcount]){
		// Assign best-effort bitmap
		Selection *seldialog=new Selection(this);
		int gw=NativeWidth();
		int gh=NativeHeight();
		seldialog->SetAlignment(HA_CENTER,VA_CENTER);
		seldialog->Resize(sselection[selcount]->w,sselection[selcount]->h);
		seldialog->Move((gw-sselection[selcount]->w)/2,
				(gh-sselection[selcount]->h)/2);
		seldialog->Set(sselection[selcount],0,0);
		seldialog->SetText(&Strings);
		seldialog->SetFocusItem(0);
		return seldialog;
	}
	else{
		LogError("Failed to load selection cache: %d",selcount);
		return 0;
	}
}


