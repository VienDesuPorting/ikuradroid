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

#include "niloc.h"
#include "nocturnal.h"

NILocation::NILocation(Nocturnal *Engine) : DialogBase(Engine,true){
	if(Engine){
		// Load current
		Uint8 floor=Engine->GetValue(0x0127);
		switch(floor){
			case 0:		LoadOutDoors();		break;
			case 1:		LoadFirstFloor();	break;
			case 2:		LoadSecondFloor();	break;
		}
	}
}

void NILocation::LoadOutDoors(){
	Nocturnal *niengine=(Nocturnal*)engine;
	SDL_Surface *sbase=niengine->LoadImage("od_base.ggd");
	SDL_Surface *s1=niengine->LoadImage("od_01.ggd");
	SDL_Surface *s2=niengine->LoadImage("od_02.ggd");
	SDL_Surface *s3=niengine->LoadImage("od_03.ggd");
	SDL_Surface *s4=niengine->LoadImage("od_04.ggd");
	SDL_Surface *s5=niengine->LoadImage("od_05.ggd");
	SDL_Surface *s6=niengine->LoadImage("od_06.ggd");
	if(sbase && s1 && s2 && s3 && s4 && s5 && s6){
		// Set centering offset
		int offx=20;
		int offy=0;

		// Get source rects
		SDL_Rect r1s={0,0,s1->w-7,s1->h-6};
		SDL_Rect r2s={0,0,s2->w-7,s2->h-2};
		SDL_Rect r3s={0,0,s3->w-9,s3->h-6};
		SDL_Rect r4s={0,0,s4->w-7,s4->h-1};
		SDL_Rect r5s={0,0,s5->w-6,s5->h-2};
		SDL_Rect r6s={0,0,s6->w-7,s6->h-4};

		// Get destination rects
		SDL_Rect r1d={offx+235,offy+221,r1s.w,r1s.h};
		SDL_Rect r2d={offx+352,offy+190,r2s.w,r2s.h};
		SDL_Rect r3d={offx+410,offy+97,r3s.w,r3s.h};
		SDL_Rect r4d={offx+304,offy+64,r4s.w,r4s.h};
		SDL_Rect r5d={offx+174,offy+57,r5s.w,r5s.h};
		SDL_Rect r6d={offx+293,offy+108,r6s.w,r6s.h};

		// Configure background
		SDL_Surface *sbg=EDL_CreateSurface(640,384);
		EDL_SetBox(sbg,0,0,640,384,0,0,0,0xFF);

		SDL_Rect rbg={offx+56,offy+24,sbase->w,sbase->h};
		EDL_BlitSurface(sbase,0,sbg,&rbg);
		Move(0,0);
		Resize(640,384);
		Blit(sbg);
		SDL_FreeSurface(sbg);

		// Clear old widgets and load new ones
		DestroyWidgets();
		AddWidget(loadbutton(1,sbase,s5,&r5s,&r5d));
		AddWidget(loadbutton(2,sbase,s6,&r6s,&r6d));
		AddWidget(loadbutton(3,sbase,s4,&r4s,&r4d));
		AddWidget(loadbutton(4,sbase,s3,&r3s,&r3d));
		AddWidget(loadbutton(5,sbase,s2,&r2s,&r2d));
		AddWidget(loadbutton(0,sbase,s1,&r1s,&r1d));

		SDL_FreeSurface(s1);
		SDL_FreeSurface(s2);
		SDL_FreeSurface(s3);
		SDL_FreeSurface(s4);
		SDL_FreeSurface(s5);
		SDL_FreeSurface(s6);
		SDL_FreeSurface(sbase);
	}
	else{
		LogError("Failed to load location surfaces");
	}
}

