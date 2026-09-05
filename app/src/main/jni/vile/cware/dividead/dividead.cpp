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

#include "dividead.h"

DiviDead::DiviDead(uString Path) : EngineCWare(640,480) {
	// Add resources
	AddBGM(new ArchiveFiles(Path+"mid/*.mid"));
	AddVoices(new ArchiveCWareDL1(Path+"wv.dl1"));
	AddSE(new ArchiveCWareDL1(Path+"wv.dl1"));
	AddScripts(new ArchiveCWareDL1(Path+"sg.dl1"));
	AddImages(new ArchiveFiles(Path+"m_a1.bmp"));
	AddImages(new ArchiveFiles(Path+"m_a2.bmp"));
	AddImages(new ArchiveFiles(Path+"m_b1.bmp"));
	AddImages(new ArchiveFiles(Path+"m_b2.bmp"));
	AddImages(new ArchiveCWareDL1(Path+"sg.dl1"));
	AddImages(new ArchiveCWareDL1(Path+"sg.dl1"));
	AddVideo(new ArchiveFiles(Path+"cs_rogo.avi"));
	AddVideo(new ArchiveFiles(Path+"open.avi"));

	// Add menu interface to background
	main=0;
	extra=0;
	divigui=new DiviGUI(this);
	AddWidget(divigui,VL_HOTSPOTS);

	// Configure textview
	textview->SetTextPosition(0,0,440,70);
	textview->MoveDialog(100,396);
	textview->Resize(440,70);

	// Configure textview
	//selection->Move(100,396);
	//selection->Resize(440,70);
	selection->Resize(640/3,480/3);
	selection->Move(640/3,480/3);
	selection->SetAlignment(HA_CENTER,VA_CENTER);

	// Map CG resources
	int i=0;
	RegisterCG(i++,"i_00");
	RegisterCG(i++,"i_01");
	RegisterCG(i++,"i_02");
	RegisterCG(i++,"i_03");
	RegisterCG(i++,"i_04");
	RegisterCG(i++,"i_06");
	RegisterCG(i++,"i_07");
	RegisterCG(i++,"i_08");
	RegisterCG(i++,"i_09");
	RegisterCG(i++,"i_10");
	RegisterCG(i++,"i_11");
	RegisterCG(i++,"i_12");
	RegisterCG(i++,"i_13");
	RegisterCG(i++,"i_14");
	RegisterCG(i++,"i_15");
	RegisterCG(i++,"i_16");
	RegisterCG(i++,"i_17");
	RegisterCG(i++,"i_18");
	RegisterCG(i++,"i_19");
	RegisterCG(i++,"i_20");
	RegisterCG(i++,"i_21");
	RegisterCG(i++,"i_22");
	RegisterCG(i++,"i_23");
	RegisterCG(i++,"i_24");
	RegisterCG(i++,"i_25");
	RegisterCG(i++,"i_26");
	RegisterCG(i++,"i_27");
	RegisterCG(i++,"i_28");
	RegisterCG(i++,"i_29");
	RegisterCG(i++,"i_30");
	RegisterCG(i++,"i_31");
	RegisterCG(i++,"i_32");
	RegisterCG(i++,"i_33");
	RegisterCG(i++,"i_35");
	RegisterCG(i++,"i_36");
	RegisterCG(i++,"i_37");
	RegisterCG(i++,"i_38");
	RegisterCG(i++,"i_39");
	RegisterCG(i++,"i_40");
	RegisterCG(i++,"i_41");
	RegisterCG(i++,"i_42");
	RegisterCG(i++,"i_43");
	RegisterCG(i++,"i_44");
	RegisterCG(i++,"i_45");
	RegisterCG(i++,"i_46");
	RegisterCG(i++,"i_47");
	RegisterCG(i++,"i_48");
	RegisterCG(i++,"i_49");
	RegisterCG(i++,"i_50");
	RegisterCG(i++,"i_51");
	RegisterCG(i++,"i_52");
	RegisterCG(i++,"i_53");
	RegisterCG(i++,"i_54");
	RegisterCG(i++,"i_55");
	RegisterCG(i++,"i_56");
	RegisterCG(i++,"i_57");
	RegisterCG(i++,"i_58");
	RegisterCG(i++,"i_59");
	RegisterCG(i++,"i_60");
	RegisterCG(i++,"i_61");
	RegisterCG(i++,"i_62");
	RegisterCG(i++,"i_63");
	RegisterCG(i++,"i_64");
	RegisterCG(i++,"i_65");
	RegisterCG(i++,"i_66");
	RegisterCG(i++,"i_67");
	RegisterCG(i++,"i_68");
	RegisterCG(i++,"i_69");
	RegisterCG(i++,"i_70");
	RegisterCG(i++,"i_71");
	RegisterCG(i++,"i_72");
	RegisterCG(i++,"i_73");
	RegisterCG(i++,"i_74");
	RegisterCG(i++,"i_75");
	RegisterCG(i++,"i_76");
	RegisterCG(i++,"i_77");
	RegisterCG(i++,"i_78");
	RegisterCG(i++,"i_79");
	RegisterCG(i++,"i_80");
	RegisterCG(i++,"i_81");
	RegisterCG(i++,"i_82");
	RegisterCG(i++,"i_83");
	RegisterCG(i++,"i_84");
	RegisterCG(i++,"i_85");
	RegisterCG(i++,"i_86");
	RegisterCG(i++,"i_88");
	RegisterCG(i++,"i_89");
	RegisterCG(i++,"i_90");
	RegisterCG(i++,"i_91");
	RegisterCG(i++,"i_92");
	RegisterCG(i++,"i_93");
	RegisterCG(i++,"i_95");
	RegisterCG(i++,"i_96");
	RegisterCG(i++,"i_97");
	RegisterCG(i++,"i_98");
	RegisterCG(i++,"i_99");
	RegisterCG(i++,"i_100");
	RegisterCG(i++,"i_101");
	RegisterCG(i++,"h_a0");
	RegisterCG(i++,"h_b0");
	RegisterCG(i++,"h_c0");
	RegisterCG(i++,"h_d0");
	RegisterCG(i++,"h_e0");
	RegisterCG(i++,"h_f0");
	RegisterCG(i++,"h_g0");
	RegisterCG(i++,"h_h0");

	// Load videos
	SDL_Rect client={0,0,640,480};
	AddAnimation(new FadeBlack(client,1000));
	QueueVideo("cs_rogo.avi");
	QueueVideo("open.avi");

	// Show main menu
	EventGameDialog(VD_TITLE);
}

