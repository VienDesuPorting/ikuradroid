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

#include "sequencer.h"
#include "evn.h"

Sequencer::Sequencer(EngineVN *Engine){
	engine=Engine;
}

/*! \brief Blends a text logo onto a surface
 *  \param Surface Surface to blend into
 *  \param FG Color for text
 */
void Sequencer::CreateLogoText(SDL_Surface *Surface,SDL_Color FG){
	// Create a logo
	Printer printer;
	printer.Resize(Surface->w,Surface->h);
	// Get proper fontsize
	int size=36;
	int width=0,height=0;
	int lastwidth=0,lastheight=0;

	printer.SetFontSize(size);//android

	for(int i=4;i<400;i++){
		//printer.SetFontSize(i);
		if(printer.GetTextSize("ViLE",&width,&height)){
			if(width>Surface->w || height>Surface->h){
				break;
			}
			else if(width>lastwidth || height>lastheight){
				size=i;
			}
		}
	}
	printer.SetFontSize(size);

	// Print name
	printer.Clear();
	printer.SetFontColor(FG.r,FG.g,FG.b);
	printer.Move(0,0);
	printer.Print("V LE",0);
	//printer.Copy(Surface);

	// Calculate and print 'i' in different color
	if(printer.GetTextSize("V",&width,&height)){
		printer.SetFontColor(0xFF,0,0);
		printer.Move(width,0);
		printer.Clear();
		printer.Print("i",0);
//		printer.Copy(Surface);
	}
}

/*! \brief Creates a transparent logo
 *  \param Width Width of logo surface
 *  \param Height Height of logo surface
 *  \param FG Color for text
 *  \return Alpha blendable text logo
 */
SDL_Surface *Sequencer::CreateLogoAlpha(int Width,int Height,SDL_Color FG){
	SDL_Surface *surface=EDL_CreateSurface(Width,Height);
	CreateLogoText(surface,FG);
	return surface;
}

/*! \brief Creates an opaque logo
 *  \param Width Width of logo surface
 *  \param Height Height of logo surface
 *  \param BG Color for background
 *  \param FG Color for text
 *  \return Alpha blendable text logo
 */
SDL_Surface *Sequencer::CreateLogoOpaque(int Width,int Height,
		SDL_Color BG,SDL_Color FG){

	SDL_Surface *surface=EDL_CreateSurface(Width,Height);
	EDL_SetBox(surface,0,0,Width,Height,BG.r,BG.g,BG.b,0xFF);
	CreateLogoText(surface,FG);
	return surface;
}

void Sequencer::RollPaddedVertical(SDL_Surface *Surface){
	// Pad surface
	Uint8 r,g,b,a;
	int height=engine->NativeHeight();
	SDL_Surface *tmp=EDL_CreateSurface(Surface->w,Surface->h+(height*2));
	EDL_GetPixel(Surface,Surface->w/2,0,&r,&g,&b,&a);
	EDL_SetBox(tmp,0,0,Surface->w,height,r,g,b,0xFF);

	EDL_GetPixel(Surface,Surface->w/2,Surface->h-1,&r,&g,&b,&a);
	EDL_SetBox(tmp,0,height+Surface->h,Surface->w,
			(height*2)+Surface->h,r,g,b,0xFF);
	SDL_Rect rpad={0,height,Surface->w,Surface->h};
	EDL_BlitSurface(Surface,0,tmp,&rpad);

	// Roll padded surface
	RollVertical(tmp);
	SDL_FreeSurface(tmp);
}

void Sequencer::RollVertical(SDL_Surface *Surface){
	// Fade to background color
	Uint8 r,g,b,a;
	int width=engine->NativeWidth();
	int height=engine->NativeHeight();
	SDL_Rect full={0,0,width,height};
	EDL_GetPixel(Surface,Surface->w/2,0,&r,&g,&b,&a);
	engine->AddAnimation(new FadeColor(full,r,g,b,1000));

	// Scroll staffroll
	SDL_Rect start={0,0,Surface->w,Surface->h};
	SDL_Rect end={0,Surface->h-height,Surface->w,Surface->h};
	SDL_Rect dst={(width-Surface->w)/2,0,Surface->w,height};
	Uint32 time=SEQUENCER_MSPERSCREEN*(Surface->h/(double)height);
	engine->AddAnimation(new Scroll(dst,Surface,start,end,time));
}