void NILocation::LoadSecondFloor(){
	Nocturnal *niengine=(Nocturnal*)engine;
	SDL_Surface *sbase=niengine->LoadImage("2f_base.ggd");
	SDL_Surface *s1=niengine->LoadImage("2f_01.ggd");
	SDL_Surface *s2=niengine->LoadImage("2f_02.ggd");
	SDL_Surface *s3=niengine->LoadImage("2f_03.ggd");
	SDL_Surface *s4=niengine->LoadImage("2f_04.ggd");
	SDL_Surface *s5=niengine->LoadImage("2f_05.ggd");
	SDL_Surface *s6=niengine->LoadImage("2f_06.ggd");
	SDL_Surface *s7=niengine->LoadImage("2f_07.ggd");
	SDL_Surface *s8=niengine->LoadImage("2f_08.ggd");
	SDL_Surface *s9=niengine->LoadImage("2f_09.ggd");
	SDL_Surface *s10=niengine->LoadImage("2f_10.ggd");
	SDL_Surface *s11=niengine->LoadImage("2f_11.ggd");
	if(sbase && s1 && s2 && s3 && s4 && s5 &&
			s6 && s7 && s8 && s9 && s10 && s11){
		// Set centering offset
		int offx=20;
		int offy=0;

		// Get source rects
		SDL_Rect r1s={0,0,s1->w-7,s1->h-4};
		SDL_Rect r2s={0,0,s2->w-4,s2->h-2};
		SDL_Rect r3s={0,0,s3->w-5,s3->h-1};
		SDL_Rect r4s={0,0,s4->w-4,s4->h-0};
		SDL_Rect r5s={0,0,s5->w-6,s5->h-5};
		SDL_Rect r6s={0,0,s6->w-3,s6->h-6};
		SDL_Rect r7s={0,0,s7->w-4,s7->h-6};
		SDL_Rect r8s={0,0,s8->w-8,s8->h-2};
		SDL_Rect r9s={0,0,s9->w-2,s9->h-7};
		SDL_Rect r10s={0,0,s10->w-0,s10->h-2};
		SDL_Rect r11s={0,0,s11->w-2,s11->h-3};

		// Get destination rects
		SDL_Rect r1d={offx+58,offy+152,r1s.w,r1s.h};
		SDL_Rect r2d={offx+100,offy+105,r2s.w,r2s.h};
		SDL_Rect r3d={offx+136,offy+64,r3s.w,r3s.h};
		SDL_Rect r4d={offx+169,offy+27,r4s.w,r4s.h};
		SDL_Rect r5d={offx+261,offy+52,r5s.w,r5s.h};
		SDL_Rect r6d={offx+199,offy+190,r6s.w,r6s.h};
		SDL_Rect r7d={offx+248,offy+82,r7s.w,r7s.h};
		SDL_Rect r8d={offx+425,offy+101,r8s.w,r8s.h};
		SDL_Rect r9d={offx+346,offy+251,r9s.w,r9s.h};
		SDL_Rect r10d={offx+375,offy+196,r10s.w,r10s.h};
		SDL_Rect r11d={offx+401,offy+145,r11s.w,r11s.h};

		// Configure background
		SDL_Surface *sbg=EDL_CreateSurface(640,384);
		EDL_SetBox(sbg,0,0,640,384,0,0,0,0xFF);

		SDL_Rect rbg={offx+56,offy+24,sbase->w,sbase->h};
		EDL_BlitSurface(sbase,0,sbg,&rbg);
		Move(0,0);
		Resize(640,384);
		Blit(sbg);
		SDL_FreeSurface(sbg);

		// Clear old widgets and load new ones
		DestroyWidgets();
		AddWidget(loadbutton(0,sbase,s1,&r1s,&r1d));	// Redhead
		AddWidget(loadbutton(1,sbase,s2,&r2s,&r2d));	// French doll
		AddWidget(loadbutton(2,sbase,s3,&r3s,&r3d));	// Guest room
		AddWidget(loadbutton(3,sbase,s4,&r4s,&r4d));	// Dude's room
		AddWidget(loadbutton(4,sbase,s9,&r9s,&r9d));	// Home base
		AddWidget(loadbutton(5,sbase,s10,&r10s,&r10d));	// Yukina
		AddWidget(loadbutton(6,sbase,s11,&r11s,&r11d));	// Guest room
		AddWidget(loadbutton(7,sbase,s8,&r8s,&r8d));	// Locked room
		AddWidget(loadbutton(8,sbase,s6,&r6s,&r6d));	// Stairs
		AddWidget(loadbutton(9,sbase,s7,&r7s,&r7d));	// Library
		AddWidget(loadbutton(10,sbase,s5,&r5s,&r5d));	// Attic
		SDL_FreeSurface(s1);
		SDL_FreeSurface(s2);
		SDL_FreeSurface(s3);
		SDL_FreeSurface(s4);
		SDL_FreeSurface(s5);
		SDL_FreeSurface(s6);
		SDL_FreeSurface(s7);
		SDL_FreeSurface(s8);
		SDL_FreeSurface(s9);
		SDL_FreeSurface(s10);
		SDL_FreeSurface(s11);
		SDL_FreeSurface(sbase);
	}
	else{
		LogError("Failed to load location surfaces");
	}
}

