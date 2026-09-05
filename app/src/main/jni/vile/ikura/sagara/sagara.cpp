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

#include "sagara.h"

Sagara::Sagara(uString Path) : IkuraDecoder(640,480){
	// Load resources
	AddScripts(new ArchiveIkura(Path+"ISF"));
	AddImages(new ArchiveIkura(Path+"GGD"));
	AddVoices(new ArchiveIkura(Path+"VOICE"));
	AddSE(new ArchiveIkura(Path+"SE"));
	AddBGM(new ArchiveIkura(Path+"WMSC"));
	AddBGM(new ArchiveIkura(Path+"MIDI"));
	CreateTextview();

	// Load standard boot script
	RunScript("START.ISF");
}

const uString Sagara::NativeID(){
	return "Sagara";
}

const uString Sagara::NativeName(){
	return "The Sagara Family";
}

void Sagara::CreateTextview(){
	// Load textwindow graphic and prepare widget
	w_textview->SetTextPosition(20,70,600,70);
	SDL_Surface *tmpsurface=LoadImage("m_win.gg3");
	SDL_Surface *tmpbuttons=LoadImage("m_win_p.gg3");
	if(tmpsurface && tmpbuttons){
		// Set window graphics
		w_textview->Resize(tmpsurface->w,tmpsurface->h);
		w_textview->Set(tmpsurface);
		SDL_FreeSurface(tmpsurface);

		// Get size of individual button
		int bw=tmpbuttons->w/7;
		int bh=tmpbuttons->h/4;

		// Save button
		BitmapButton *w_save=new BitmapButton();
		w_save->Move(496,6);
		w_save->Resize(bw,bh);
		w_save->SetState(WS_NORMAL,tmpbuttons,0,bh*0,bw,bh);
		w_save->SetState(WS_HOVER,tmpbuttons,0,bh*1,bw,bh);
		w_save->SetState(WS_SELECT,tmpbuttons,0,bh*2,bw,bh);
		w_save->SetState(WS_DISABLE,tmpbuttons,0,bh*3,bw,bh);
		w_textview->SetSaveButton(w_save);

		// Load button
		BitmapButton *w_load=new BitmapButton();
		w_load->Move(496,6+bh);
		w_load->Resize(bw,bh);
		w_load->SetState(WS_NORMAL,tmpbuttons,bw,bh*0,bw,bh);
		w_load->SetState(WS_HOVER,tmpbuttons,bw,bh*1,bw,bh);
		w_load->SetState(WS_SELECT,tmpbuttons,bw,bh*2,bw,bh);
		w_load->SetState(WS_DISABLE,tmpbuttons,bw,bh*3,bw,bh);
		w_textview->SetLoadButton(w_load);

		// Skip checkbox
		ValueButton *w_skip=new ValueButton();
		SDL_Rect skipclear={bw*2,0,bw,bh};
		SDL_Rect skipset={bw*3,0,bw,bh};
		w_skip->Move(496+bw,6);
		w_skip->Resize(bw,bh);
		w_skip->SetState(WS_NORMAL,tmpbuttons,&skipclear,tmpbuttons,&skipset);
		skipset.y+=bh;
		skipclear.y+=bh;
		w_skip->SetState(WS_HOVER,tmpbuttons,&skipclear,tmpbuttons,&skipset);
		skipset.y+=bh;
		skipclear.y+=bh;
		w_skip->SetState(WS_SELECT,tmpbuttons,&skipclear,tmpbuttons,&skipset);
		skipset.y+=bh;
		skipclear.y+=bh;
		w_skip->SetState(WS_DISABLE,tmpbuttons,&skipclear,tmpbuttons,&skipset);
		w_textview->SetSkipButton(w_skip);

		// Autobutton
		BitmapButton *w_auto=new BitmapButton();
		w_auto->Move(496+bw,6+bh);
		w_auto->Resize(bw,bh);
		w_auto->SetState(WS_NORMAL,tmpbuttons,bw*4,bh*0,bw,bh);
		w_auto->SetState(WS_HOVER,tmpbuttons,bw*4,bh*1,bw,bh);
		w_auto->SetState(WS_SELECT,tmpbuttons,bw*4,bh*2,bw,bh);
		w_auto->SetState(WS_DISABLE,tmpbuttons,bw*4,bh*3,bw,bh);
		w_textview->SetAutoButton(w_auto);

		// Optionbutton
		BitmapButton *w_option=new BitmapButton();
		w_option->Move(496+bw*2,6);
		w_option->Resize(bw,bh);
		w_option->SetState(WS_NORMAL,tmpbuttons,bw*5,bh*0,bw,bh);
		w_option->SetState(WS_HOVER,tmpbuttons,bw*5,bh*1,bw,bh);
		w_option->SetState(WS_SELECT,tmpbuttons,bw*5,bh*2,bw,bh);
		w_option->SetState(WS_DISABLE,tmpbuttons,bw*5,bh*3,bw,bh);
		w_textview->SetOptionButton(w_option);

		// Backlog button
		BitmapButton *w_backlog=new BitmapButton();
		w_backlog->Move(496+bw*2,6+bh);
		w_backlog->Resize(bw,bh);
		w_backlog->SetState(WS_NORMAL,tmpbuttons,bw*6,bh*0,bw,bh);
		w_backlog->SetState(WS_HOVER,tmpbuttons,bw*6,bh*1,bw,bh);
		w_backlog->SetState(WS_SELECT,tmpbuttons,bw*6,bh*2,bw,bh);
		w_backlog->SetState(WS_DISABLE,tmpbuttons,bw*6,bh*3,bw,bh);
		w_textview->SetBackButton(w_backlog);

		SDL_FreeSurface(tmpbuttons);
	}
	else{
		LogError("Failed to load textview graphics");
	}
}