/*! \brief Picks a random ViLE logo with a random background color
 */
void Sequencer::LogoRandom(){
	if(EDL_Random()){
		LogoRandom(0xFF,0xFF,0xFF);
	}
	else{
		LogoRandom(0x00,0x00,0x00);
	}
}

/*! \brief Picks a random ViLE logo
 *  \param R Color value for background
 *  \param G Color value for background
 *  \param B Color value for background
 */
void Sequencer::LogoRandom(Uint8 R,Uint8 G,Uint8 B){
	int seed=EDL_Random(0,1);
	if(seed==0){
		LogoZoom(R,G,B);
	}
	else{
		LogoFlicker(R,G,B);
	}
}

/*! \brief Zoom into a ViLE logo and fade in url
 *  \param R Color value for background
 *  \param G Color value for background
 *  \param B Color value for background
 */
void Sequencer::LogoZoom(Uint8 R,Uint8 G,Uint8 B){
	// Get sizes and colors

	SDL_Color BG={R,G,B};
	SDL_Color FG={0xFF,0xFF,0xFF};
	if((R+G+B)/3>0xFF/2){
		EDL_SETCOLOR(FG,0x00,0x00,0x00);
	}
	int ratio=8;
	int width=engine->NativeWidth();
	int height=engine->NativeHeight();
	int widthlogo=0,heightlogo=0;
	Printer printer;
	printer.SetFontSize(12);
	printer.GetTextSize("ViLE",&widthlogo,&heightlogo);
	SDL_Rect rfull={0,0,width,height};
	SDL_Rect rend={(width-(width/ratio))/2,
		(height-(height/ratio))/2,
		width/ratio,height/ratio};

	// Create surfaces
	int widthurl=0,heighturl=0;
	printer.GetTextSize("www.vilevn.org",&widthurl,&heighturl);

	printer.Resize(widthurl,heighturl);

	SDL_Rect rsmall={0,0,widthurl,heighturl};
	SDL_Rect rurl={rend.x+((rend.w-widthurl)/2),
		height*(heightlogo/(double)widthlogo),
		widthurl,heighturl};

	SDL_Surface *urlnormal=EDL_CreateSurface(rurl.w,rurl.h);
	EDL_SetBox(urlnormal,0,0,rurl.w,rurl.h,BG.r,BG.g,BG.b,0xFF);


	SDL_Surface *urlred=EDL_CopySurface(urlnormal,0);

	SDL_Surface *logobig=CreateLogoOpaque(width,height,BG,FG);

	// Print tag
	printer.Clear();
	printer.SetFontColor(0xFF,0x00,0x00);
	printer.Print("www.vilevn.org",0);
//	printer.Copy(urlred);
	printer.SetFontColor(FG.r,FG.g,FG.b);
	printer.Reprint(0);
//	printer.Copy(urlnormal);

	// Assert background and zoom in logo
	engine->AddAnimation(new FadeColor(rfull,R,G,B,150));
	engine->AddAnimation(new Zoom(logobig,rfull,rfull,rend,400));

	// Animate url
	engine->AddAnimation(new Fade(rurl,urlnormal,rsmall,1000));
	engine->AddAnimation(new Fade(rurl,urlred,rsmall,1000));
	engine->AddAnimation(new Fade(rurl,urlred,rsmall,2000));

	SDL_FreeSurface(urlnormal);
	SDL_FreeSurface(urlred);
	SDL_FreeSurface(logobig);
}

/*! \brief Loads a simple "powered by" logo
 *  \param R Color value for background
 *  \param G Color value for background
 *  \param B Color value for background
 */