void NILocation::LoadFirstFloor(){
	Nocturnal *niengine=(Nocturnal*)engine;
	SDL_Surface *sbase=niengine->LoadImage("1f_base.ggd");
	SDL_Surface *s1=niengine->LoadImage("1f_01.ggd");
	SDL_Surface *s2=niengine->LoadImage("1f_02.ggd");
	SDL_Surface *s3=niengine->LoadImage("1f_03.ggd");
	SDL_Surface *s4=niengine->LoadImage("1f_04.ggd");
	SDL_Surface *s5=niengine->LoadImage("1f_05.ggd");
	SDL_Surface *s6=niengine->LoadImage("1f_06.ggd");
	SDL_Surface *s7=niengine->LoadImage("1f_07.ggd");
	SDL_Surface *s8=niengine->LoadImage("1f_08.ggd");
	SDL_Surface *s9=niengine->LoadImage("1f_09.ggd");
	SDL_Surface *s10=niengine->LoadImage("1f_10.ggd");
	SDL_Surface *s11=niengine->LoadImage("1f_11.ggd");
	if(sbase && s1 && s2 && s3 && s4 && s5 &&
			s6 && s7 && s8 && s9 && s10 && s11){
		// Set centering offset
		int offx=20;
		int offy=0;

		// Get source rects
		SDL_Rect r1s={0,0,s1->w-5,s1->h-4};
		SDL_Rect r2s={0,0,s2->w-4,s2->h-6};
		SDL_Rect r3s={0,0,s3->w-4,s3->h-4};
		SDL_Rect r4s={0,0,s4->w-7,s4->h-5};
		SDL_Rect r5s={0,0,s5->w-4,s5->h};
		SDL_Rect r6s={0,0,s6->w-7,s6->h};
		SDL_Rect r7s={0,0,s7->w-4,s7->h-6};
		SDL_Rect r8s={0,0,s8->w-5,s8->h-5};
		SDL_Rect r9s={0,0,s9->w-6,s9->h-7};
		SDL_Rect r10s={0,0,s10->w-4,s10->h-5};
		SDL_Rect r11s={0,0,s11->w-2,s11->h-7};

		// Get destination rects
		SDL_Rect r1d={offx+188,offy+238,r1s.w,r1s.h};
		SDL_Rect r2d={offx+401,offy+96,r2s.w,r2s.h};
		SDL_Rect r3d={offx+350,offy+245,r3s.w,r3s.h};
		SDL_Rect r4d={offx+376,offy+191,r4s.w,r4s.h};
		SDL_Rect r5d={offx+187,offy+166,r5s.w,r5s.h};
		SDL_Rect r6d={offx+293,offy+215,r6s.w,r6s.h};
		SDL_Rect r7d={offx+248,offy+81,r7s.w,r7s.h};
		SDL_Rect r8d={offx+261,offy+50,r8s.w,r8s.h};
		SDL_Rect r9d={offx+69,offy+153,r9s.w,r9s.h};
		SDL_Rect r10d={offx+107,offy+106,r10s.w,r10s.h};
		SDL_Rect r11d={offx+143,offy+28,r11s.w,r11s.h};

		// Configure background
		SDL_Surface *sbg=EDL_CreateSurface(640,384);
		EDL_SetBox(sbg,0,0,640,384,0,0,0,0xFF);

		SDL_Rect rbg={offx+56,offy+24,sbase->w,sbase->h};
		EDL_BlitSurface(sbase,0,sbg,&rbg);
		Move(0,0);
		Resize(640,384);
		Blit(sbg);
		SDL_FreeSurface(sbg);

		// Clear old widgets and load new ones
		DestroyWidgets();
		AddWidget(loadbutton(0,sbase,s9,&r9s,&r9d));
		AddWidget(loadbutton(1,sbase,s10,&r10s,&r10d));
		AddWidget(loadbutton(2,sbase,s11,&r11s,&r11d));
		AddWidget(loadbutton(3,sbase,s3,&r3s,&r3d));
		AddWidget(loadbutton(4,sbase,s4,&r4s,&r4d));
		AddWidget(loadbutton(5,sbase,s2,&r2s,&r2d));	// Bath
		AddWidget(loadbutton(7,sbase,s7,&r7s,&r7d));	// Kitchen
		AddWidget(loadbutton(9,sbase,s8,&r8s,&r8d));	// Basement

		// New stuff ...
		// Location 6 used to be a secondary menu (Lobby/Upstairs/Outside)
		// Location 8 used to be the "cancel" item that returns you
		// Location 10 is actually an overflow
		AddWidget(loadbutton(6,sbase,s5,&r5s,&r5d));	// Lobby
		AddWidget(loadbutton(8,sbase,s6,&r6s,&r6d));	// Upstairs
		AddWidget(loadbutton(10,sbase,s1,&r1s,&r1d));	// Outside

		SDL_FreeSurface(s1);
		SDL_FreeSurface(s2);
		SDL_FreeSurface(s3);
		SDL_FreeSurface(s4);
		SDL_FreeSurface(s5);
		SDL_FreeSurface(s6);
		SDL_FreeSurface(s7);
		SDL_FreeSurface(s8);
		SDL_FreeSurface(s9);
		SDL_FreeSurface(s10);
		SDL_FreeSurface(s11);
		SDL_FreeSurface(sbase);
	}
	else{
		LogError("Failed to load location surfaces");
	}
}

