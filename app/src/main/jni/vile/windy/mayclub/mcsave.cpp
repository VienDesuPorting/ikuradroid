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

#include "mcsave.h"
#include "mayclub.h"

MCSave::MCSave(Mayclub *Engine) : DialogBase(Engine,true){
	// Save main menu graphics
	SDL_Surface *sback=Engine->LoadImage("save.bet");
	SDL_Surface *smarker=Engine->LoadImage("marker.bet");
	SDL_Surface *mmarker=Engine->LoadImage("marker_.bet");
	SDL_Surface *skeywait=Engine->LoadImage("keywait.bet");
	SDL_Surface *mkeywait=Engine->LoadImage("keywait_.bet");

	if(sback && smarker && mmarker && skeywait && mkeywait){
		// Fade in background graphics
		SDL_Rect rfull={0,0,sback->w,sback->h};
		Engine->AddAnimation(new Fade(rfull,sback,rfull,1000));
		Move(rfull);
		Set(sback);

		// Create button backgrounds
		SDL_Rect rpage1d={109+(85*0),84,82,25};
		SDL_Rect rpage2d={109+(85*1),84,82,25};
		SDL_Rect rpage3d={109+(85*2),84,82,25};
		SDL_Rect rpage4d={109+(85*3),84,82,25};
		SDL_Rect rpage5d={109+(85*4),84,82,25};
		SDL_Rect rexitd={263,444,116,36};
		w_pages[0]=new Hotspot(rpage1d);
		w_pages[1]=new Hotspot(rpage2d);
		w_pages[2]=new Hotspot(rpage3d);
		w_pages[3]=new Hotspot(rpage4d);
		w_pages[4]=new Hotspot(rpage5d);
		w_exit=new Hotspot(rexitd);

		// Create sprites
		SDL_Rect rmarker={10,10,28,28};
		w_page=new Sprite(rmarker);
		SDL_Surface *marker=EDL_MaskSurface(smarker,mmarker,0);
		EDL_SETRECT(rmarker,0,0,28,28);
		w_page->AddFrame(marker,&rmarker,100);
		rmarker.x+=28;
		w_page->AddFrame(marker,&rmarker,100);
		rmarker.x+=28;
		w_page->AddFrame(marker,&rmarker,100);
		rmarker.x+=28;
		w_page->AddFrame(marker,&rmarker,100);
		rmarker.x+=28;
		w_page->AddFrame(marker,&rmarker,100);
		SDL_Rect rkeywait={10,10,32,32};
		w_hover=new Sprite(rkeywait);
		SDL_Surface *keywait=EDL_MaskSurface(skeywait,mkeywait,0);
		EDL_SETRECT(rkeywait,0,0,32,32);
		w_hover->AddFrame(keywait,&rkeywait,100);
		rkeywait.x+=32;
		w_hover->AddFrame(keywait,&rkeywait,100);
		rkeywait.x+=32;
		w_hover->AddFrame(keywait,&rkeywait,100);
		rkeywait.x+=32;
		w_hover->AddFrame(keywait,&rkeywait,100);

		// Set default sprite positions and visibility
		w_hover->SetVisible(false);
		w_hover->Move(rpage1d.x+20,rpage1d.y-rpage1d.h);
		w_page->Move(rpage1d.x+20,rpage1d.y-rpage1d.h);

		// Assign widgets
		AddWidget(w_pages[0]);
		AddWidget(w_pages[1]);
		AddWidget(w_pages[2]);
		AddWidget(w_pages[3]);
		AddWidget(w_pages[4]);
		AddWidget(w_exit);
		AddWidget(w_page);
		AddWidget(w_hover);

		// Create saveslab widgets
		for(int i=0;i<4;i++){
			SDL_Rect rdest={0,117+(80*i),317,78};
			w_slabs[i]=new SaveSlab(rdest);
			AddWidget(w_slabs[i]);
		}
		for(int i=4;i<8;i++){
			SDL_Rect rdest={320,117+(80*(i-4)),317,78};
			w_slabs[i]=new SaveSlab(rdest);
			AddWidget(w_slabs[i]);
		}


		// Free graphics
		SDL_FreeSurface(sback);
		SDL_FreeSurface(marker);
		SDL_FreeSurface(smarker);
		SDL_FreeSurface(mmarker);
		SDL_FreeSurface(keywait);
		SDL_FreeSurface(skeywait);
		SDL_FreeSurface(mkeywait);

		// Set defaults
		page=-1;
		LoadPage(0);
	}
}