/*! \brief Toggles main menu visibilty
 *  \return True if main menu was shown
 */
bool DiviDead::ToggleMainmenu(){
	bool retval=false;
	if(extra){
		DestroyWidget(extra);
		extra=0;
	}
	if(main){
		DestroyWidget(main);
		main=0;
	}
	else{
		SDL_Rect dummy={0,0,0,0};
		main=new DiviMenu(this);
		main->SetText("NEW",dummy,DM_NEW);
		if(Running()){
			main->SetText("SAVE",dummy,DM_SAVE);
		}
		main->SetText("LOAD",dummy,DM_LOAD);
		main->SetText("CONFIG",dummy,DM_CONFIG);
		main->SetText("EXIT",dummy,DM_EXIT);
		main->Generate();
		AddWidget(main,VL_EXTRAS);
		retval=true;
	}
	return retval;
}

bool DiviDead::ToggleExtramenu(){
	bool retval=false;
	if(main){
		DestroyWidget(main);
		main=0;
	}
	if(extra){
		DestroyWidget(extra);
		extra=0;
	}
	else{
		SDL_Rect dummy={0,0,0,0};
		extra=new DiviMenu(this);
		extra->SetText("EXTRAS",dummy,DM_EXTRAS);
		extra->SetText("SCREENSHOT",dummy,DM_SCREENSHOT);
		extra->Generate();
		AddWidget(extra,VL_EXTRAS);
		retval=true;
	}
	return retval;
}

void DiviDead::ClearMenu(){
	if(main){
		DestroyWidget(main);
		main=0;
	}
	if(extra){
		DestroyWidget(extra);
		extra=0;
	}
	divigui->Clear();
}

const uString DiviDead::NativeID(){
	return "Divi";
}

const uString DiviDead::NativeName(){
	return "DiviDead";
}

bool DiviDead::EventLoad(int Index){
	DestroyLayer(VL_OVERLAY);
	DestroyLayer(VL_EXTRAS);
	ClearMenu();
	return EngineCWare::EventLoad(Index);
}

bool DiviDead::EventBackgroundMouseRightUp(int X,int Y){
	DestroyLayer(VL_OVERLAY);
	DestroyLayer(VL_EXTRAS);
	ClearMenu();
	Resume();
	return true;
}

void DiviDead::EventNew(){
	DestroyLayer(VL_OVERLAY);
	DestroyLayer(VL_EXTRAS);
	ClearMenu();
	LoadCWareScript("aastart");
}

void DiviDead::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_EXTRAS){
		// Stop game features
		EngineCWare::EventGameDialog(Dialog);
		AddWidget(new DiviCG(this),VL_EXTRAS);
	}
	else if(Dialog==VD_TITLE){
		// Clear up old rubble
		StopMusic();
		StopSound();
		textview->ClearText();
		Stop();

		// Show main screen graphics
		SDL_Surface *title=LoadImage("title");
		SDL_Surface *frame=LoadImage("waku_a1");
		if(title && frame){
			display->Blit(frame);
			display->Blit(title,0,&background);
			SDL_FreeSurface(frame);
			SDL_FreeSurface(title);
		}

		// Load music
		QueueMusic("opening.avi");
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}


