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

#include "nocturnal.h"

Nocturnal::Nocturnal(uString Path) : EngineWindy(640,480) {
	// Set defaults
	background=0;
	menu_main_cache=0;
	menu_sub_cache=0;
	menu_main_flag=false;

	// Add ViLE patches
	AddImages(new ArchiveViLE(Path+"nirdecensor.pck"));

	// Load english patches
	AddScripts(new ArchiveFiles(Path+"OPEN.S"));
	AddScripts(new ArchiveFiles(Path+"9_42.S"));
	AddScripts(new ArchiveFiles(Path+"B_07.S"));
	AddScripts(new ArchiveFiles(Path+"B_08.S"));
	AddScripts(new ArchiveFiles(Path+"B_17.S"));

	// Load english legacy resources
	AddScripts(new ArchiveWindy(Path+"mug0.dat"));
	AddOther(new ArchiveFiles(Path+"svs"));
	AddOther(new ArchiveFiles(Path+"svx"));
	AddSE(new ArchiveFiles(Path+"*.WAV"));

	// Load new japanese resources
	AddImages(new ArchiveIkura(Path+"GRP"));
	//AddSE(new ArchiveIkura(Path+"SE"));
	AddVoices(new ArchiveIkura(Path+"v1"));
	AddVoices(new ArchiveIkura(Path+"v2"));
	AddBGM(new ArchiveIkura(Path+"BGW"));

	// Assert subroutines
	LoadWindyRoutines("MUG_SUB.S");

	// Create and assign voice syncronizer
	//whisper=new NIWhisper(this);
	//whisper->AddResource(new ArchiveIkura(Path+"SNR"));

	// Cache menu backgrounds
	iselection=0;
	sselection[0]=LoadImage("SEL.GGD");
	sselection[1]=LoadImage("SELLAST.GGD");
	sselection[2]=LoadImage("SEL0.GGD");

	// Create and assign textview object
	SDL_Surface *stv=LoadImage("WAKU_H.GGD");
	if(stv){
		w_tv=new Textview(this);
		w_tv->SetTextPosition(15,10,610,80);
		w_tv->MoveDialog(0,NativeHeight()-stv->h);
		w_tv->Resize(stv->w,stv->h);
		w_tv->Set(stv);
		AddWidget(w_tv,VL_TEXTVIEW);
		SDL_FreeSurface(stv);
	}
	else{
		LogError("Could not load textview: WAKU_H.GGD");
	}

	// Map cgresources
	int i=0;
	RegisterCG(i++,"007");
	RegisterCG(i++,"017a");
	RegisterCG(i++,"017ao");
	RegisterCG(i++,"017b");
	RegisterCG(i++,"017c");
	RegisterCG(i++,"018");
	RegisterCG(i++,"019");
	RegisterCG(i++,"020");
	RegisterCG(i++,"021");
	RegisterCG(i++,"022");
	RegisterCG(i++,"023");
	RegisterCG(i++,"024");
	RegisterCG(i++,"091");
	RegisterCG(i++,"103");
	RegisterCG(i++,"120");
	RegisterCG(i++,"026");
	RegisterCG(i++,"027");
	RegisterCG(i++,"028");
	RegisterCG(i++,"030a");
	RegisterCG(i++,"030b");
	RegisterCG(i++,"030c");
	RegisterCG(i++,"030d");
	RegisterCG(i++,"031");
	RegisterCG(i++,"032");
	RegisterCG(i++,"033");
	RegisterCG(i++,"034");
	RegisterCG(i++,"035");
	RegisterCG(i++,"036");
	RegisterCG(i++,"092");
	RegisterCG(i++,"102");
	RegisterCG(i++,"114");
	RegisterCG(i++,"038");
	RegisterCG(i++,"040");
	RegisterCG(i++,"041");
	RegisterCG(i++,"042");
	RegisterCG(i++,"093");
	RegisterCG(i++,"104");
	RegisterCG(i++,"105");
	RegisterCG(i++,"106");
	RegisterCG(i++,"116");
	RegisterCG(i++,"044");
	RegisterCG(i++,"045");
	RegisterCG(i++,"046");
	RegisterCG(i++,"047");
	RegisterCG(i++,"094a");
	RegisterCG(i++,"112");
	RegisterCG(i++,"113");
	RegisterCG(i++,"050");
	RegisterCG(i++,"051");
	RegisterCG(i++,"053");
	RegisterCG(i++,"054a");
	RegisterCG(i++,"054b");
	RegisterCG(i++,"055");
	RegisterCG(i++,"056");
	RegisterCG(i++,"057");
	RegisterCG(i++,"095");
	RegisterCG(i++,"119");
	RegisterCG(i++,"058d");
	RegisterCG(i++,"059");
	RegisterCG(i++,"060a");
	RegisterCG(i++,"060b");
	RegisterCG(i++,"060c");
	RegisterCG(i++,"061");
	RegisterCG(i++,"062");
	RegisterCG(i++,"063");
	RegisterCG(i++,"096a");
	RegisterCG(i++,"096b");
	RegisterCG(i++,"097");
	RegisterCG(i++,"107");
	RegisterCG(i++,"117");
	RegisterCG(i++,"118");
	RegisterCG(i++,"065");
	RegisterCG(i++,"066");
	RegisterCG(i++,"067a");
	RegisterCG(i++,"067b");
	RegisterCG(i++,"067c");
	RegisterCG(i++,"068");
	RegisterCG(i++,"069");
	RegisterCG(i++,"070");
	RegisterCG(i++,"071");
	RegisterCG(i++,"098");
	RegisterCG(i++,"115");
	RegisterCG(i++,"073");
	RegisterCG(i++,"074");
	RegisterCG(i++,"075");
	RegisterCG(i++,"076");
	RegisterCG(i++,"077");
	RegisterCG(i++,"078");
	RegisterCG(i++,"100");
	RegisterCG(i++,"121a");
	RegisterCG(i++,"121b");
	RegisterCG(i++,"079a");
	RegisterCG(i++,"079b");
	RegisterCG(i++,"079c");
	RegisterCG(i++,"079d");
	RegisterCG(i++,"080");
	RegisterCG(i++,"081");
	RegisterCG(i++,"082");
	RegisterCG(i++,"101a");
	RegisterCG(i++,"101b");
	RegisterCG(i++,"085");
	RegisterCG(i++,"086a");
	RegisterCG(i++,"086b");
	RegisterCG(i++,"086c");
	RegisterCG(i++,"086d");
	RegisterCG(i++,"087");
	RegisterCG(i++,"088");
	RegisterCG(i++,"108");
	RegisterCG(i++,"109");
	RegisterCG(i++,"110");
	RegisterCG(i++,"111");

	// Load main menu
	main=new NIMain(this);
	AddWidget(main,VL_DIALOG);
	EventGameDialog(VD_TITLE);
}

