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

#include "mcalbum.h"
#include "mayclub.h"

MCAlbum::MCAlbum(Mayclub *Engine) : DialogBase(Engine,true){
	// Load main menu graphics
	SDL_Surface *salbum2=Engine->LoadImage("album2.bet");
	SDL_Surface *palbum1=Engine->LoadImage("album1.bet");
	SDL_Surface *malbum1=Engine->LoadImage("album1_.bet");
	SDL_Surface *pparts=Engine->LoadImage("albumparts.bet");
	SDL_Surface *mparts=Engine->LoadImage("albumparts_.bet");
	if(pparts && mparts && salbum2 && palbum1 && malbum1){
		// Fade in background graphics
		SDL_Rect rfull={0,0,salbum2->w,salbum2->h};
		Move(rfull);
		Engine->AddAnimation(new Fade(rfull,salbum2,rfull,1000));
		SDL_Surface *salbum1=EDL_MaskSurface(palbum1,malbum1,0);
		EDL_BlitSurface(salbum1,0,salbum2,0);
		Engine->AddAnimation(new Fade(rfull,salbum2,rfull,1000));
		Set(salbum2);

		// Create page buttons
		SDL_Rect rpage1d={60+(85*0),64,82,25};
		SDL_Rect rpage2d={60+(85*1),64,82,25};
		SDL_Rect rpage3d={60+(85*2),64,82,25};
		SDL_Rect rpage4d={60+(85*3),64,82,25};
		SDL_Rect rpage5d={60+(85*4),64,82,25};
		SDL_Rect rpage1sn={(85*0),328,82,25};
		SDL_Rect rpage1ss={(85*0),356,82,25};
		SDL_Rect rpage2sn={(85*1),328,82,25};
		SDL_Rect rpage2ss={(85*1),356,82,25};
		SDL_Rect rpage3sn={(85*2),328,82,25};
		SDL_Rect rpage3ss={(85*2),356,82,25};
		SDL_Rect rpage4sn={(85*3),328,82,25};
		SDL_Rect rpage4ss={(85*3),356,82,25};
		SDL_Rect rpage5sn={(85*4),328,82,25};
		SDL_Rect rpage5ss={(85*4),356,82,25};
		SDL_Surface *sparts=EDL_MaskSurface(pparts,mparts,0);
		w_pages[0]=new ValueButton(rpage1d);
		w_pages[0]->SetState(WS_NORMAL,sparts,&rpage1sn,sparts,&rpage1ss);
		w_pages[0]->SetState(WS_HOVER,sparts,&rpage1sn,sparts,&rpage1ss);
		w_pages[0]->SetState(WS_SELECT,sparts,&rpage1sn,sparts,&rpage1ss);
		w_pages[0]->ChangeState(WS_NORMAL);
		w_pages[1]=new ValueButton(rpage2d);
		w_pages[1]->SetState(WS_NORMAL,sparts,&rpage2sn,sparts,&rpage2ss);
		w_pages[1]->SetState(WS_HOVER,sparts,&rpage2sn,sparts,&rpage2ss);
		w_pages[1]->SetState(WS_SELECT,sparts,&rpage2sn,sparts,&rpage2ss);
		w_pages[1]->ChangeState(WS_NORMAL);
		w_pages[2]=new ValueButton(rpage3d);
		w_pages[2]->SetState(WS_NORMAL,sparts,&rpage3sn,sparts,&rpage3ss);
		w_pages[2]->SetState(WS_HOVER,sparts,&rpage3sn,sparts,&rpage3ss);
		w_pages[2]->SetState(WS_SELECT,sparts,&rpage3sn,sparts,&rpage3ss);
		w_pages[2]->ChangeState(WS_NORMAL);
		w_pages[3]=new ValueButton(rpage4d);
		w_pages[3]->SetState(WS_NORMAL,sparts,&rpage4sn,sparts,&rpage4ss);
		w_pages[3]->SetState(WS_HOVER,sparts,&rpage4sn,sparts,&rpage4ss);
		w_pages[3]->SetState(WS_SELECT,sparts,&rpage4sn,sparts,&rpage4ss);
		w_pages[3]->ChangeState(WS_NORMAL);
		w_pages[4]=new ValueButton(rpage5d);
		w_pages[4]->SetState(WS_NORMAL,sparts,&rpage5sn,sparts,&rpage5ss);
		w_pages[4]->SetState(WS_HOVER,sparts,&rpage5sn,sparts,&rpage5ss);
		w_pages[4]->SetState(WS_SELECT,sparts,&rpage5sn,sparts,&rpage5ss);
		w_pages[4]->ChangeState(WS_NORMAL);

		// Create modebutton
		SDL_Rect rmodedspd={540,260,100,56};
		SDL_Rect rmodebtnd={556,391,100,36};
		SDL_Rect rmodebtns={112,200,100,36};
		w_modedsp=new Widget(rmodedspd);
		w_modebtn=new BitmapButton(rmodebtnd);
		w_modebtn->SetState(WS_NORMAL,salbum2,&rmodebtnd);
		w_modebtn->SetState(WS_HOVER,sparts,&rmodebtns);
		w_modebtn->SetState(WS_SELECT,sparts,&rmodebtns);
		w_modebtn->ChangeState(WS_NORMAL);

		// Create exit button
		SDL_Rect rexitd={551,447,100,32};
		SDL_Rect rexits={112,120,100,32};
		w_exit=new BitmapButton(rexitd);
		w_exit->SetState(WS_NORMAL,salbum2,&rexitd);
		w_exit->SetState(WS_HOVER,sparts,&rexits);
		w_exit->SetState(WS_SELECT,sparts,&rexits);
		w_exit->ChangeState(WS_NORMAL);

		// Create sprites
		SDL_Surface *pmarker=Engine->LoadImage("akiho.bet");
		SDL_Surface *mmarker=Engine->LoadImage("akiho_.bet");
		SDL_Surface *smarker=EDL_MaskSurface(pmarker,mmarker,0);
		SDL_Rect rmarker={0,0,240/5,64};
		w_hover=new Sprite(rmarker);
		w_hover->AddFrame(smarker,&rmarker,100);
		rmarker.x+=rmarker.w;
		w_hover->AddFrame(smarker,&rmarker,100);
		rmarker.x+=rmarker.w;
		w_hover->AddFrame(smarker,&rmarker,100);
		rmarker.x+=rmarker.w;
		w_hover->AddFrame(smarker,&rmarker,100);
		rmarker.x+=rmarker.w;
		w_hover->AddFrame(smarker,&rmarker,100);
		w_hover->Move(rpage1d.x+20,rpage1d.y-rpage1d.h);

		// Create bgm widgets
		SDL_Rect rbgmd={550,58,27,27};
		SDL_Rect rbgms={0,0,rbgmd.w,rbgmd.h};
		for(int i=0;i<6;i++){
			w_bgm[i]=new ValueButton(rbgmd);
			w_bgm[i]->SetState(WS_NORMAL,salbum2,&rbgmd,sparts,&rbgms);
			w_bgm[i]->SetState(WS_HOVER,sparts,&rbgms,sparts,&rbgms);
			w_bgm[i]->SetState(WS_SELECT,sparts,&rbgms,sparts,&rbgms);
			w_bgm[i]->ChangeState(WS_NORMAL);
			AddWidget(w_bgm[i]);
			rbgms.x+=(rbgmd.w+1);
			rbgmd.y+=(rbgmd.h+5);
		}
		rbgmd.x+=44;
		rbgmd.y=58;
		rbgms.x=0;
		rbgms.y+=28;
		for(int i=6;i<11;i++){
			w_bgm[i]=new ValueButton(rbgmd);
			w_bgm[i]->SetState(WS_NORMAL,salbum2,&rbgmd,sparts,&rbgms);
			w_bgm[i]->SetState(WS_HOVER,sparts,&rbgms,sparts,&rbgms);
			w_bgm[i]->SetState(WS_SELECT,sparts,&rbgms,sparts,&rbgms);
			w_bgm[i]->ChangeState(WS_NORMAL);
			AddWidget(w_bgm[i]);
			rbgms.x+=(rbgmd.w+1);
			rbgmd.y+=(rbgmd.h+5);
		}


		// Create item widgets
		w_itembg=new Widget();
		w_itembg->Move(30,105);
		AddWidget(w_itembg);
		SDL_Rect ritemd={36,111,112,84};
		SDL_Rect ritems={0,116,112,100};
		for(int i=0;i<4;i++){
			ritemd.x=36;
			for(int j=0;j<4;j++){
				w_items[(i*4)+j]=new Hotspot(ritemd);
				w_items[(i*4)+j]->Blit(sparts,&ritems);
				ritemd.x+=(ritemd.w+4);
				AddWidget(w_items[(i*4)+j]);
			}
			ritemd.y+=(ritemd.h+4);
		}

		// Assign widgets
		AddWidget(w_pages[0]);
		AddWidget(w_pages[1]);
		AddWidget(w_pages[2]);
		AddWidget(w_pages[3]);
		AddWidget(w_pages[4]);
		AddWidget(w_modedsp);
		AddWidget(w_modebtn);
		AddWidget(w_exit);
		AddWidget(w_hover);

		// Free graphics
		SDL_FreeSurface(salbum2);
		SDL_FreeSurface(salbum1);
		SDL_FreeSurface(palbum1);
		SDL_FreeSurface(malbum1);
		SDL_FreeSurface(sparts);
		SDL_FreeSurface(pparts);
		SDL_FreeSurface(mparts);
		SDL_FreeSurface(smarker);
		SDL_FreeSurface(pmarker);
		SDL_FreeSurface(mmarker);

		// Load default page
		SetMode(AM_CG);
		SetPage(0);
	}
}