void Sequencer::LogoFlicker(Uint8 R,Uint8 G,Uint8 B){
	// Get size and color
	SDL_Color BG={R,G,B};
	SDL_Color FG={0xFF,0xFF,0xFF};
	if((R+G+B)/3>0xFF/2){
		EDL_SETCOLOR(FG,0x00,0x00,0x00);
	}
	int width=engine->NativeWidth();
	int height=engine->NativeHeight();
	SDL_Rect rfull={0,0,width,height};
	SDL_Rect rlogo={0,0,80,55};
	SDL_Rect rend={(width-rlogo.w)/2,(height-rlogo.h)/2,rlogo.w,rlogo.h};

	// Create surfaces
	int widthtag=0,heighttag=0;
	Printer printer;
	printer.SetFontSize(12);
	printer.GetTextSize("Powered By",&widthtag,&heighttag);
	printer.Resize(widthtag,heighttag);
	SDL_Rect rsmall={0,0,widthtag,heighttag};
	SDL_Rect rtag={rend.x+((rend.w-widthtag)/2),
		rend.y-heighttag,widthtag,heighttag};
	SDL_Surface *tagback=EDL_CreateSurface(rtag.w,rtag.h);
	EDL_SetBox(tagback,0,0,rtag.w,rtag.h,BG.r,BG.g,BG.b,0xFF);
	SDL_Surface *tagnormal=EDL_CopySurface(tagback,0);
	SDL_Surface *tagdim=EDL_CopySurface(tagback,0);
	SDL_Surface *logofg=CreateLogoOpaque(rlogo.w,rlogo.h,BG,FG);
	SDL_Surface *logodim=EDL_CopySurface(logofg,0);
	SDL_Surface *logodark=EDL_CopySurface(logofg,0);
	SDL_Surface *logoblend=EDL_CreateSurface(rlogo.w,rlogo.h);
	EDL_SetBox(logoblend,0,0,rlogo.w,rlogo.h,BG.r,BG.g,BG.b,0xFF);

	//SDL_SetAlpha(logoblend,SDL_SRCALPHA,128);
	SDL_SetSurfaceAlphaMod(logoblend,128);
	EDL_BlitSurface(logoblend,0,logodim,0);
	EDL_BlitSurface(logoblend,0,logodark,0);
	EDL_BlitSurface(logoblend,0,logodark,0);

	// Print slogan
	printer.Clear();
	printer.SetFontColor((FG.r+BG.r)/2,(FG.g+BG.g)/2,(FG.b+BG.b)/2);
	printer.Print("Powered By",0);
//	printer.Copy(tagdim);
	printer.SetFontColor(FG.r,FG.g,FG.b);
	printer.Reprint(0);
//	printer.Copy(tagnormal);

	// Assert background
	engine->AddAnimation(new FadeColor(rfull,R,G,B,100));

	// Flicker
	engine->AddAnimation(new Fade(rtag,tagdim,rsmall,20));
	engine->AddAnimation(new Fade(rend,logodark,rlogo,50));
	engine->AddAnimation(new Fade(rtag,tagdim,rsmall,200));
	engine->AddAnimation(new Fade(rtag,tagback,rsmall,10));
	engine->AddAnimation(new Fade(rtag,tagdim,rsmall,10));
	engine->AddAnimation(new Fade(rtag,tagback,rsmall,10));

	// Dim
	engine->AddAnimation(new Fade(rtag,tagnormal,rsmall,30));
	engine->AddAnimation(new Fade(rend,logodim,rlogo,100));
	engine->AddAnimation(new Fade(rtag,tagback,rsmall,20));
	engine->AddAnimation(new Fade(rtag,tagback,rsmall,40));

	// Really light
	engine->AddAnimation(new Fade(rtag,tagnormal,rsmall,30));
	engine->AddAnimation(new Fade(rend,logofg,rlogo,100));
	engine->AddAnimation(new Fade(rend,logofg,rlogo,3000));

	// Free surfaces
	SDL_FreeSurface(logofg);
	SDL_FreeSurface(logodim);
	SDL_FreeSurface(logodark);
	SDL_FreeSurface(logoblend);
	SDL_FreeSurface(tagnormal);
	SDL_FreeSurface(tagdim);
	SDL_FreeSurface(tagback);
}