Nocturnal::~Nocturnal(){
	// Delete selection graphics
	if(sselection[0]){
		SDL_FreeSurface(sselection[0]);
	}
	if(sselection[1]){
		SDL_FreeSurface(sselection[1]);
	}
	if(sselection[2]){
		SDL_FreeSurface(sselection[2]);
	}

	// Delete cached graphics
	if(background){
		SDL_FreeSurface(background);
	}
}

uString Nocturnal::GetImagename(uString Imagename){
	uString tmp=EDL_Searchname(Imagename);
	if(tmp=="ara")			return "mu132.ggd";
	else if(tmp=="07n")		return "mu007an.ggd";
	else if(tmp=="10n")		return "mu010an.ggd";
	else if(tmp=="2a1")		return "mu072a1.ggd";
	else if(tmp=="2a2")		return "mu072a2.ggd";
	else if(tmp=="2b1")		return "mu072b1.ggd";
	else if(tmp=="2b2")		return "mu072b2.ggd";
	else if(tmp=="2c1")		return "mu072c1.ggd";
	else if(tmp=="2c2")		return "mu072c2.ggd";
	else if(tmp=="2d1")		return "mu072d1.ggd";
	else if(tmp=="2d2")		return "mu072d2.ggd";
	else if(tmp=="4bd")		return "mu064bd.ggd";
	else if(tmp=="7a2")		return "mu037a.ggd";
	else if(tmp=="7b2")		return "mu037b.ggd";
	else if(tmp=="7c2")		return "mu037c.ggd";
	else if(tmp=="7d2")		return "mu037d.ggd";
	else if(tmp=="7e2")		return "mu037e.ggd";
	else if(tmp=="101")		return "mu129.ggd";
	else if(tmp=="6c2")		return "mu016c2.ggd";
	else if(tmp=="6d2")		return "mu016d2.ggd";
	else if(tmp=="005b")	return "mu130.ggd";
	else if(tmp=="mu058d")	return "mu058d.ggd";
	else if(tmp=="mu094a")	return "mu094a.ggd";
	else if(tmp=="mu094b")	return "mu113.ggd";
	else if(tmp.length()){
		// Append prefix
		uString ret="mu";

		// Pad indexed bitmaps
		if(tmp[tmp.length()-1].to_code()>='a'){
			if(tmp[tmp.length()-1].to_code()<='z'){
				ret+="0";
			}
		}

		// Build remainder
		ret+=tmp;
		ret+=".ggd";
		return ret;
	}
	else{
		return "";
	}
}

