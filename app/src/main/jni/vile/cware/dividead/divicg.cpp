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

#include "divicg.h"
#include "dividead.h"

DiviCG::DiviCG(DiviDead *Engine) : Hotspot(32,8,576,376) {
	game=Engine;
	loadpage(0);
}

void DiviCG::loadpage(int Index){
	// Load images and staring index
	overview=true;
	pageindex=Index;
	SDL_Surface *bgs=0;
	SDL_Surface *fgs=0;
	int sindex=0;
	if(pageindex==0){
		bgs=game->LoadImage("cgmode_1");
		fgs=game->LoadImage("cgmode_a");
	}
	else if(pageindex==1){
		bgs=game->LoadImage("cgmode_2");
		fgs=game->LoadImage("cgmode_b");
		sindex+=24-1;
	}
	else if(pageindex==2){
		bgs=game->LoadImage("cgmode_2");
		fgs=game->LoadImage("cgmode_c");
		sindex+=24+(23*1)-1;
	}
	else if(pageindex==3){
		bgs=game->LoadImage("cgmode_2");
		fgs=game->LoadImage("cgmode_d");
		sindex+=24+(23*2)-1;
	}
	else if(pageindex==4){
		bgs=game->LoadImage("cgmode_2");
		fgs=game->LoadImage("cgmode_e");
		sindex+=24+(23*3)-1;
	}
	else if(pageindex==5){
		bgs=game->LoadImage("cgmode_3");
		fgs=game->LoadImage("cgmode_f");
		sindex+=24+(23*4)-1;
	}

	// Populate view
	if(bgs){
		Blit(bgs);
		if(fgs){
			bool flag;
			uString name;
			SDL_Rect r={0,0,107,70};
			int x=0,y=0;
			for(int i=sindex;i<sindex+25;i++){
				if(game->MapCG(i,&flag,&name) && flag){
					r.x=(x*(r.w+5))+10;
					r.y=(y*(r.h+4))+5;
					Blit(fgs,&r,&r);
				}
				if(++x>4){
					x=0;
					y++;
				}
			}
		}
		SDL_FreeSurface(bgs);
	}

}

bool DiviCG::MouseLeftDown(int X,int Y){
	if(overview){
		// Calculate where the user clicked
		int x=X-pos.x;
		int y=Y-pos.y;
		int w=pos.w/5;
		int h=pos.h/5;
		int hi=x/w;
		int vi=y/h;
		if(hi==0 && vi==0 && pageindex!=0){
			loadpage(pageindex-1);
		}
		else if(hi==4 && vi==4 && pageindex!=5){
			loadpage(pageindex+1);
		}
		else{
			int index=0;
			bool flag=false;
			uString name;
			if(pageindex){
				index=24+(23*(pageindex-1))-1;
			}
			index+=hi+(vi*5);
			if(game->MapCG(index,&flag,&name) && flag){
				SDL_Surface *surface=game->LoadImage(name);
				if(surface){
					// Load widget and display surface
					PopImage *pop=new PopImage(game);
					pop->AddImage(surface,32,8);
					game->AddWidget(pop,VL_OVERLAY);
					SDL_FreeSurface(surface);

					// Add related resources
					if(name=="i_20"){
						surface=game->LoadImage("i_20_");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
					if(name=="i_21"){
						surface=game->LoadImage("i_21a");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
					if(name=="i_26"){
						surface=game->LoadImage("i_26a");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
					if(name=="i_33"){
						surface=game->LoadImage("i_34");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
						surface=game->LoadImage("i_34a");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
					if(name=="i_46"){
						surface=game->LoadImage("i_46a");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
					if(name=="i_85"){
						surface=game->LoadImage("i_85a");
						pop->AddImage(surface,32,8);
						SDL_FreeSurface(surface);
					}
				}
			}
		}
	}
	else{
		// Close currently shown image
		loadpage(pageindex);
	}
	return true;
}