/*! \brief Load a button using the specified sources and coordinates
 *  \param Tag Indentifying number
 *  \param Base Base graphics
 *  \param Icon Overlay graphics
 *  \param Source Coordinates to copy from the overlay graphics
 *  \param Dest Coordinates for the onscreen widget
 */
BitmapButton *NILocation::loadbutton(int Tag,
		SDL_Surface *Base,SDL_Surface *Icon,SDL_Rect *Source,SDL_Rect *Dest){
	BitmapButton *tmpptr=new BitmapButton(*Dest);
	tmpptr->SetState(WS_HOVER,Icon,Source);
	tmpptr->SetTag(Tag);
	return tmpptr;
}

bool NILocation::InputOk(Widget *Object){
	if(Object && Object->GetTag()>=0){
		// Get settings from engine
		Nocturnal *niengine=(Nocturnal*)engine;
		Uint16 floor=niengine->GetValue(0x0127);
		Uint16 room=Object->GetTag();

		// Autoselect location
		bool autoselect=false;
		int selection=0;
		if(floor==1){
			if(room==6){
				// Go to lobby
				selection=2;
				autoselect=true;
			}
			else if(room==8){
				// Go upstairs
				floor=2;
				selection=0;
				autoselect=true;
			}
			else if(room==10){
				// Go outside
				floor=0;
				selection=1;
				autoselect=true;
			}
		}

		// Unset scripted cache to avoid looping certain scenes
		niengine->SetValue(0x008E,0x00FE);

		// Register location
		LogDebug("Location: %d@%d%s",room,floor,autoselect?" (auto)":"");
		niengine->SetValue(0x0000,selection);
		//niengine->SetValue(0x012D,niengine->GetValue(0x0126));
		//niengine->SetValue(0x012E,niengine->GetValue(0x0127));
		//niengine->SetValue(0x0127,floor);
		niengine->SetValue(0x0126,room);

		// Flush graphics if we are changing scenes
		if((niengine->GetValue(0x0126)!=niengine->GetValue(0x012D)) ||
		   (niengine->GetValue(0x0127)!=niengine->GetValue(0x012E))){
			niengine->FillBackground(0x000000FF);
		}

		// Jump to menu selection
		niengine->EventSelect(selection);
		if(autoselect){
			niengine->Jump(0x7744);
		}
	}
	return true;
}


