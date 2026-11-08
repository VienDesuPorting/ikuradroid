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
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
 */

#include "printer.h"

Printer::Printer(int X,int Y,int Width,int Height) : Widget(X,Y,Width,Height){
	// Preset values
	stext=0;
	stexturetext=0;
	trect.x=0;
	trect.y=0;
	index=0;
	interval=50;
	charcount=0;
	leading=0;
	wordwrap=true;
	clearscreen=false;
	shadow_x=0;
	shadow_y=0;

	// Cursor animation state (no sheet loaded yet)
	cursor_sheet=0;
	cursor_fw=0;
	cursor_fh=0;
	cursor_frames=0;
	cursor_textures=0;
	cursor_frame=0;
	cursor_lasttick=0;
	cursor_framems=0;
	cursor_on=false;

	// Create default font
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::Printer(SDL_Rect Pos) : Widget(Pos){
	// Preset values
	stext=0;
	stexturetext=0;
	trect.x=0;
	trect.y=0;
	index=0;
	interval=50;
	charcount=0;
	leading=0;
	wordwrap=true;
	clearscreen=false;
	shadow_x=0;
	shadow_y=0;

	// Cursor animation state (no sheet loaded yet)
	cursor_sheet=0;
	cursor_fw=0;
	cursor_fh=0;
	cursor_frames=0;
	cursor_textures=0;
	cursor_frame=0;
	cursor_lasttick=0;
	cursor_framems=0;
	cursor_on=false;

	// Create default font
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::Printer() : Widget(){
	// Preset values
	stext=0;
	stexturetext=0;
	trect.x=0;
	trect.y=0;
	index=0;
	interval=50;
	charcount=0;
	leading=0;
	wordwrap=true;
	clearscreen=false;
	shadow_x=0;
	shadow_y=0;

	// Cursor animation state (no sheet loaded yet)
	cursor_sheet=0;
	cursor_fw=0;
	cursor_fh=0;
	cursor_frames=0;
	cursor_textures=0;
	cursor_frame=0;
	cursor_lasttick=0;
	cursor_framems=0;
	cursor_on=false;

	// Create default font
	font_ttf=0;
	font_size=Cfg::Font::default_size;
	font_style=Cfg::Font::default_style;
	SetBackgroundColor(0x00000000);
	SetFontColor(0xFFFFFFFF);
	SetFontFace(Cfg::Font::default_face.c_str());
}

Printer::~Printer(){
	FreeCursor();
	if(stext){
		SDL_FreeSurface(stext);
	}
	if(stexturetext){
            stexturetext->free();
			stexturetext=0;
	}

	if(font_ttf){
		TTF_CloseFont(font_ttf);
	}
}

void Printer::SetWordwrap(bool Enable){
	wordwrap=Enable;
}

/*! \brief Releases the click-to-continue cursor animation
 *
 * The sheet surface itself stays owned by the engine decoder.
 */
void Printer::FreeCursor(){
	if(cursor_textures){
		for(int i=0;i<cursor_frames;i++){
			if(cursor_textures[i]){
				cursor_textures[i]->free();
				delete cursor_textures[i];
			}
		}
		delete [] cursor_textures;
		cursor_textures=0;
	}
	cursor_frames=0;
	cursor_frame=0;
	cursor_on=false;
	cursor_sheet=0;
}

/*! \brief Sets the click-to-continue cursor sheet (DRS 0x21/0x22)
 *  \param Sheet Horizontal strip of FrameWidth x FrameHeight frames (not owned)
 *  \param FrameMS Hold time of one animation frame in milliseconds
 *
 *  The sheet is sliced into prebuilt textures; the DRS sheets carry a
 *  pure black background that acts as the color key (the original
 *  blits through a black DirectDraw colorkey).
 */
void Printer::SetCursor(SDL_Surface *Sheet,int FrameWidth,int FrameHeight,Uint32 FrameMS){
	FreeCursor();
	if(Sheet && FrameWidth>0 && FrameHeight>0 &&
				Sheet->w>=FrameWidth && Sheet->h>=FrameHeight){
		cursor_fw=FrameWidth;
		cursor_fh=FrameHeight;
		cursor_frames=Sheet->w/FrameWidth;
		cursor_framems=FrameMS?FrameMS:120;
		cursor_lasttick=SDL_GetTicks();
		if(cursor_frames>0 && Sheet->format->BytesPerPixel<=4){
			cursor_textures=new EDLTexture*[cursor_frames];
			for(int i=0;i<cursor_frames;i++){
				cursor_textures[i]=0;
			}
			for(int i=0;i<cursor_frames;i++){
				// Slice one frame into a 32bit surface, keying black out
				SDL_Surface *rgba=SDL_CreateRGBSurface(0,FrameWidth,FrameHeight,32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
							0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
#else
							0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
#endif
				if(rgba){
					if(SDL_MUSTLOCK(rgba)){
						SDL_LockSurface(rgba);
					}
					for(int y=0;y<FrameHeight;y++){
						const Uint8 *src=(const Uint8*)Sheet->pixels+
							y*Sheet->pitch+i*FrameWidth*Sheet->format->BytesPerPixel;
						Uint32 *dst=(Uint32*)((Uint8*)rgba->pixels+y*rgba->pitch);
						for(int x=0;x<FrameWidth;x++){
							Uint32 pixel=0;
							switch(Sheet->format->BytesPerPixel){
								case 1:
									pixel=src[x];
									break;
								case 2:
									pixel=((const Uint16*)src)[x];
									break;
								case 3:
									pixel=src[x*3]|(src[x*3+1]<<8)|(src[x*3+2]<<16);
									break;
								default:
									pixel=((const Uint32*)src)[x];
									break;
							}
							Uint8 r,g,b;
							SDL_GetRGB(pixel,Sheet->format,&r,&g,&b);
							dst[x]=SDL_MapRGBA(rgba->format,r,g,b,
										(r||g||b)?0xFF:0x00);
						}
					}
					if(SDL_MUSTLOCK(rgba)){
						SDL_UnlockSurface(rgba);
					}
					cursor_textures[i]=new EDLTexture();
					if(!cursor_textures[i]->loadFromSurface(rgba)){
						cursor_textures[i]->free();
						delete cursor_textures[i];
						cursor_textures[i]=0;
					}
					SDL_FreeSurface(rgba);
				}
			}
			// Drop the whole animation when no frame survived
			int alive=0;
			for(int i=0;i<cursor_frames;i++){
				if(cursor_textures[i]){
					alive++;
				}
			}
			if(!alive){
				FreeCursor();
			}
		}
	}
}

/*! \brief Shows or hides the click-to-continue cursor */
void Printer::ShowCursor(bool Enable){
	cursor_on=Enable;
	if(Enable){
		cursor_lasttick=SDL_GetTicks();
	}
}

/*! \brief Pins the row pitch (leading) between text rows
 *  \param Leading Row pitch in pixels (0 restores the font height)
 *
 *  Rows advance by the height of the font's "A" glyph by default.
 *  Engines that space dialog rows on a fixed grid (Ikura: 28px) pin
 *  the pitch with this override instead.
 */
void Printer::SetLeading(int Leading){
	leading=Leading;
}

void Printer::SetFontFace(string Name){
	// Verify font existence
	if(Name.length() && Name!=font_name){
		FILE *tf=fopen(Name.c_str(),"r");
		if(tf){
			fclose(tf);
			font_name=Name;
			if(font_ttf){
				TTF_CloseFont(font_ttf);
			}
			font_ttf=TTF_OpenFont(font_name.c_str(),font_size);
			if(!font_ttf){
				LogError("Failed to open font: %s",font_name.c_str());
			}
		}
		else{
			LogError("Failed to find font: %s",font_name.c_str());
		}
	}
}

void Printer::SetFontSize(int Size){
	// Register new size
	font_size=Size;

	// Attempt to recreate font
	if(font_name.length()){
		if(font_ttf){
			TTF_CloseFont(font_ttf);
		}
		font_ttf=TTF_OpenFont(font_name.c_str(),font_size);
	}
}

void Printer::SetBackgroundColor(Uint32 Color){
	back_color.r=(Color>>24)&0xFF;
	back_color.g=(Color>>16)&0xFF;
	back_color.b=(Color>>8)&0xFF;
	back_alpha=Color&0xFF;
//	back_alpha=128&0xFF;
}

void Printer::SetFontColor(Uint32 Color){
	font_color.r=(Color>>24)&0xFF;
	font_color.g=(Color>>16)&0xFF;
	font_color.b=(Color>>8)&0xFF;
}

void Printer::SetFontColor(Uint8 Red,Uint8 Green,Uint8 Blue){
	font_color.r=Red;
	font_color.g=Green;
	font_color.b=Blue;
}

void Printer::SetFontColor(SDL_Color Color){
	font_color.r=Color.r;
	font_color.g=Color.g;
	font_color.b=Color.b;
}

void Printer::SetFontStyle(int Style){
	font_style=Style;
	if(font_ttf){
		TTF_SetFontStyle(font_ttf,font_style);
	}
}

void Printer::SetFontShadow(int X,int Y,SDL_Color Color){
	shadow_x=X;
	shadow_y=Y;
	shadow_color=Color;
}

void Printer::SetFontGlow(int Glow,SDL_Color Color){
	LogMessage("SETFONTGLOW");
}

/*! \brief Prints remaining text immidiatly
 *  \return True if text was rendered
 */
bool Printer::Skip(){
	bool retval=font_ttf && text.length();
	if(retval){
		while(text.length()){
			while(printnext());
		}
		Refresh();
	}
	return retval;
}

/*! \brief Prints remaining text immidiatly, but wait at full display
 *  \return True if text was rendered
 */
bool Printer::SkipScreen(){
	bool retval=text.length();
	if(retval){
		if(clearscreen){
			clearscreen=false;
			ClearScreen();
			start=SDL_GetTicks();
		}
		while(printnext());
		Refresh();
	}
	return retval;
}


/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Printer::Continue(){
	return text.length();
}

/*! \brief Clear text and display
 */
void Printer::Clear(){
	if(stext){
		SDL_FreeSurface(stext);
		stext=0;
		Refresh();
	}
	if(stexturetext){
		stexturetext->free();
		stexturetext=0;
		//Refresh();
	}

	index=0;
	text="";
	curtext="";
	charcount=0;
	// Define the carriage: printnext no longer resets it when
	// it recreates the text surface
	trect.x=0;
	trect.y=0;
}

/*! \brief Clear display only
 */
void Printer::ClearScreen(){
	if(stext){
		// Drop printed text
		SDL_FreeSurface(stext);
		stext=0;
		Refresh();
	}
	if(stexturetext){
		stexturetext->free();
		stexturetext=0;
		//Refresh();
	}
	if(text.length()>index){
		// Clear currently printed text
		start=SDL_GetTicks();
		text=text.substr(index);
		index=0;
	}
	else{
		// Clear all text
		text="";
		index=0;
	}
	curtext="";
	charcount=0;
	// Continuation pages resume on the body origin: row 2 of
	// a pinned grid (leading), row 1 on the legacy font grid
	trect.y=(leading>0?leading:0);
	trect.x=0;
}

/*! \brief Total number of characters to be printed
 *  \returns Total number of characters to be printed
 */
int Printer::GetLength(){
	return charcount;
}

/*! \brief Number of characters not yet printed
 *  \returns Number of characters not yet printed
 */
int Printer::GetRemaining(){
	return EDL_MAX(text.length()-index,0);
}

int Printer::GetInterval(){
	return interval;
}

void Printer::SetInterval(Uint32 Interval){
	interval=Interval;
}

void Printer::Newline(){
	if(font_ttf){
		if(text.length()){
			text+="\n";
		}
		else{
			int w,h;
			if(!EDL_SizeUTF8(font_ttf,"A",&w,&h)){
				trect.x=0;
				trect.y+=(leading>0?leading:h);
			}
		}
		Refresh();
	}
}

void Printer::Reprint(int Interval){
	// Complete current text
	Skip();

	// Start over
	start=SDL_GetTicks();
	string curtextb=curtext;
	Clear();
	text=curtextb;
	curtext="";
	index=0;

	// Append textlength and override interval
	charcount+=text.length();
	if(Interval>=0){
		interval=Interval;
	}
	Refresh();

}

/*! \brief Prints text (Appended to existing)
 *  \param Text Text to be printed
 *  \param Interval Delay between each character (-1=Use preset value)
 */

void Printer::Print(string Text,int Interval){

	if(text.length()){
		// Append to existing text
		text+=Text;
	}
	else{
		// Prepare printing data
		start=SDL_GetTicks();
		text=Text;
		index=0;
	}

	// Append textlength and override interval
	charcount+=Text.length();
	if(Interval>=0){
		interval=Interval;
	}
	Refresh();
}

void Printer::Autosize(string Text){
	if(font_ttf){
		int width,height;
		if(!EDL_SizeUTF8(font_ttf,Text.c_str(),&width,&height)){
			Resize(width,height);
		}
	}
}

bool Printer::GetTextSize(string Text,int *Width,int *Height){
	bool retval=false;
	if(font_ttf){
		retval=!EDL_SizeUTF8(font_ttf,Text.c_str(),Width,Height);
	}
	return retval;
}

/*! \brief Prints next character to the internal surface
 *  \returns True if text was added
 */


bool Printer::printnext(){
	bool retval=false;

	if(font_ttf && text.length()){
		if(!stext){
			// The carriage is already positioned by the Clear,
			// ClearScreen and Newline paths; resetting it here would
			// wipe a pre-stream row advance (the ikura quote-row skip)
			stext=EDL_CreateSurface(pos.w,pos.w);
		}

		if(!stexturetext){
			stexturetext=EDL_CreateTexture(pos.w,pos.w);
		}

		// Check for special characters
		while(index<text.length() &&
				(text[index]=='\r' ||
				text[index]=='\n' ||
				text[index]=='\t')){
			if(text[index]=='\r' && text[index+1]=='\n'){
				index++;
			}
			else if(text[index]=='\r' || text[index]=='\n'){
				index++;
				int w,h;
				if(!EDL_SizeUTF8(font_ttf,"A",&w,&h)){
					trect.x=0;
					trect.y+=(leading>0?leading:h);
				}
			}
			else if(text[index]=='\t'){
				index++;
				int w,h;
				if(!EDL_SizeUTF8(font_ttf,"A",&w,&h)){
					trect.x+=(w*4);
				}
			}
		}

		// Check for horizontal wordwrap
		if(wordwrap && index<text.length() && text[index]==' '){
			unsigned int end=index+1;
			for(;end<text.length() && text[end]!=' ' && text[end]!='\n';end++);
			string word=text.substr(index+1,end-index);
			int w,h;
			if(!EDL_SizeUTF8(font_ttf,word.c_str(),&w,&h)){
				if(trect.x && trect.x+w>pos.w){
					// Wrap text and drop whitespace
					trect.x=0;
					trect.y+=(leading>0?leading:h);
					index++;
				}
			}
		}

		// Check for vertical wordwrap
		if(wordwrap && index<text.length()){
			int w,h;
			if(!EDL_SizeUTF8(font_ttf,"A",&w,&h)){
				if(trect.y && trect.y+(h*2)>pos.h){
					clearscreen=true;
					return false;
				}
			}
		}

		// Get next character
		if(index<text.length()){
           string print=text.substr(index++,1).c_str();
			curtext+=print;
			int w,h;
			if(!EDL_SizeUTF8(font_ttf,print.c_str(),&w,&h)){
				trect.w=w;
				trect.h=h;
			}

			// Check for ending
			if(index==text.length()){
				text="";
				index=0;
			}

			// Drop shadow (opt-in via SetFontShadow): an ink-colored copy
			// of the glyph at the offset, blitted before the glyph itself
			if(shadow_x||shadow_y){
				SDL_Surface *ss=EDL_RenderText(font_ttf,print,shadow_color);
				if(ss){
					SDL_Rect srect=trect;
					srect.x+=shadow_x;
					srect.y+=shadow_y;
					EDL_BlitSurface(ss,0,stext,&srect);
					SDL_FreeSurface(ss);
				}
			}

			// Experimental font rendering
			SDL_Surface *sp=0;

			if((sp=EDL_RenderText(font_ttf,print,font_color))){
				// Copy text to image
				EDL_BlitSurface(sp,0,stext,&trect);

				SDL_FreeSurface(sp);
				trect.x+=trect.w;
				retval=true;

			}
			else {
				LogError("Failed to render font: %s",TTF_GetError());
			}
		}
	}

	return retval;
}

void Printer::Render(){
		// Update background
		if(back_alpha){
			boxRGBA(EDLRenderer,pos.x,pos.y,pos.x+pos.w,pos.y+pos.h,
					back_color.r,back_color.g,
					back_color.b,back_alpha);
		}

		// Update delayed text
		bool refresh=text.length();
		if(refresh && !clearscreen){
			if(interval){
				Uint32 now=SDL_GetTicks();
				if(now<start){
					start=SDL_GetTicks();
				}
				while(text.length() &&
						now>start+(interval*index) &&
						(refresh=printnext()));

			}
			else{
				while(text.length() && (refresh=printnext()));
			}

			// Force refresh until all text has been printed
			Refresh();
		}
		if(simage){
			// Use widget graphics
			//Widget::Copy(Dst);
			Widget::Render();
		}

		if(stexturetext){
            stexturetext->lockTexture();
            stexturetext->copyPixels( stext->pixels );
            stexturetext->unlockTexture();
           // stexturetext->setBlendMode(SDL_BLENDMODE_BLEND);
			// Use widget graphics
			stexturetext->render(pos.x,pos.y);
		}

		// Click-to-continue cursor: tracks the carriage (the position the
		// next character would land on), 4px to its right, clamped to the
		// print area; the frame advances (frame+1)%count every FrameMS
		if(cursor_on && cursor_textures && cursor_frames>0){
			Uint32 now=SDL_GetTicks();
			if(now<cursor_lasttick || now-cursor_lasttick>=cursor_framems){
				cursor_lasttick=now;
				cursor_frame=(cursor_frame+1)%cursor_frames;
			}
			int cx=pos.x+trect.x+4;
			int cy=pos.y+trect.y;
			if(cx+cursor_fw>pos.x+pos.w){
				cx=pos.x+pos.w-cursor_fw;
			}
			if(cx<pos.x){
				cx=pos.x;
			}
			if(cy+cursor_fh>pos.y+pos.h){
				cy=pos.y+pos.h-cursor_fh;
			}
			if(cy<pos.y){
				cy=pos.y;
			}
			if(cursor_textures[cursor_frame]){
				cursor_textures[cursor_frame]->render(cx,cy);
			}
		}
}