MCAlbum::~MCAlbum(){
	DestroyWidgets();
}

void MCAlbum::SetMode(ALBUM_MODE Mode){
	mode=Mode;
	SDL_Rect rmodedsps={112,61,100,55};
	if(mode==AM_STORY){
		rmodedsps.x=0;
	}
	SDL_Surface *pparts=((Mayclub*)engine)->LoadImage("albumparts.bet");
	SDL_Surface *mparts=((Mayclub*)engine)->LoadImage("albumparts_.bet");
	if(pparts && mparts){
		SDL_Surface *sparts=EDL_MaskSurface(pparts,mparts,0);
		w_modedsp->Blit(sparts,&rmodedsps);
		SDL_FreeSurface(sparts);
		SDL_FreeSurface(pparts);
		SDL_FreeSurface(mparts);
	}
}

void MCAlbum::SetPage(int Page){
	if(Page>=0){
		// Load background
		char name[32];
		if(mode==AM_CG){
			page=EDL_MIN(Page,4);
			sprintf(name,"P%d.bet",page+1);
			SDL_Surface *sbg=((Mayclub*)engine)->LoadImage(name);
			if(sbg){
				w_itembg->Set(sbg);
				SDL_FreeSurface(sbg);
			}

			// Configure paginators
			for(int i=0;i<5;i++){
				w_pages[i]->SetValue(false);
				w_pages[i]->SetVisible(true);
				w_pages[i]->SetHittable(true);
			}
			w_pages[page]->SetValue(true);

			// Configure hotspots
			for(int i=5;i<16;i++){
				w_items[i]->SetVisible(true);
			}
			for(int i=0;i<16;i++){
				bool bit=false;
				uString name;
				((Mayclub*)engine)->MapCG((Page*16)+i,&bit,&name);
				w_items[i]->SetAlpha(bit?0:255);
			}
		}
		if(mode==AM_STORY){
			page=EDL_MIN(Page,1);
			sprintf(name,"st_P%d.bet",page+1);
			SDL_Surface *sbg=((Mayclub*)engine)->LoadImage(name);
			if(sbg){
				w_itembg->Set(sbg);
				SDL_FreeSurface(sbg);
			}

			// Configure paginators
			for(int i=0;i<2;i++){
				w_pages[i]->SetValue(false);
				w_pages[i]->SetVisible(true);
				w_pages[i]->SetHittable(true);
			}
			for(int i=2;i<5;i++){
				w_pages[i]->SetValue(false);
				w_pages[i]->SetVisible(false);
				w_pages[i]->SetHittable(false);
			}
			w_pages[page]->SetValue(true);

			// Configure hotspots
			for(int i=0;i<16;i++){
				bool bit=false;
				uString name;
				((Mayclub*)engine)->MapScene((Page*16)+i,&bit,&name);
				w_items[i]->SetAlpha(bit?0:255);
			}
			if(Page==1){
				for(int i=5;i<16;i++){
					w_items[i]->SetVisible(false);
				}
			}
			else{
				for(int i=5;i<16;i++){
					w_items[i]->SetVisible(true);
				}
			}

		}
	}
}