const uString Nocturnal::NativeID(){
	return "Nocturnal";
}

const uString Nocturnal::NativeName(){
	return "Nocturnal Illusion Renewal";
}

bool Nocturnal::PlayMusic(uString Name){
	// Generate name
	bool retval=false;
	char buffer[32]={0};
	int number=atoi(Name.c_str());
	if(number>0 && number<12){
		sprintf(buffer,"MUMU%02d.WAV",number);
		retval=EngineVN::QueueMusic(buffer);
	}
	else if(number>0){
		sprintf(buffer,"MUNEW%02d.WAV",number-11);
		retval=EngineVN::QueueMusic(buffer);
	}
	return retval;
}

void Nocturnal::EventSelect(int Selection){
	// Cache selection values
	if(menu_main_flag){
		// Unset submenu choice when main selection changes
		if(menu_main_cache!=Selection){
			menu_sub_cache=0;
		}
		menu_main_cache=Selection;
	}
	else{
		menu_sub_cache=Selection;
	}

	// Pass selection down to engine
	EngineWindy::EventSelect(Selection);
}

bool Nocturnal::LoadGraphics(uString Resource,Uint16 Index,Uint16 Type){
	LogTest("LOAD:%04x:%04x:%s",Index,Type,Resource.c_str());
	uString resource=GetImagename(Resource);
	SDL_Surface *surface=LoadImage(resource.c_str());
	if(surface){
		Uint8 r,g,b,a,c=0;
		EDL_GetPixel(surface,1,1,&r,&g,&b,&a);
		Uint8 tr,tg,tb,ta;

		// Some images are overlayed in the old version, but not in the
		// remake, so verify colorkey in other corners
		EDL_GetPixel(surface,1,surface->h-1,&tr,&tg,&tb,&ta);
		c=(tr==r && tg==g && tb==b && ta==a)?c+1:c;
		EDL_GetPixel(surface,surface->w-1,1,&tr,&tg,&tb,&ta);
		c=(tr==r && tg==g && tb==b && ta==a)?c+1:c;
		EDL_GetPixel(surface,surface->w-1,surface->h-1,&tr,&tg,&tb,&ta);
		c=(tr==r && tg==g && tb==b && ta==a)?c+1:c;
		bool transparent=(c>=2);
		if(transparent){
			Uint32 key=(r<<24)|(g<<16)|(b<<8)|0xFF;
			SDL_Surface *masked=EDL_ColorkeySurface(surface,key,0);
			if(masked){
				SDL_FreeSurface(surface);
				surface=masked;
			}
		}

		if(transparent){
			// Blit background and blend in transparent image
			if(background){
				w_bg->Blit(background);
			}
			w_bg->Blend(surface);
			SDL_FreeSurface(surface);
		}
		else{
			// Cache background and blit it to display
			if(background){
				SDL_FreeSurface(background);
			}
			background=surface;
			w_bg->Blit(surface);
		}
	}
	else{
		// Report missing image unless its the usual suspects
		uString res=EDL_Searchname(Resource);
		if(res!="90z" && res!="bk0" && res!="win" && res!="ttl"){
			LogError("Failed to load surface: %s",Resource.c_str());
		}
	}
	return surface;
}