MCSave::~MCSave(){
	DestroyWidgets();
}

void MCSave::LoadPage(int Page){
	if(Page!=page){
		page=Page;
		for(int i=0;i<8;i++){
			w_slabs[i]->Free();
			Savegame save(engine->NativeID(),(page*8)+i);
			if(save.Read()){
				SDL_Surface *screen=0;
				if(save.LoadSurface("screen-thumb",&screen,96,72)){
					// Set thumbview
					SDL_Rect dst={3,3,96,72};
					w_slabs[i]->SetThumb(screen,0,&dst);
					SDL_FreeSurface(screen);

					// Set saveinfo
					EDL_SETRECT(dst,110,8,200,66);
					uString savedate="Unknown date";
					save.LoadString("savedate",&savedate);
					w_slabs[i]->SetText(savedate,&dst);
				}
			}
		}
	}
}

bool MCSave::FocusEnter(Widget *Object){
	if(Object==w_pages[0] ||
	   Object==w_pages[1] ||
	   Object==w_pages[2] ||
	   Object==w_pages[3] ||
	   Object==w_pages[4]){
		w_hover->Move(Object->GetX()+20,
				Object->GetY()-Object->GetHeight());
		w_hover->SetVisible(w_page->GetX()!=w_hover->GetX());
	}
	else if(Object==w_exit){
		w_hover->Move(Object->GetX()+20,
				Object->GetY()-Object->GetHeight());
		w_hover->SetVisible(true);
	}
	return false;
}

bool MCSave::FocusLeave(Widget *Object){
	w_hover->SetVisible(false);
	return false;
}

bool MCSave::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_exit){
		// Close dialog
		engine->DestroyWidget(this);
		engine->SetTransition();
		retval=true;
	}
	else if(Object==w_pages[0] ||
			Object==w_pages[1] ||
			Object==w_pages[2] ||
			Object==w_pages[3] ||
			Object==w_pages[4]){
		// Change page
		if(Object==w_pages[0])	LoadPage(0);
		if(Object==w_pages[1])	LoadPage(1);
		if(Object==w_pages[2])	LoadPage(2);
		if(Object==w_pages[3])	LoadPage(3);
		if(Object==w_pages[4])	LoadPage(4);
		w_page->Move(w_hover->GetX(),w_hover->GetY());
		w_hover->SetVisible(false);
		retval=true;
	}
	else if(Object==w_slabs[0] ||
			Object==w_slabs[1] ||
			Object==w_slabs[2] ||
			Object==w_slabs[3] ||
			Object==w_slabs[4] ||
			Object==w_slabs[5] ||
			Object==w_slabs[6] ||
			Object==w_slabs[7]){
		// Save game
		int index=-1;
		if(Object==w_slabs[0])	index=(page*8)+0;
		if(Object==w_slabs[1])	index=(page*8)+1;
		if(Object==w_slabs[2])	index=(page*8)+2;
		if(Object==w_slabs[3])	index=(page*8)+3;
		if(Object==w_slabs[4])	index=(page*8)+4;
		if(Object==w_slabs[5])	index=(page*8)+5;
		if(Object==w_slabs[6])	index=(page*8)+6;
		if(Object==w_slabs[7])	index=(page*8)+7;
		if(((Mayclub*)engine)->EventSave(index)){
			engine->DestroyWidget(this);
			engine->SetTransition();
		}
		retval=true;
	}

	return retval;
}

