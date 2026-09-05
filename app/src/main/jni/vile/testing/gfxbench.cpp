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

#include "gfxbench.h"

GFXBench::GFXBench(int Runs){
	Header("Graphic functions");
	SDL_Surface *s1=EDL_CreateSurface(640,480);
	SDL_Surface *s2=EDL_CreateSurface(640,480);
	Start("Creating and deallocating %d 640x480 surfaces",Runs);
	for(int i=0;i<Runs;i++){
		SDL_Surface *tmp=EDL_CreateSurface(640,480);
		SDL_FreeSurface(tmp);
	}
	Stop(Runs);
	Start("Filling %d 640x480 surfaces with 32b color",Runs);
	for(int i=0;i<Runs;i++){
		//boxRGBA(s1,0,0,640,480,0,0,0,0);
		boxRGBA(EDLRenderer,0,0,640,480,0,0,0,0);
	}
	Stop(Runs);
	Start("Filling %d 320x240 surfaces with 32b color",Runs);
	for(int i=0;i<Runs;i++){
		//boxRGBA(s2,0,0,320,240,0xFF,0xFF,0xFF,0xFF);
		boxRGBA(EDLRenderer,0,0,320,240,0xFF,0xFF,0xFF,0xFF);
	}
	Stop(Runs);
	Start("Blitting %d 640x480 surfaces",Runs);
	for(int i=0;i<Runs;i++){
		EDL_BlitSurface(s2,0,s1,0);
	}
	Stop(Runs);
	Start("Blending %d 640x480 surfaces",Runs);
	for(int i=0;i<Runs;i++){
		EDL_BlendSurface(s2,0,s1,0);
	}
	Stop(Runs);

	/*
	Start("Rendering and blitting %d 640x480 text surfaces",Runs);
	for(int i=0;i<Runs;i++){
		EDL_BlitText("TEXT TEST",0xFFFFFF00,s1,0);
	}
	Stop(Runs);

	Start("Rendering and blending %d 640x480 text surfaces",Runs);
	for(int i=0;i<Runs;i++){
		EDL_BlendText("TEXT TEST",0xFFFFFF00,s1,0);
	}
	Stop(Runs);
	*/


	SDL_FreeSurface(s1);
	SDL_FreeSurface(s2);
	Report();
}