/*
void Sagara::EventShowSave(){
	// Set background
	SDL_Surface *bgsurface=LoadImage("save_b.gg3");
	SDL_Surface *fgsurface=LoadImage("sl_p.gg3");
	if(bgsurface && fgsurface){
		// Set background
		SetDialogGraphics(IW_BACKGROUND,bgsurface);

		// Create loadbutton
		SDL_Rect normal={0,0,32,24};
		SDL_Rect hover={0,normal.h*1,normal.w,normal.h};
		SDL_Rect select={0,normal.h*2,normal.w,normal.h};
		SDL_Rect dst={448,8,normal.w,normal.h};
		for(int i=IW_SAVELOAD_PAGE_0;i<=IW_SAVELOAD_PAGE_4;i++){
			SetDialogButton((IKURA_WIDGET)i,fgsurface,
					dst,normal,hover,select,normal);
			dst.x+=normal.w;
			normal.x+=normal.w;
			hover.x+=normal.w;
			select.x+=normal.w;
		}

		// Create exitbutton
		normal.w=100;
		hover.w=normal.w;
		select.w=normal.w;
		dst.x=510;
		dst.y=448;
		dst.w=normal.w;
		SetDialogButton(IW_SAVELOAD_EXIT,fgsurface,
				dst,normal,hover,select,normal);

		// Create slabs
		SDL_Rect bgslab={32,41,280,80};
		SDL_Rect fgslab={0,184,280,80};
		SDL_Rect si={10,10,86,60};
		SDL_Rect st={110,10,160,60};
		dst.x=bgslab.x;
		dst.y=bgslab.y;
		dst.w=bgslab.w;
		dst.h=bgslab.h;
		uString savename;
		for(int i=IW_SAVELOAD_SLAB_0;i<IW_SAVELOAD_SLAB_5;i++){
			savename=Savegame::Savename("",NativeID(),i-IW_SAVELOAD_SLAB_0);
			SetDialogSlab((IKURA_WIDGET)i,bgsurface,fgsurface,
					dst,bgslab,fgslab,si,st,savename);
			dst.y+=bgslab.h;
		}
		dst.x+=bgslab.w+16;
		dst.y=bgslab.y;
		for(int i=IW_SAVELOAD_SLAB_5;i<=IW_SAVELOAD_SLAB_9;i++){
			savename=Savegame::Savename("",NativeID(),i-IW_SAVELOAD_SLAB_0);
			SetDialogSlab((IKURA_WIDGET)i,bgsurface,fgsurface,
					dst,bgslab,fgslab,si,st,savename);
			dst.y+=bgslab.h;
		}

		// Free surface
		SDL_FreeSurface(bgsurface);
		SDL_FreeSurface(fgsurface);
	}
}

void Sagara::EventShowLoad(){
	SDL_Surface *bgsurface=LoadImage("load_b.gg3");
	SDL_Surface *fgsurface=LoadImage("sl_p.gg3");
	if(bgsurface && fgsurface){
		// Set background
		SetDialogGraphics(IW_BACKGROUND,bgsurface);

		// Create loadbutton
		SDL_Rect normal={0,0,32,24};
		SDL_Rect hover={0,normal.h*1,normal.w,normal.h};
		SDL_Rect select={0,normal.h*2,normal.w,normal.h};
		SDL_Rect dst={448,8,normal.w,normal.h};
		for(int i=IW_SAVELOAD_PAGE_0;i<=IW_SAVELOAD_PAGE_4;i++){
			SetDialogButton((IKURA_WIDGET)i,fgsurface,
					dst,normal,hover,select,normal);
			dst.x+=normal.w;
			normal.x+=normal.w;
			hover.x+=normal.w;
			select.x+=normal.w;
		}

		// Create exitbutton
		normal.w=100;
		hover.w=normal.w;
		select.w=normal.w;
		dst.x=510;
		dst.y=448;
		dst.w=normal.w;
		SetDialogButton(IW_SAVELOAD_EXIT,fgsurface,
				dst,normal,hover,select,normal);

		// Create slabs
		SDL_Rect bgslab={32,41,280,80};
		SDL_Rect fgslab={0,184,280,80};
		SDL_Rect si={10,10,86,60};
		SDL_Rect st={110,10,160,60};
		dst.x=bgslab.x;
		dst.y=bgslab.y;
		dst.w=bgslab.w;
		dst.h=bgslab.h;
		uString savename;
		for(int i=IW_SAVELOAD_SLAB_0;i<IW_SAVELOAD_SLAB_5;i++){
			savename=Savegame::Savename("",NativeID(),i-IW_SAVELOAD_SLAB_0);
			SetDialogSlab((IKURA_WIDGET)i,bgsurface,fgsurface,
					dst,bgslab,fgslab,si,st,savename);
			dst.y+=bgslab.h;
		}
		dst.x+=bgslab.w+16;
		dst.y=bgslab.y;
		for(int i=IW_SAVELOAD_SLAB_5;i<=IW_SAVELOAD_SLAB_9;i++){
			savename=Savegame::Savename("",NativeID(),i-IW_SAVELOAD_SLAB_0);
			SetDialogSlab((IKURA_WIDGET)i,bgsurface,fgsurface,
					dst,bgslab,fgslab,si,st,savename);
			dst.y+=bgslab.h;
		}

		// Free surface
		SDL_FreeSurface(bgsurface);
		SDL_FreeSurface(fgsurface);
	}
}

void Sagara::EventShowOptions(){
	// Set option background
	SDL_Surface *tmpsurface=LoadImage("opt_b.gg1");
	if(tmpsurface){
		SetDialogGraphics(IW_BACKGROUND,tmpsurface);
		SDL_FreeSurface(tmpsurface);
	}

	// Map out the options for sagara family
	tmpsurface=LoadImage("opt_p.gg3");
	if(tmpsurface){
		// Map sliders
		SDL_Rect sfr={196,242,200,12};
		SDL_Rect ser={196,258,200,12};
		SDL_Rect destr={360,160,sfr.w,sfr.h};
		SetDialogSlider(IW_OPTION_SE_VOLUME,tmpsurface,destr,sfr,ser);
		destr.y=212;
		SetDialogSlider(IW_OPTION_BGM_VOLUME,tmpsurface,destr,sfr,ser);
		destr.y=267;
		SetDialogSlider(IW_OPTION_VOICE_VOLUME,tmpsurface,destr,sfr,ser);
	
		// Map buttons
		SDL_Rect nr={0,240,60,60};
		SDL_Rect hr={0,nr.y+(nr.h*1),nr.w,nr.h};
		SDL_Rect sr={0,nr.y+(nr.h*2),nr.w,nr.h};
		SDL_Rect dr={0,nr.y+(nr.h*3),nr.w,nr.h};
		destr.x=84;
		destr.y=60;
		destr.w=nr.w;
		destr.h=nr.h;
		SetDialogButton(IW_OPTION_HELP,tmpsurface,destr,nr,hr,sr,dr);
		nr.x+=nr.w;
		hr.x+=nr.w;
		sr.x+=nr.w;
		dr.x+=nr.w;
		destr.x+=64;
		SetDialogButton(IW_OPTION_TITLE,tmpsurface,destr,nr,hr,sr,dr);
		nr.x+=nr.w;
		hr.x+=nr.w;
		sr.x+=nr.w;
		dr.x+=nr.w;
		destr.x+=64;
		SetDialogButton(IW_OPTION_QUIT,tmpsurface,destr,nr,hr,sr,dr);
		nr.x+=nr.w;
		nr.y=305;
		nr.w=48;
		nr.h=28;
		hr.x=sr.x=dr.x=nr.x;
		hr.y=nr.y+(nr.h*1);
		sr.y=nr.y+(nr.h*2);
		dr.y=nr.y+(nr.h*3);
		hr.w=sr.w=dr.w=nr.w;
		hr.h=sr.h=dr.h=nr.h;
		destr.x=420;
		destr.y=127;
		destr.w=nr.w;
		destr.h=nr.h;
		SetDialogButton(IW_OPTION_SE_TEST,tmpsurface,destr,nr,hr,sr,dr);
		destr.y=233;
		SetDialogButton(IW_OPTION_VOICE_TEST,tmpsurface,destr,nr,hr,sr,dr);
		nr.x+=nr.w;
		nr.y=305;
		nr.w=104;
		nr.h=40;
		hr.x=sr.x=dr.x=nr.x;
		hr.y=nr.y+(nr.h*1);
		sr.y=nr.y+(nr.h*2);
		dr.y=nr.y+(nr.h*3);
		hr.w=sr.w=dr.w=nr.w;
		hr.h=sr.h=dr.h=nr.h;
		destr.x=96;
		destr.y=371;
		destr.w=nr.w;
		destr.h=nr.h;
		SetDialogButton(IW_OPTION_OK,tmpsurface,destr,nr,hr,sr,dr);
		nr.x+=nr.w;
		hr.x=sr.x=dr.x=nr.x;
		destr.x=204;
		SetDialogButton(IW_OPTION_CANCEL,tmpsurface,destr,nr,hr,sr,dr);


		// Map sound settings checkboxes
		SDL_Rect nur={0,0,420/6,80/4};
		SDL_Rect nsr={nur.w,0,nur.w,nur.h};
		SDL_Rect hur={0,nur.h,nur.w,nur.h};
		SDL_Rect hsr={nur.w,nur.h,nur.w,nur.h};
		SDL_Rect sur={0,nur.h*2,nur.w,nur.h};
		SDL_Rect ssr={nur.w,nur.h*2,nur.w,nur.h};
		SDL_Rect dur={0,nur.h*3,nur.w,nur.h};
		SDL_Rect dsr={nur.w,nur.h*3,nur.w,nur.h};
		destr.x=350;
		destr.y=125;
		destr.w=nur.w;
		destr.h=nur.h;
		SetDialogValue(IW_OPTION_SE_ENABLED,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		nur.x+=nur.w*2;
		nsr.x+=nur.w*2;
		hur.x+=nur.w*2;
		hsr.x+=nur.w*2;
		sur.x+=nur.w*2;
		ssr.x+=nur.w*2;
		dur.x+=nur.w*2;
		dsr.x+=nur.w*2;
		destr.y+=55;
		SetDialogValue(IW_OPTION_BGM_ENABLED,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		nur.x+=nur.w*2;
		nsr.x+=nur.w*2;
		hur.x+=nur.w*2;
		hsr.x+=nur.w*2;
		sur.x+=nur.w*2;
		ssr.x+=nur.w*2;
		dur.x+=nur.w*2;
		dsr.x+=nur.w*2;
		destr.y+=50;
		SetDialogValue(IW_OPTION_VOICE_ENABLED,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);

		// Map voice characters
		nur.x=0;
		nur.y=80;
		nur.w=66;
		nur.h=20;
		hur.x=sur.x=dur.x=nur.x;
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		nsr.y=nur.y;
		hur.y=hsr.y=nur.y+(hur.h*1);
		sur.y=ssr.y=nur.y+(hur.h*2);
		dur.y=dsr.y=nur.y+(hur.h*3);;
		nsr.w=hur.w=hsr.w=sur.w=ssr.w=dur.w=dsr.w=nur.w;
		nsr.h=hur.h=hsr.h=sur.h=ssr.h=dur.h=dsr.h=nur.h;
		destr.x=360;
		destr.y=283;
		destr.w=nur.w;
		destr.h=nur.h;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_0,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		hur.x=sur.x=dur.x=nur.x=nur.x+(nur.w*2);
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		destr.x+=destr.w+8;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_1,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		hur.x=sur.x=dur.x=nur.x=nur.x+(nur.w*2);
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		destr.x+=destr.w+8;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_2,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		hur.x=sur.x=dur.x=nur.x=0;
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		nsr.y=nur.y=(nur.y+(nur.h*4));
		hsr.y=hur.y=hur.y+(hur.h*4);
		ssr.y=sur.y=sur.y+(sur.h*4);
		dsr.y=dur.y=dur.y+(dur.h*4);
		destr.x=360;
		destr.y=308;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_3,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		hur.x=sur.x=dur.x=nur.x=nur.x+(nur.w*2);
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		destr.x+=destr.w+8;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_4,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		hur.x=sur.x=dur.x=nur.x=nur.x+(nur.w*2);
		nsr.x=hsr.x=ssr.x=dsr.x=nur.x+nur.w;
		destr.x+=destr.w+8;
		SetDialogValue(IW_OPTION_VOICE_CHARACTER_5,
				tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);

		// Map buttons as checkboxes
		nur.x=420;
		nur.y=0;
		nur.w=116;
		nur.h=28;
		nsr.y=nur.h*2;
		hur.y=nur.h*1;
		hsr.y=nur.h*2;
		sur.y=ssr.y=nur.h*2;
		dur.y=dsr.y=nur.h*3;
		dur.x=dsr.x=sur.x=ssr.x=hur.x=hsr.x=nsr.x=nur.x;
		dur.w=dsr.w=sur.w=ssr.w=hur.w=hsr.w=nsr.w=nur.w;
		dur.h=dsr.h=sur.h=ssr.h=hur.h=hsr.h=nsr.h=nur.h;
		destr.x=88;
		destr.y=192;
		destr.w=nur.w;
		destr.h=nur.h;
		SetDialogValue(IW_OPTION_SCREEN_FULLSIZE,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		nur.y+=nur.h*4;
		nsr.y+=nur.h*4;
		hur.y+=nur.h*4;
		hsr.y+=nur.h*4;
		sur.y+=nur.h*4;
		ssr.y+=nur.h*4;
		dur.y+=nur.h*4;
		dsr.y+=nur.h*4;
		destr.x=208;
		SetDialogValue(IW_OPTION_SCREEN_WINDOW,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		nur.x=436;
		nur.y=304;
		nur.w=76;
		nur.h=28;
		nsr.y=nur.y+(nur.h*2);
		hur.y=nur.y+(nur.h*1);
		hsr.y=nur.y+(nur.h*2);
		sur.y=ssr.y=nur.y+(nur.h*2);
		dur.y=dsr.y=nur.y+(nur.h*3);
		dur.x=dsr.x=sur.x=ssr.x=hur.x=hsr.x=nsr.x=nur.x;
		dur.w=dsr.w=sur.w=ssr.w=hur.w=hsr.w=nsr.w=nur.w;
		dur.h=dsr.h=sur.h=ssr.h=hur.h=hsr.h=nsr.h=nur.h;
		destr.x=168;
		destr.y=256;
		destr.w=nur.w;
		destr.h=nur.h;
		SetDialogValue(IW_OPTION_SKIP_SKIPALL,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);
		dur.x=dsr.x=sur.x=ssr.x=hur.x=hsr.x=nsr.x=nur.x=nur.x+nur.w;
		destr.x=248;
		SetDialogValue(IW_OPTION_SKIP_READONLY,tmpsurface,destr,
				nur,nsr,hur,hsr,sur,ssr,dur,dsr);

		SDL_FreeSurface(tmpsurface);
	}
}
*/