void Nocturnal::FillGraphics(Uint16 Index){
	int w=NativeWidth();
	int h=NativeHeight();
	SDL_Surface *source=EDL_CreateSurface(w,h);
	EDL_SetBox(source,0,0,w,h,0,0,0,0xFF);

	if(background){
		SDL_FreeSurface(background);
	}
	background=source;
	w_bg->Fill(0x000000FF);
}

Widget *Nocturnal::LoadSelection(Stringlist Strings,
		Uint16 R1,Uint16 R2,Uint16 A1,Uint16 A2){
	// Clear text and set location as background until something is loaded
	NILocation *location=new NILocation(this);
	w_tv->ClearText();
	return location;
}

Widget *Nocturnal::LoadSelection(Stringlist Strings){
	// Graphics selected in EventTextMode
	if(iselection<0 || iselection>=3 || !sselection[iselection]){
		iselection=0;
	}
	if(iselection>=0 && iselection<3 && sselection[iselection]){
		SDL_Surface *selbitmap=sselection[iselection];
		Selection *seldialog=new Selection(this);
		seldialog->Move(40,40);
		seldialog->Resize(selbitmap->w,selbitmap->h);
		seldialog->SetAlignment(HA_CENTER,VA_CENTER);
		seldialog->Set(selbitmap,0,0);
		seldialog->SetText(&Strings);
		if(menu_main_flag){
			seldialog->SetFocusItem(menu_main_cache);
		}
		else{
			seldialog->SetFocusItem(menu_sub_cache);
		}
		return seldialog;
	}
	else{
		return 0;
	}
}

void Nocturnal::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_TITLE){
		DestroyLayer(VL_CHOICES);
		StopMusic();
		StopSound();
		main->SetVisible(true);
		PlayMusic("02");
	}
	else if(Dialog==VD_SAVE){
		StdSave *dlg=new StdSave(this);
		SDL_Surface *sback=LoadImage("omabg.ggd");
		if(sback){
			dlg->Blit(sback);
			SDL_FreeSurface(sback);
		}
		AddWidget(dlg,VL_DIALOG);
	}
	else if(Dialog==VD_LOAD){
		StdLoad *dlg=new StdLoad(this);
		SDL_Surface *sback=LoadImage("omabg.ggd");
		if(sback){
			dlg->Blit(sback);
			SDL_FreeSurface(sback);
		}
		AddWidget(dlg,VL_DIALOG);
	}
	else if(Dialog==VD_CREDITS){
		// Scroll staffroll
		SDL_Surface *surface=LoadImage("STAFROLL.GGD");
		if(surface){
			Sequences->RollPaddedVertical(surface);
			Sequences->LogoRandom(0,0,0);
			SDL_FreeSurface(surface);
		}

		// Fade in titlescreen
		surface=LoadImage("TITLE.GGD");
		if(surface){
			SDL_Rect dst={0,0,NativeWidth(),NativeHeight()};
			AddAnimation(new Fade(dst,surface,dst,5000));
			SDL_FreeSurface(surface);
		}
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

bool Nocturnal::EventSave(int Index){
	bool retval=EngineWindy::EventSave(Index);
	if(retval){
		Savegame *save=new Savegame(NativeID(),Index);
		save->Read();
		if(background){
			save->SaveSurface("screen-background",background);
		}
		save->Write();
	}
	return retval;
}

bool Nocturnal::EventLoad(int Index){
	bool retval=EngineWindy::EventLoad(Index);
	if(retval){
		// Force textview to normal textmode
		int x,y,w,h;
		x=15;
		y=NativeHeight()-w_tv->GetHeight()+8;
		w=610;
		h=84;
		w_tv->ClearText();
		w_tv->SetTextPosition(x,y,w,h);

		// Load mayclub specific data
		Savegame *load=new Savegame(NativeID(),Index);
		load->Read();
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-background",&tmps)){
			if(background){
				SDL_FreeSurface(background);
			}
			background=tmps;
		}
		delete load;

		// Make sure the mainscreen is hidden
		main->SetVisible(false);
	}
	return retval;
}

