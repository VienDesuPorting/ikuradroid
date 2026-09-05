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

#include "niremenisce.h"
#include "nocturnal.h"

NIRemenisce::NIRemenisce(Nocturnal *Engine) : DialogBase(Engine,true){
	// Load remenisce graphics
	mode=AM_SELECT;
	SDL_Surface *omabg=Engine->LoadImage("omabg.ggd");
	if(omabg){
		// Fade in background graphics
		SDL_Rect rfull={0,0,omabg->w,omabg->h};
		Move(rfull);
		Engine->AddAnimation(new Fade(rfull,omabg,rfull,1000));
		Set(omabg);
		SDL_FreeSurface(omabg);
	}

	// Create exit button
	SDL_Rect rexitd={551,447,100,32};
	w_back=new TextButton(rexitd,"Back");
	AddWidget(w_back);

	// Create menu
	SDL_Surface *sel=Engine->LoadImage("sel.ggd");
	if(sel){
		int itemheight=sel->h/4;
		SDL_Rect rsel={60,40+itemheight,sel->w-40,itemheight};
		w_menu[0]=new TextButton(rsel,"CG Mode");
		rsel.y+=itemheight;
		w_menu[1]=new TextButton(rsel,"Music Mode");
		rsel.y+=itemheight;
		AddWidget(w_menu[0]);
		AddWidget(w_menu[1]);
		SDL_FreeSurface(sel);
	}

	// Create bgm widgets
	SDL_Surface *bgm1=Engine->LoadImage("bgm1.ggd");
	SDL_Surface *bgm2=Engine->LoadImage("bgm2.ggd");
	if(bgm1 && bgm2){
		SDL_Rect rbgm={32,50,160,30};
		for(int i=0;i<6;i++){
			w_bgm[i]=new ValueButton(rbgm);
			w_bgm[i]->SetState(WS_NORMAL,bgm1,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_HOVER,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_SELECT,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->ChangeState(WS_NORMAL);
			w_bgm[i]->SetVisible(false);
			AddWidget(w_bgm[i]);
			rbgm.y+=(rbgm.h+20);
		}
		rbgm.x+=190;
		rbgm.y=50;
		for(int i=6;i<12;i++){
			w_bgm[i]=new ValueButton(rbgm);
			w_bgm[i]->SetState(WS_NORMAL,bgm1,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_HOVER,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_SELECT,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->ChangeState(WS_NORMAL);
			w_bgm[i]->SetVisible(false);
			AddWidget(w_bgm[i]);
			rbgm.y+=(rbgm.h+20);
		}
		rbgm.x+=190;
		rbgm.y=50;
		for(int i=12;i<18;i++){
			w_bgm[i]=new ValueButton(rbgm);
			w_bgm[i]->SetState(WS_NORMAL,bgm1,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_HOVER,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->SetState(WS_SELECT,bgm2,&rbgm,bgm2,&rbgm);
			w_bgm[i]->ChangeState(WS_NORMAL);
			w_bgm[i]->SetVisible(false);
			AddWidget(w_bgm[i]);
			rbgm.y+=(rbgm.h+20);
		}

		SDL_FreeSurface(bgm1);
		SDL_FreeSurface(bgm2);
	}

	// Create item widgets
	SDL_Surface *sitem1=Engine->LoadImage("grp1.ggd");
	SDL_Surface *sitem2=Engine->LoadImage("grp2.ggd");
	if(sitem1 && sitem2){
		SDL_Rect ritem={20,64,144,96};
		for(int i=0;i<3;i++){
			w_items[i]=new ValueButton(ritem);
			w_items[i]->SetState(WS_NORMAL,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_HOVER,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_SELECT,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->ChangeState(WS_NORMAL);
			w_items[i]->SetVisible(false);
			AddWidget(w_items[i]);
			ritem.y+=(ritem.h+15);
		}
		ritem.x+=152;
		ritem.y=64;
		for(int i=3;i<6;i++){
			w_items[i]=new ValueButton(ritem);
			w_items[i]->SetState(WS_NORMAL,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_HOVER,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_SELECT,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->ChangeState(WS_NORMAL);
			w_items[i]->SetVisible(false);
			AddWidget(w_items[i]);
			ritem.y+=(ritem.h+15);
		}
		ritem.x+=152;
		ritem.y=64;
		for(int i=6;i<9;i++){
			w_items[i]=new ValueButton(ritem);
			w_items[i]->SetState(WS_NORMAL,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_HOVER,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_SELECT,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->ChangeState(WS_NORMAL);
			w_items[i]->SetVisible(false);
			AddWidget(w_items[i]);
			ritem.y+=(ritem.h+15);
		}
		ritem.x+=152;
		ritem.y=64;
		for(int i=9;i<11;i++){
			w_items[i]=new ValueButton(ritem);
			w_items[i]->SetState(WS_NORMAL,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_HOVER,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->SetState(WS_SELECT,sitem2,&ritem,sitem1,&ritem);
			w_items[i]->ChangeState(WS_NORMAL);
			w_items[i]->SetVisible(false);
			AddWidget(w_items[i]);
			ritem.y+=(ritem.h+15);
		}

		// Free graphics
		SDL_FreeSurface(sitem1);
		SDL_FreeSurface(sitem2);
	}

	SetModeSelect();
}

NIRemenisce::~NIRemenisce(){
	DestroyWidgets();
}

void NIRemenisce::SetModeSelect(){
	// Load background
	SDL_Surface *omabg=engine->LoadImage("omabg.ggd");
	SDL_Surface *sel=engine->LoadImage("sel.ggd");
	if(omabg && sel){
		Blit(omabg);
		SDL_Rect dst={40,40,sel->w,sel->h};
		Blit(sel,0,&dst);
		SDL_FreeSurface(omabg);
		SDL_FreeSurface(sel);
	}

	// Hide items
	for(int i=0;i<11;i++){
		w_items[i]->SetVisible(false);
	}

	// Hide bgm controls
	for(int i=0;i<18;i++){
		w_bgm[i]->SetVisible(false);
	}

	// Show menu
	mode=AM_SELECT;
	for(int i=0;i<2;i++){
		w_menu[i]->SetVisible(true);
	}
}

void NIRemenisce::SetModeMusic(){
	// Load background
	SDL_Surface *bgm1=engine->LoadImage("bgm1.ggd");
	if(bgm1){
		Blit(bgm1);
		SDL_FreeSurface(bgm1);
	}

	// Hide menu
	for(int i=0;i<2;i++){
		w_menu[i]->SetVisible(false);
	}

	// Show bgm controls
	mode=AM_MUSIC;
	for(int i=0;i<18;i++){
		w_bgm[i]->SetVisible(true);
		w_bgm[i]->SetValue(false);
	}
}

void NIRemenisce::SetModeCG(){
	// Load background
	SDL_Surface *sitem1=engine->LoadImage("grp1.ggd");
	if(sitem1){
		Blit(sitem1);
		SDL_FreeSurface(sitem1);
	}

	// Hide menu
	for(int i=0;i<2;i++){
		w_menu[i]->SetVisible(false);
	}

	// Configure hotspots
	Nocturnal *niengine=(Nocturnal*)engine;
	mode=AM_CG;
	for(int i=0;i<11;i++){
		bool bit=false;
		bool result=false;
		uString name;
		int s=0,e=0;
		if(i==4){	s=0;	e=15;	}
		if(i==0){	s=15;	e=31;	}
		if(i==6){	s=31;	e=40;	}
		if(i==7){	s=40;	e=47;	}
		if(i==9){	s=47;	e=57;	}
		if(i==1){	s=57;	e=71;	}
		if(i==3){	s=72;	e=82;	}
		if(i==10){	s=82;	e=91;	}
		if(i==2){	s=91;	e=100;	}
		if(i==5){	s=100;	e=107;	}
		if(i==8){	s=107;	e=111;	}
		if(e>0){
			for(int j=s;j<e;j++){
				if(niengine->MapCG(j,&bit,&name)){
					result|=bit;
				}
			}
		}
		w_items[i]->SetValue(result);
		w_items[i]->SetVisible(true);
	}
}

bool NIRemenisce::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_back){
		if(mode==AM_SELECT){
			// Close dialog
			engine->DestroyWidget(this);
			engine->SetTransition();
			retval=true;
		}
		else{
			// Back to remenisce menu
			SetModeSelect();
		}
	}
	else if(Object==w_menu[0]){
		// CG Mode
		SetModeCG();
	}
	else if(Object==w_menu[1]){
		// Music Mode
		SetModeMusic();
	}
	else if(Object==w_items[0] ||
			Object==w_items[1] ||
			Object==w_items[2] ||
			Object==w_items[3] ||
			Object==w_items[4] ||
			Object==w_items[5] ||
			Object==w_items[6] ||
			Object==w_items[7] ||
			Object==w_items[8] ||
			Object==w_items[9] ||
			Object==w_items[10]){
		// Open item
		bool bit;
		uString name;
		SDL_Surface *cg;
		if(mode==AM_CG){
			// Extract unlocked images using indexes
			Nocturnal *niengine=(Nocturnal*)engine;
			PopImage *popimage=new PopImage(engine);
			int s=0,e=0;
			if(Object==w_items[4]){		s=0;	e=15;	}
			if(Object==w_items[0]){		s=15;	e=31;	}
			if(Object==w_items[6]){		s=31;	e=40;	}
			if(Object==w_items[7]){		s=40;	e=47;	}
			if(Object==w_items[9]){		s=47;	e=57;	}
			if(Object==w_items[1]){		s=57;	e=71;	}
			if(Object==w_items[3]){		s=72;	e=82;	}
			if(Object==w_items[10]){	s=82;	e=91;	}
			if(Object==w_items[2]){		s=91;	e=100;	}
			if(Object==w_items[5]){		s=100;	e=107;	}
			if(Object==w_items[8]){		s=107;	e=111;	}
			bool result=false;
			if(e>0){
				for(int j=s;j<e;j++){
					if(niengine->MapCG(j,&bit,&name) && bit){
						name=niengine->GetImagename(name);
						if((cg=niengine->LoadImage(name))){
							popimage->AddImage(cg);
							SDL_FreeSurface(cg);
							result=true;
						}
					}
				}
			}

			// Show the images
			if(result){
				niengine->AddWidget(popimage,VL_OVERLAY);
			}
			else{
				delete popimage;
			}
		}
		retval=true;
	}
	else if(Object==w_bgm[0] ||
			Object==w_bgm[1] ||
			Object==w_bgm[2] ||
			Object==w_bgm[3] ||
			Object==w_bgm[4] ||
			Object==w_bgm[5] ||
			Object==w_bgm[6] ||
			Object==w_bgm[7] ||
			Object==w_bgm[8] ||
			Object==w_bgm[9] ||
			Object==w_bgm[10] ||
			Object==w_bgm[11] ||
			Object==w_bgm[12] ||
			Object==w_bgm[13] ||
			Object==w_bgm[14] ||
			Object==w_bgm[15] ||
			Object==w_bgm[16] ||
			Object==w_bgm[17]){
		// Play bgm track
		int bgm=-1;
		if(Object==w_bgm[0])	bgm=0;
		if(Object==w_bgm[1])	bgm=1;
		if(Object==w_bgm[2])	bgm=2;
		if(Object==w_bgm[3])	bgm=3;
		if(Object==w_bgm[4])	bgm=4;
		if(Object==w_bgm[5])	bgm=5;
		if(Object==w_bgm[6])	bgm=6;
		if(Object==w_bgm[7])	bgm=7;
		if(Object==w_bgm[8])	bgm=8;
		if(Object==w_bgm[9])	bgm=9;
		if(Object==w_bgm[10])	bgm=10;
		if(Object==w_bgm[11])	bgm=11;
		if(Object==w_bgm[12])	bgm=12;
		if(Object==w_bgm[13])	bgm=13;
		if(Object==w_bgm[14])	bgm=14;
		if(Object==w_bgm[15])	bgm=15;
		if(Object==w_bgm[16])	bgm=16;
		if(Object==w_bgm[17])	bgm=17;
		if(bgm>=0 && bgm<18){
			for(int i=0;i<18;i++){
				w_bgm[i]->SetValue(false);
			}
			w_bgm[bgm]->SetValue(true);
			char bgmname[32];
			sprintf(bgmname,"%d",bgm+1);
			((Nocturnal*)engine)->PlayMusic(bgmname);
		}
		retval=true;
	}

	return retval;
}