bool MCAlbum::FocusEnter(Widget *Object){
	if(Object==w_pages[0] ||
	   Object==w_pages[1] ||
	   Object==w_pages[2] ||
	   Object==w_pages[3] ||
	   Object==w_pages[4]){
		w_hover->Move(Object->GetX()+20,
				Object->GetY()-Object->GetHeight());
		w_hover->SetVisible(true);
	}
	return false;
}

bool MCAlbum::FocusLeave(Widget *Object){
	w_hover->SetVisible(false);
	return false;
}

bool MCAlbum::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_exit){
		// Close dialog
		engine->DestroyWidget(this);
		engine->SetTransition();
		retval=true;
	}
	else if(Object==w_modebtn){
		// Change mode
		if(mode==AM_CG){
			SetMode(AM_STORY);
		}
		else{
			SetMode(AM_CG);
		}
		SetPage(page);
		retval=true;
	}
	else if(Object==w_pages[0] ||
			Object==w_pages[1] ||
			Object==w_pages[2] ||
			Object==w_pages[3] ||
			Object==w_pages[4]){
		// Change page
		if(Object==w_pages[0])	SetPage(0);
		if(Object==w_pages[1])	SetPage(1);
		if(Object==w_pages[2])	SetPage(2);
		if(Object==w_pages[3])	SetPage(3);
		if(Object==w_pages[4])	SetPage(4);
		w_hover->SetVisible(false);
		retval=true;
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
			Object==w_items[10] ||
			Object==w_items[11] ||
			Object==w_items[12] ||
			Object==w_items[13] ||
			Object==w_items[14] ||
			Object==w_items[15]){
		// Open item
		int index=-1;
		if(Object==w_items[0])	index=(page*16)+0;
		if(Object==w_items[1])	index=(page*16)+1;
		if(Object==w_items[2])	index=(page*16)+2;
		if(Object==w_items[3])	index=(page*16)+3;
		if(Object==w_items[4])	index=(page*16)+4;
		if(Object==w_items[5])	index=(page*16)+5;
		if(Object==w_items[6])	index=(page*16)+6;
		if(Object==w_items[7])	index=(page*16)+7;
		if(Object==w_items[8])	index=(page*16)+7;
		if(Object==w_items[9])	index=(page*16)+7;
		if(Object==w_items[10])	index=(page*16)+10;
		if(Object==w_items[11])	index=(page*16)+11;
		if(Object==w_items[12])	index=(page*16)+12;
		if(Object==w_items[13])	index=(page*16)+13;
		if(Object==w_items[14])	index=(page*16)+14;
		if(Object==w_items[15])	index=(page*16)+15;
		bool bit;
		uString name;
		SDL_Surface *cg;
		if(mode==AM_CG){
			if(((Mayclub*)engine)->MapCG(index,&bit,&name) && bit){
				if((cg=engine->LoadImage(name))){
					engine->AddWidget(new PopImage(engine,cg),VL_OVERLAY);
					SDL_FreeSurface(cg);
				}
			}
		}
		if(mode==AM_STORY){
			if(((Mayclub*)engine)->MapScene(index,&bit,&name) && bit){
				((Mayclub*)engine)->FillBackground(0x000000FF);
				if(((Mayclub*)engine)->LoadWindyScript(name)){
					engine->ShowLayer(VL_DIALOG,false);
				}
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
			Object==w_bgm[10]){
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
		if(bgm>=0 && bgm<11){
			for(int i=0;i<11;i++){
				w_bgm[i]->SetValue(false);
			}
			w_bgm[bgm]->SetValue(true);
			char bgmname[32];
			sprintf(bgmname,"%d",bgm+1);
			((Mayclub*)engine)->PlayMusic(bgmname);
		}
		retval=true;
	}

	return retval;
}