void Nocturnal::EventNew(){
	// Force textview to normal textmode
	int x,y,w,h;
	x=15;
	y=NativeHeight()-w_tv->GetHeight()+8;
	w=610;
	h=84;
	w_tv->ClearText();
	w_tv->SetTextPosition(x,y,w,h);

	// Preset default values
	EngineWindy::EventNew();
	SetValue(0x0127,2);			// Current floor
	SetValue(0x0126,4);			// Current room
	SetValue(0x012D,2);			// Previous floor
	SetValue(0x012E,4);			// Previous room

	// Load initialization code
	LoadWindyScript("OPEN.S",0x23B1);
	SetTransition();
}

void Nocturnal::EventBlit(Uint16 Opcode,Uint16 Index){
	if(Opcode==0x002F){
		if(background){
			w_bg->Blit(background);
		}
	}
}

void Nocturnal::EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag){
	// Check for valid textmodes
	int x=0,y=0,w=0,h=0;
	if(Mode==0x000C || Mode==0x000D){
		// Register menu type and prepare cached selection graphics
		menu_main_flag=(Mode==0x000C);
		if(Height>6){
			iselection=1;
		}
		else if(Width>9){
			iselection=2;
		}
		else{
			iselection=0;
		}
		LogDebug("Preparing %s menu: %dx%dx%dx%d (%d)",
				menu_main_flag?"main":"sub",
				X,Y,Width,Height,Flag);
	}
	else if(Mode==0x0002 || Mode==0x0003 || Mode==0x000B || Mode==0x000F){
		LogDebug("Preparing text: %dx%dx%dx%d (%d/%d)",
				X,Y,Width,Height,Mode,Flag);
		//if((X==0x0007 && Y==0x000A) ||
		//   (X==0x000A && Y==0x000A)){
		if(Y==0x000A){
			// Quotes for nocturnal illusion
			x=100;
			y=100;
			w=610;
			h=84;
		}
		else{
			// Fade quotes to black
			if(w_tv->GetTextPosition(&x,&y,&w,&h)){
				if(y<NativeHeight()/2){
					SDL_Rect dst={0,0,
						NativeWidth(),
						NativeHeight()-w_tv->GetHeight()};
					AddAnimation(new FadeBlack(dst,5000));
				}
			}

			// Default to normal mode
			x=15;
			y=NativeHeight()-w_tv->GetHeight()+8;
			w=610;
			h=84;
		}
	}
	else{
		LogError("Unknown textmode for nocturnal: %d(%d,%d)",Mode,X,Y);
	}

	// Change textmode if applicable
	if(x || y || w || h){
		w_tv->ClearText();
		w_tv->SetTextPosition(x,y,w,h);
	}
}


bool Nocturnal::EventExternal(Uint16 Address){
	// Override slow looping routines
	bool retval=false;
	if(Address==0x4150){
		// Skip graphical effects and load location menu
		LogDebug("Skipping legacy effect (0x4150)");
		Call(0x6B86);
		retval=true;
	}

	// We have no idea why the bgm opcode doesnt register, but
	// this hack should do the trick considering that we are
	// closing up the windy engine.
	static int ltrack=255;
	int ntrack=vars.GetUint16(0x0200);
	if(ntrack==0xFF){
		ltrack=ntrack;
	}
	else if(ntrack!=ltrack){
		int number=ntrack;
		if(ntrack==0)		number=1;
		else if(ntrack==1)	number=2;
		else if(ntrack==2)	number=3;
		else if(ntrack==4)	number=4;
		else if(ntrack==6)	number=11;
		else if(ntrack==7)	number=5;
		else if(ntrack==8)	number=6;
		else if(ntrack==9)	number=7;
		else if(ntrack==11)	number=8;
		else if(ntrack==13)	number=9;
		else if(ntrack==14)	number=10;
		PlayMusic(EDL_Format("%02d",number));
		ltrack=ntrack;
	}
	return retval;
}


