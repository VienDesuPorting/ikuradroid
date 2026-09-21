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

#include "edl_gfx.h"
#include "config.h"

SDL_Renderer* EDLRenderer = NULL;
TTF_Font *EDLfont= NULL;
EDLTexture *EDLrendererTex= NULL;

void EDL_SetBox(SDL_Surface *src, int x, int y, int w, int h, Uint8 R, Uint8 G, Uint8 B, Uint8 A){
        SDL_Rect td={x, y, w, h};
        SDL_FillRect(src, &td, SDL_MapRGBA(src->format,R,G,B,A));
}

inline void EDL_GetPixel_8bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	*red=surface->format->palette->colors[*pixel].r;
	*green=surface->format->palette->colors[*pixel].g;
	*blue=surface->format->palette->colors[*pixel].b;
	*alpha=0xFF;
}

inline void EDL_SetPixel_8bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	*pixel=SDL_MapRGB(surface->format,red,green,blue);
}

inline void EDL_GetPixel_16bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pixel=(Uint8*)surface->pixels+y*surface->pitch+x;
	Uint32 dc=*pixel;
	*red=(dc&surface->format->Rmask)>>surface->format->Rshift;
	*green=(dc&surface->format->Gmask)>>surface->format->Gshift;
	*blue=(dc&surface->format->Bmask)>>surface->format->Bshift;
	if(surface->format->Amask){
		*alpha=(dc&surface->format->Amask)>>surface->format->Ashift;
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_16bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint32 color=0;
	color|=(red<<surface->format->Rshift)&surface->format->Rmask;
	color|=(green<<surface->format->Gshift)&surface->format->Gmask;
	color|=(blue<<surface->format->Bshift)&surface->format->Bmask;
	color|=(alpha<<surface->format->Ashift)&surface->format->Amask;
	*((Uint16*)surface->pixels+y*surface->pitch/2+x)=color;
}

inline void EDL_GetPixel_24bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint8 *pix=(Uint8*)surface->pixels+y*surface->pitch+x*3;
	Uint8 rshift8=surface->format->Rshift/8;
	Uint8 gshift8=surface->format->Gshift/8;
	Uint8 bshift8=surface->format->Bshift/8;
	Uint8 ashift8=surface->format->Ashift/8;
	*red=*((pix)+rshift8);
	*green=*((pix)+gshift8);
	*blue=*((pix)+bshift8);
	if(surface->format->Amask){
		*alpha=*((pix)+ashift8);
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_24bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint8 *pix=(Uint8*)surface->pixels+y*surface->pitch+x*3;
	Uint32 color=blue<<16|green<<8|red;
	Uint8 rshift8=surface->format->Rshift/8;
	Uint8 gshift8=surface->format->Gshift/8;
	Uint8 bshift8=surface->format->Bshift/8;
	Uint8 ashift8=surface->format->Ashift/8;
	*(pix + rshift8) = color >> surface->format->Rshift;
	*(pix + gshift8) = color >> surface->format->Gshift;
	*(pix + bshift8) = color >> surface->format->Bshift;
	if(surface->format->Amask){
		*(pix + ashift8) = color >> surface->format->Ashift;
	}
}

inline void EDL_GetPixel_32bit(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	Uint32 *pixel=(Uint32*)surface->pixels+y*surface->pitch/4+x;
	*red=((*pixel)&surface->format->Rmask)>>surface->format->Rshift;
	*green=((*pixel)&surface->format->Gmask)>>surface->format->Gshift;
	*blue=((*pixel)&surface->format->Bmask)>>surface->format->Bshift;
	if(surface->format->Amask){
		*alpha=((*pixel)&surface->format->Amask)>>surface->format->Ashift;
	}
	else{
		*alpha=0xFF;
	}
}

inline void EDL_SetPixel_32bit(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	Uint32 *pixel=(Uint32*)surface->pixels+y*surface->pitch/4+x;
	Uint32 R=(red<<surface->format->Rshift)&surface->format->Rmask;
	Uint32 G=(green<<surface->format->Gshift)&surface->format->Gmask;
	Uint32 B=(blue<<surface->format->Bshift)&surface->format->Bmask;
	Uint32 A=(alpha<<surface->format->Ashift)&surface->format->Amask;
	*pixel=R|G|B|A;
}

inline void EDL_GetPixel_auto(SDL_Surface *surface,
		Sint16 x,Sint16 y,
		Uint8 *red,Uint8 *green,Uint8 *blue,Uint8 *alpha){
	switch (surface->format->BytesPerPixel) {
		case 1:	EDL_GetPixel_8bit(surface,x,y,red,green,blue,alpha);	break;
		case 2:	EDL_GetPixel_16bit(surface,x,y,red,green,blue,alpha);	break;
		case 3:	EDL_GetPixel_24bit(surface,x,y,red,green,blue,alpha);	break;
		case 4:	EDL_GetPixel_32bit(surface,x,y,red,green,blue,alpha);	break;
	}
}

inline void EDL_SetPixel_auto(SDL_Surface * surface,
		Sint16 x,Sint16 y,Uint8 red,Uint8 green,Uint8 blue,Uint8 alpha){
	switch (surface->format->BytesPerPixel) {
		case 1:	EDL_SetPixel_8bit(surface,x,y,red,green,blue,alpha);	break;
		case 2:	EDL_SetPixel_16bit(surface,x,y,red,green,blue,alpha);	break;
		case 3:	EDL_SetPixel_24bit(surface,x,y,red,green,blue,alpha);	break;
		case 4:	EDL_SetPixel_32bit(surface,x,y,red,green,blue,alpha);	break;
	}
}

/*! \brief Reads and maps a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \param Red Pointer to receive red color value
 *  \param Green Pointer to receive green color value
 *  \param Blue Pointer to receive blue color value
 *  \param Alpha Pointer to receive alpha channel value
 */
void EDL_GetPixel(SDL_Surface *Surface,
		Sint16 X,Sint16 Y,
		Uint8 *Red,Uint8 *Green,Uint8 *Blue,Uint8 *Alpha){
	// Lock source
	if(SDL_MUSTLOCK(Surface)){
		if(SDL_LockSurface(Surface)<0){
			return;
		}
	}

	// Read pixel
	EDL_GetPixel_auto(Surface,X,Y,Red,Green,Blue,Alpha);

	// Unlock
	if(SDL_MUSTLOCK(Surface)){
		SDL_UnlockSurface(Surface);
	}
}

/*! \brief Reads and maps rgba value of a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \return Colorvalue of given pixel (0 if failed)
 */
Uint32 EDL_GetPixel(SDL_Surface *surface,Sint16 x,Sint16 y){
	Uint32 retval=0;
	if(surface){
		Uint8 r,g,b,a;
		EDL_GetPixel(surface,x,y,&r,&g,&b,&a);
		retval=(r<<24)|(g<<16)|(b<<8)|a;
	}
	return retval;
}

/*! \brief Reads and maps a given pixel
 *  \param Surface Source surface
 *  \param X Source coordinate
 *  \param Y Source coordinate
 *  \param Red Pointer to receive red color value
 *  \param Green Pointer to receive green color value
 *  \param Blue Pointer to receive blue color value
 *  \param Alpha Pointer to receive alpha channel value
 */
void EDL_SetPixel(SDL_Surface *Surface,
		Sint16 X,Sint16 Y,
		Uint8 Red,Uint8 Green,Uint8 Blue,Uint8 Alpha){
	// Lock source
	if(SDL_MUSTLOCK(Surface)){
		if(SDL_LockSurface(Surface)<0){
			return;
		}
	}

	// Read pixel
	EDL_SetPixel_auto(Surface,X,Y,Red,Green,Blue,Alpha);

	// Unlock
	if(SDL_MUSTLOCK(Surface)){
		SDL_UnlockSurface(Surface);
	}
}

/*! \brief Saves a surface to specified file
 *  \param surface Surface to save
 *  \param Filename Filename to save as (A three digit number will be appended)
 */
void EDL_SaveSurface(SDL_Surface *surface,uString File){
	bool done=false;
	int cnt=0;
	while(!done){
		char fname[255];
		sprintf(fname,"%s-%03d.bmp",File.c_str(),cnt++);
		FILE *tf=fopen(fname,"rb");
		if(!tf){
			tf=fopen(fname,"wb");
			if(tf){
				done=true;
				fclose(tf);
				SDL_SaveBMP(surface,fname);
			}
		}
		else{
			fclose(tf);
		}
	}
}

/*! \brief Creates a new surface with alpha channel
 *  \param Width Width of new surface
 *  \param Height Height of new surface
 *  \returns New surface
 */
SDL_Surface *EDL_CreateSurface(int Width,int Height){
	if(Width>0 && Height>0){
	    //return SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA,Width,Height,32,
               SDL_Surface *g = SDL_CreateRGBSurface(SDL_SWSURFACE,Width,Height,32,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				0xff000000,0x00ff0000,0x0000ff00,0x000000ff);
#else
				0x000000ff,0x0000ff00,0x00ff0000,0xff000000);
#endif
//SDL_SetSurfaceAlphaMod(g,254);
//SDL_SetSurfaceBlendMode(g,SDL_BLENDMODE_BLEND);
return g;
	}
	else{
		return 0;
	}
}

EDLTexture *EDL_CreateTexture(int Width,int Height){
	if(Width>0 && Height>0){
            EDLTexture *g = new EDLTexture();
            g->createBlank(Width,Height);
            return g;
	}
	else{
		return 0;
	}
}

/*! \brief Generates and blits a text surface (Including alpha layer)
 *  \param Text String to render
 *  \param Color Color to render text in
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  Text size will be automatically fitted for the destination rectangle
 */
void EDL_BlitText(uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect){
	SDL_Rect drect={0,0,dst->w,dst->h};
	if(dstrect){
		drect=*dstrect;
	}
	SDL_Surface *tmp=EDL_CreateText(Text,Color,drect.w,drect.h);
	if(tmp){
		EDL_BlitSurface(tmp,0,dst,&drect);
		SDL_FreeSurface(tmp);
	}
}

/*! \brief Renders a text string and blends it into a surface
 *  \param Text String to render
 *  \param Color Color to render text in
 *  \param dst Destination surface
 *  \param dstrect Destination surface
 *
 *  Text size will be automatically fitted for the destination rectangle
 */
void EDL_BlendText(uString Text,Uint32 Color,
		SDL_Surface *dst,SDL_Rect *dstrect){
	SDL_Rect drect={0,0,dst->w,dst->h};
	if(dstrect){
		drect=*dstrect;
	}
	SDL_Surface *tmp=EDL_CreateText(Text,Color,drect.w,drect.h);
	if(tmp){
		EDL_BlendSurface(tmp,0,dst,&drect);
		SDL_FreeSurface(tmp);
	}
}

/*! \brief Calculates size of a given text
 *  \param Text String to calculate
 *  \param Size Font size (more or less the same as the height)
 *  \param Width Pointer to width variable
 *  \param Height Pointer to height variable
 *  \returns True if string can be rendered
 */
bool EDL_SizeText(uString Text,int Size,int *Width,int *Height){
	bool retval=false;
	if(Text.length()){
		uString face=Cfg::Font::default_face;
		TTF_Font *font=TTF_OpenFont(face.c_str(),Size);
		if(font){
			if(!TTF_SizeUTF8(font,Text.c_str(),Width,Height)){
				retval=true;
			}
			TTF_CloseFont(font);
		}
	}
	return retval;
}

/*! \brief Creates a surface with a text that spans multiple lines
 *  \param Text String to render (Newlines indicates linebreak)
 *  \param Color Font color
 *  \param Width Width of the resulting surface
 *  \param Height Height of the resulting surface
 *  \return Surface with rendered text
 */
SDL_Surface *EDL_CreateMultilineText(uString Text,
		Uint32 Color,int Width,int Height){
	SDL_Surface *retval=EDL_CreateSurface(Width,Height);
	std::string Txt = Text.c_str();
	unsigned int h=Cfg::Font::default_size;
	unsigned int s=0;
	unsigned int e=0;
	unsigned int i=0;
	for(;e<Txt.length();e++){
		if(Txt[e]=='\n'){
			if(e>s){
				std::string t=Txt.substr(s,e-s);
				SDL_Surface *tmp=EDL_CreateText(t,Color,Width,h);
				SDL_Rect tr={0,h*i++,Width,h};
				EDL_BlitSurface(tmp,0,retval,&tr);
				SDL_FreeSurface(tmp);
			}
			s=e+1;
		}
	}
	if(e>s){
		std::string t=Txt.substr(s,e-s);
		SDL_Surface *tmp=EDL_CreateText(t,Color,Width,h);
		SDL_Rect tr={0,h*i++,Width,h};
		EDL_BlitSurface(tmp,0,retval,&tr);
		SDL_FreeSurface(tmp);
	}
	return retval;
}

/*! \brief Creates a blendable text and automatically sizes it
 *  \param Text Text to render
 *  \param Color Color to render the text in
 *  \param Width Width of result surface
 *  \param Height Height of result surface
 *  \return blendable surface with text rendered on it
 */
SDL_Surface *EDL_CreateText(uString Text,
		Uint32 Color,int Width,int Height,bool newF ){
	// Create and configure a font object
	SDL_Surface *retval=0;
	uString face=Cfg::Font::default_face;
	int size=Cfg::Font::default_size;
	if (newF)
	{
        FILE *tf=fopen(face.c_str(),"rb");
        if(!tf){
            LogError("Failed to find font: %s",face.c_str());
        }
        else{
            fclose(tf);
        }
        if(tf && size>0 && Text.length()){
		// Load font and poll for size
            int tw,th;
            TTF_Font *font=0;
            font=TTF_OpenFont(face.c_str(),Height);
            SDL_Color color={(Color>>24)&0xFF,(Color>>16)&0xFF,Color>>8};
            retval=EDL_RenderText(font,Text,color);
			TTF_CloseFont(font);

			/*
            for(int i=4;i<Height;i++){
                font=TTF_OpenFont(face.c_str(),i);

                if(!EDL_SizeUTF8(font,Text.c_str(),&tw,&th)){
                    if(tw>Width || th>Height){
                        TTF_CloseFont(font);
                        break;
                    }
                    else{
                        size=i;
                    }
                }
            SDL_Color color={(Color>>24)&0xFF,(Color>>16)&0xFF,Color>>8};
            retval=EDL_RenderText(font,Text,color);
			TTF_CloseFont(font);
            }*/
        }
    }
    else
    {
		if(EDLfont){
			SDL_Color color={(Color>>24)&0xFF,(Color>>16)&0xFF,Color>>8};
			retval=EDL_RenderText(EDLfont,Text,color);
        }
    }
	return retval;
}

static void cp1251toUtf1(char *out, const char *in) {
    static const int table[128] = {
        0x82D0,0x83D0,0x9A80E2,0x93D1,0x9E80E2,0xA680E2,0xA080E2,0xA180E2,
        0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,
        0,0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,0x9ED1,0x88D0,0xA4C2,0x90D2,0xA6C2,0xA7C2,
        0x81D0,0xA9C2,0x84D0,0xABC2,0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,0x86D0,0x96D1,0x91D2,0xB5C2,0xB6C2,0xB7C2,
        0x91D1,0x9684E2,0x94D1,0xBBC2,0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,0x92D0,0x93D0,0x94D0,0x95D0,0x96D0,0x97D0,
        0x98D0,0x99D0,0x9AD0,0x9BD0,0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,0xA2D0,0xA3D0,0xA4D0,0xA5D0,0xA6D0,0xA7D0,
        0xA8D0,0xA9D0,0xAAD0,0xABD0,0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,0xB2D0,0xB3D0,0xB4D0,0xB5D0,0xB6D0,0xB7D0,
        0xB8D0,0xB9D0,0xBAD0,0xBBD0,0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,0x82D1,0x83D1,0x84D1,0x85D1,0x86D1,0x87D1,
        0x88D1,0x89D1,0x8AD1,0x8BD1,0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };
    while (*in)
        if (*in & 0x80) {
            int v = table[(int)(0x7f & *in++)];
            if (!v)
                continue;
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else
            *out++ = *in++;
    *out = 0;
}

std::string cp1251toUtf(std::string s) {
    int c,i;
    std::string ns;
    for(i=0; i<s.size(); i++) {
	c=s[i];
        char buf[4], in[2] = {0, 0};
        *in = c;
        cp1251toUtf1(buf, in);
        ns+=std::string(buf);
    }
   return ns;
}

SDL_Surface *EDL_RenderText(TTF_Font *Font,const uString &Text,SDL_Color fg){
#if VILE_BUILD_UNICODE
	return TTF_RenderUTF8_Blended(Font,Text.c_str(),fg);
#else
    std::string a = cp1251toUtf(Text.c_str());
    return TTF_RenderUTF8_Blended(Font,a.c_str(),fg);
   // return TTF_RenderUTF8_Blended(Font,Text.c_str(),fg);

	//return TTF_RenderText_Blended(Font,Text.c_str(),fg);
#endif
}

int EDL_SizeUTF8(TTF_Font *font, const uString &Text, int *w, int *h){
    std::string a = cp1251toUtf(Text.c_str());
    return TTF_SizeUTF8(font,a.c_str(),w,h);
}

/*! \brief Alphablends a bitmap onto a new surface using a colorkey
 *  \param Bitmap Source surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 *
 *  This lumpy thing autodetects a colorkey from the four corners.
 */
SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,SDL_Rect *Rect){
	// Autodetect colorkey
	Uint32 c1=EDL_GetPixel(Bitmap,0,0);
	Uint32 c2=EDL_GetPixel(Bitmap,Bitmap->w-1,0);
	Uint32 c3=EDL_GetPixel(Bitmap,0,Bitmap->h-1);
	Uint32 c4=EDL_GetPixel(Bitmap,Bitmap->w-1,Bitmap->h-1);
	int m1=0;
	int m2=0;
	int m3=0;
	int m4=0;
	if(c1==c2){
		m1++;
		m2++;
	}
	if(c1==c3){
		m1++;
		m3++;
	}
	if(c1==c4){
		m1++;
		m4++;
	}
	if(c2==c3){
		m2++;
		m3++;
	}
	if(c2==c4){
		m2++;
		m4++;
	}
	if(c3==c4){
		m2++;
		m3++;
	}

	if(m1>2){
		return EDL_ColorkeySurface(Bitmap,c1,Rect);
	}
	if(m2>2){
		return EDL_ColorkeySurface(Bitmap,c2,Rect);
	}
	if(m3>2){
		return EDL_ColorkeySurface(Bitmap,c3,Rect);
	}
	if(m4>2){
		return EDL_ColorkeySurface(Bitmap,c4,Rect);
	}
	if(m1>1){
		return EDL_ColorkeySurface(Bitmap,c1,Rect);
	}
	if(m2>1){
		return EDL_ColorkeySurface(Bitmap,c2,Rect);
	}
	if(m3>1){
		return EDL_ColorkeySurface(Bitmap,c3,Rect);
	}
	if(m4>1){
		return EDL_ColorkeySurface(Bitmap,c4,Rect);
	}
	return EDL_ColorkeySurface(Bitmap,c1,Rect);
}

/*! \brief Alphablends a bitmap onto a new surface using a colorkey
 *  \param Bitmap Source surface
 *  \param Key Color to filter
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_ColorkeySurface(SDL_Surface *Bitmap,
		Uint32 Key,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Bitmap){
		// Get target rect
		SDL_Rect rect={0,0,Bitmap->w,Bitmap->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Bitmap)){
			if(SDL_LockSurface(Bitmap)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel by colorkey
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 kr=(Key>>24)&0xFF,kg=(Key>>16)&0xFF,kb=(Key>>8)&0xFF;
				Uint8 br=0,bg=0,bb=0,ba=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx>=0 && dy>=0){
					if(dx<=Bitmap->w && dy<=Bitmap->h){
						EDL_GetPixel_auto(Bitmap,
								x+rect.x,y+rect.y,
								&br,&bg,&bb,&ba);
						if(br==kr && bg==kg && bb==kb){
							ba=0;
						}
						else{
							ba=0xFF;
						}
					}
					EDL_SetPixel_auto(retval,x,y,br,bg,bb,ba);
				}
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Bitmap)){
			SDL_UnlockSurface(Bitmap);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;
}



void EDL_setupImageAlpha( SDL_Surface *surface, SDL_Surface *surface_m,Uint8 a)
{
    if (surface == NULL) return;

    SDL_LockSurface( surface );
    Uint32 *buffer = (Uint32 *)surface->pixels;
    SDL_PixelFormat *fmt = surface_m->format;

    int w = surface->w;
    int h = surface->h;

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
    unsigned char *alphap = (unsigned char *)buffer + 3;
#else
    unsigned char *alphap = (unsigned char *)buffer;
#endif

    int i, j;
        if (surface_m){
            SDL_LockSurface( surface_m );
            const int mw  = surface_m->w;
            const int mwh = surface_m->w * surface_m->h;

            int i2 = 0;
            for (i=h ; i!=0 ; i--){
                Uint32 *buffer_m = (Uint32 *)surface_m->pixels + i2;
                    int j2 = 0;
                    for (j=w ; j!=0 ; j--, buffer++, alphap+=4){
                        alphap[0] = a;
                        if (((*(buffer_m+ j2)&fmt->Rmask)>>fmt->Rshift)>a)
                          alphap[0]= 0x00;
                        if (j2 >= mw) j2 = 0;
                        else          j2++;
                    }
                if (i2 >= mwh) i2 = 0;
                else           i2 += mw;
            }
            SDL_UnlockSurface( surface_m );
        }
    SDL_UnlockSurface( surface );
}

/*! \brief Alphablends a bitmap onto a new surface using a b/w mask
 *  \param Bitmap Source surface
 *  \param Mask Mask surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_MaskSurface(SDL_Surface *Bitmap,
		SDL_Surface *Mask,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Bitmap){
		// Get target rect
		SDL_Rect rect={0,0,Bitmap->w,Bitmap->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Bitmap)){
			if(SDL_LockSurface(Bitmap)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(Mask)){
			if(SDL_LockSurface(Mask)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 br=0,bg=0,bb=0,ba=0xFF;
				Uint8 mr=0,mg=0,mb=0,ma=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx>=0 && dy>=0){
					if(dx<=Bitmap->w && dy<=Bitmap->h){
						EDL_GetPixel_auto(Bitmap,
								x+rect.x,y+rect.y,
								&br,&bg,&bb,&ba);
					}
					if(dx<=Mask->w && dy<=Mask->h){
						EDL_GetPixel_auto(Mask,
								x+rect.x,y+rect.y,
								&mr,&mg,&mb,&ma);
					}
					EDL_SetPixel_auto(retval,x,y,br,bg,bb,(mr+mg+mb)/3);
				}
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Bitmap)){
			SDL_UnlockSurface(Bitmap);
		}
		if(SDL_MUSTLOCK(Mask)){
			SDL_UnlockSurface(Mask);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;

}

/*! \brief Maps the alpha channel of a surface onto a new one
 *  \param Source Source surface
 *  \param Rect Source rectangle and destination size
 *  \returns New SDL_Surface with alphachannel as graphics
 */
SDL_Surface *EDL_MapSurface(SDL_Surface *Source,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Source && Source->format->Amask){
		// Get target rect
		SDL_Rect rect={0,0,Source->w,Source->h};
		if(Rect){
			if(Rect->x<rect.w)	rect.x=Rect->x;
			if(Rect->y<rect.h)	rect.y=Rect->y;
			if(Rect->w<rect.w)	rect.w=Rect->w;
			if(Rect->h<rect.h)	rect.h=Rect->h;
		}

		// Create destination
		retval=EDL_CreateSurface(rect.w,rect.h);

		// Lock surfaces
		if(SDL_MUSTLOCK(Source)){
			if(SDL_LockSurface(Source)<0){
				return 0;
			}
		}
		if(SDL_MUSTLOCK(retval)){
			if(SDL_LockSurface(retval)<0){
				return 0;
			}
		}

		// Map alpha channel
		for(int y=0;y<rect.h;y++){
			for(int x=0;x<rect.w;x++){
				Uint8 r=0,g=0,b=0,a=0xFF;
				int dx=x+rect.x;
				int dy=y+rect.y;
				if(dx<=Source->w && dx>=0 && dy<=Source->h && dy>=0){
					EDL_GetPixel_auto(Source,x+rect.x,y+rect.y,&r,&g,&b,&a);
				}
				EDL_SetPixel_auto(retval,x,y,a,a,a,0xFF);
			}
		}

		// Unlock
		if(SDL_MUSTLOCK(Source)){
			SDL_UnlockSurface(Source);
		}
		if(SDL_MUSTLOCK(retval)){
			SDL_UnlockSurface(retval);
		}
	}
	return retval;
}

/*! \brief Creates a new surface and copies the content from the source
 *  \param Source Source surface
 *  \param Rect X and Y is used as source while W and H destinates size
 *  \return New SDL_Surface containing source graphics
 */
SDL_Surface *EDL_CopySurface(SDL_Surface *Source,SDL_Rect *Rect){
	SDL_Surface *retval=0;
	if(Source){
		// Create destination
		retval=EDL_CreateSurface(Source->w,Source->h);
		if(Rect){
			SDL_Rect srect={Rect->x,Rect->y,Source->w,Source->h};
			SDL_Rect drect={0,0,Source->w,Source->h};
			EDL_BlitSurface(Source,&srect,retval,&drect);
		}
		else{
			EDL_BlitSurface(Source,0,retval,0);
		}
	}
	return retval;
}

#ifdef VILE_FEATURE_SCALER
/*! \brief Resizes one surface to another using HQ2X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ2X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq2x once
	static bool hq2x_init=false;
	if(!hq2x_init){
		InitHQ2X();
		hq2x_init=true;
	}

LogTest("a");
	SDL_Surface *tmpsrc=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w,src->h,16,0,0,0,0);
	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			(src->w*2)+4,(src->h*2)+4,16,0,0,0,0);
	EDL_BlitSurface(src,0,tmpsrc,0);
	if(SDL_MUSTLOCK(tmpsrc)){
		SDL_LockSurface(tmpsrc);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
LogTest("b:%d",tmpsrc->pitch);
	hq2x_32((unsigned char*)tmpsrc->pixels,
			(unsigned char*)tmpdst->pixels,
			tmpsrc->w,tmpsrc->h,tmpsrc->pitch);
LogTest("c");
	if(SDL_MUSTLOCK(tmpsrc)){
		SDL_UnlockSurface(tmpsrc);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpsrc);
	SDL_FreeSurface(tmpdst);
LogTest("DONE");
}

/*! \brief Resizes one surface to another using HQ2X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ3X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq3x once
	static bool hq3x_init=false;
	if(!hq3x_init){
		InitHQ3X();
		hq3x_init=true;
	}

	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w*3,src->h*3,16,0,0,0,0);
	if(SDL_MUSTLOCK(src)){
		SDL_LockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
	hq2x_32((unsigned char*)src->pixels,
			(unsigned char*)tmpdst->pixels,
			src->w,src->h,src->w*4);
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpdst);

}

/*! \brief Resizes one surface to another using HQ4X scaling
 *  \param Src Source surface
 *  \param Dst Destination surface
 */
void EDL_ScaleHQ4X(SDL_Surface *src,SDL_Surface *dst){
	// Initialize hq4x once
	static bool hq4x_init=false;
	if(!hq4x_init){
		InitHQ4X();
		hq4x_init=true;
	}

	SDL_Surface *tmpdst=SDL_CreateRGBSurface(SDL_SWSURFACE,
			src->w*4,src->h*4,16,0,0,0,0);
	if(SDL_MUSTLOCK(src)){
		SDL_LockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_LockSurface(tmpdst);
	}
	hq4x_32((unsigned char*)src->pixels,
			(unsigned char*)tmpdst->pixels,
			src->w,src->h,src->w*4);
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
	if(SDL_MUSTLOCK(tmpdst)){
		SDL_UnlockSurface(tmpdst);
	}
	EDL_ResizeSurface(tmpdst,dst,true);
	SDL_FreeSurface(tmpdst);
}
#endif

/*! \brief Resizes one surface to another
 *  \param Src Source surface
 *  \param Dst Destination surface
 *  \param Smooth Wether to smooth the surface
 */
void EDL_ResizeSurface(SDL_Surface *Src,SDL_Surface *Dst,bool Smooth){
	if(Src->w==Dst->w && Src->h==Dst->h){
		// Blit directly
		EDL_BlitSurface(Src,0,Dst,0);
	}
	else{
		// Resized blit
		SDL_Surface *zs=zoomSurface(Src,
				Dst->w/(double)Src->w,
				Dst->h/(double)Src->h,Smooth?1:0);
		if(zs){
			EDL_BlitSurface(zs,0,Dst,0);
			SDL_FreeSurface(zs);
		}
		else{
			LogError("Resized blit failed!");
			EDL_BlitSurface(Src,0,Dst,0);
		}
	}
}

/*! \brief Blends one surface onto another
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  Please note that the alpha values will be untouched in the result surface
 */
void EDL_BlendSurface(
		SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect){
	// Assert source and destination rects
	SDL_Rect s={0,0,src->w,src->h};
	SDL_Rect d={0,0,dst->w,dst->h};
	if(srcrect){
		s=*srcrect;
	}
	if(dstrect){
		d=*dstrect;
	}

	// Limit blit to available data
	d.w=EDL_MIN(d.w,s.w);
	d.h=EDL_MIN(d.h,s.h);
	s.w=EDL_MIN(d.w,s.w);
	s.h=EDL_MIN(d.h,s.h);

	SDL_BlitSurface(src,&s,dst,&d);
	//SDL_BlitScaled(src,&s,dst,&d);
}

/*! \brief Copies one surface to another (Including alpha channel)
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param dst Destination surface
 *  \param dstrect Destination rectangle
 *
 *  This method will copy all the specified pixels from source to destination
 *  surfaces, including the alpha channel when applicable, so no form of
 *  blending will take place.
 */
void EDL_BlitSurface(
		SDL_Surface *src, SDL_Rect *srcrect,
		SDL_Surface *dst, SDL_Rect *dstrect){
	// Assert source and destination rects
	SDL_Rect s={0,0,src->w,src->h};
	SDL_Rect d={0,0,dst->w,dst->h};
	if(srcrect){
		s=*srcrect;
	}
	if(dstrect){
		d=*dstrect;
	}

	// Limit blit to available data
	d.w=EDL_MIN(d.w,s.w);
	d.h=EDL_MIN(d.h,s.h);
	s.w=EDL_MIN(d.w,s.w);
	s.h=EDL_MIN(d.h,s.h);


	// Pass call to SDL
//	Uint32 flag=src->flags&SDL_SRCALPHA;
//	if(flag){
//		src->flags&=~(SDL_SRCALPHA);
//		SDL_BlitSurface(src,&s,dst,&d);
//		src->flags|=SDL_SRCALPHA;
//	}
//	else{
	//	SDL_BlitSurface(src,&s,dst,&d);
//	}
Uint8 saved_alpha;
SDL_GetSurfaceAlphaMod(src,&saved_alpha);
SDL_SetSurfaceAlphaMod(src,255);
SDL_BlitSurface(src,&s,dst,&d);
SDL_SetSurfaceAlphaMod(src,saved_alpha);
}

/*! \brief Sets alphachannel to a fixed value
 *  \param src Source surface
 *  \param srcrect Source rectangle
 *  \param Alpha Alpha value
 */
void EDL_SetAlpha(SDL_Surface *src, SDL_Rect *srcrect,Uint8 Alpha){
	// Get best rect
	SDL_Rect srect={0,0,src->w,src->h};
	if(srcrect){
		srect.x=srcrect->x;
		srect.y=srcrect->y;
		srect.w=srcrect->w;
		srect.h=srcrect->h;
	}
	srect.x=EDL_LIMIT(srect.x,0,src->w);
	srect.y=EDL_LIMIT(srect.y,0,src->h);
	srect.w=EDL_LIMIT(srect.w,0,src->w);
	srect.h=EDL_LIMIT(srect.h,0,src->h);

	// Lock surfaces
	if(SDL_MUSTLOCK(src)){
		if(SDL_LockSurface(src)<0){
			return;
		}
	}

	// Force alpha (Assuming 32bit surface)
	srect.w+=srect.x;
	srect.h+=srect.y;
	for(int x=srect.x;x<srect.w;x++){
		for(int y=srect.y;y<srect.h;y++){
			Uint32 *pixel=(Uint32*)src->pixels+y*src->pitch/4+x;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			*pixel=((*pixel)&0xFFFFFF00)|Alpha;
#else
			*pixel=((*pixel)&0x00FFFFFF)|(Alpha<<24);
#endif
		}
	}

	// Unlock
	if(SDL_MUSTLOCK(src)){
		SDL_UnlockSurface(src);
	}
}

// ----- PC-style hover invert (CP choice highlight) -----
/*! The PC originals highlight the hovered choice row by inverting the
 *  composed strip (raster DSTINVERT): the dark dialog window turns into
 *  a light bar and the white caption turns black. SDL 2.0.3 offers no
 *  blend mode that can express 255-dst, so the queued rectangles are
 *  read back after the widget pass, inverted in software and copied
 *  back over the composed frame.
 */
static SDL_Rect invert_queue[8];
static int invert_count=0;
static int invert_flip=-1; //!< Readback orientation: -1 unknown, 0 upright, 1 flipped

/*! \brief Detects once whether target readback arrives vertically flipped
 *
 *  Some backends (GLES2 of SDL 2.0.3) store render targets bottom-up,
 *  which flips SDL_RenderReadPixels output. A tiny probe pattern is
 *  rendered into an offscreen target and inspected once per session.
 */
static bool DetectReadbackFlip(){
	bool flipped=false;
	if(EDLRenderer){
		SDL_Texture *probe=SDL_CreateTexture(EDLRenderer,
				SDL_PIXELFORMAT_ABGR8888,
				SDL_TEXTUREACCESS_TARGET,8,8);
		if(probe){
			SDL_Texture *previous=SDL_GetRenderTarget(EDLRenderer);
			if(SDL_SetRenderTarget(EDLRenderer,probe)==0){
				SDL_SetRenderDrawColor(EDLRenderer,0,0,0,255);
				SDL_RenderClear(EDLRenderer);
				boxRGBA(EDLRenderer,1,1,2,2,255,255,255,255);
				Uint32 pixels[64];
				if(SDL_RenderReadPixels(EDLRenderer,NULL,
							SDL_PIXELFORMAT_ABGR8888,
							pixels,8*sizeof(Uint32))==0){
					int first=-1;
					for(int y=0;y<8 && first<0;y++){
						for(int x=0;x<8;x++){
							if(pixels[y*8+x]==0xFFFFFFFF){
								first=y;
								break;
							}
						}
					}
					// The white probe was drawn at logical rows 1-2
					flipped=(first>=5);
				}
			}
			SDL_SetRenderTarget(EDLRenderer,previous);
			SDL_DestroyTexture(probe);
		}
	}
	return flipped;
}

/*! \brief Whether render-target readback arrives vertically flipped
 *
 *  Probed once per session; every readback consumer shares the result
 *  so target-to-surface copies keep one consistent orientation.
 */
bool EDL_ReadbackFlipped(void){
	if(invert_flip<0){
		invert_flip=DetectReadbackFlip()?1:0;
	}
	return invert_flip!=0;
}

void EDL_QueueHoverInvert(SDL_Rect Rect){
	if(invert_count<8){
		invert_queue[invert_count++]=Rect;
	}
}

void EDL_FlushHoverInverts(void){
	if(!invert_count){
		return;
	}
	if(!EDLRenderer){
		invert_count=0;
		return;
	}
	if(invert_flip<0){
		invert_flip=DetectReadbackFlip()?1:0;
	}
	for(int index=0;index<invert_count;index++){
		SDL_Rect rect=invert_queue[index];

		// Clip against the current target viewport
		SDL_Rect viewport={0,0,0,0};
		SDL_RenderGetViewport(EDLRenderer,&viewport);
		SDL_Rect out;
		if(SDL_IntersectRect(&rect,&viewport,&out)!=SDL_TRUE){
			continue;
		}
		SDL_Surface *tmp=SDL_CreateRGBSurface(SDL_SWSURFACE,
				out.w,out.h,32,
				0x000000FF,0x0000FF00,0x00FF0000,0xFF000000);
		if(!tmp){
			continue;
		}
		if(SDL_RenderReadPixels(EDLRenderer,&out,
					SDL_PIXELFORMAT_ABGR8888,
					tmp->pixels,tmp->pitch)==0){
			if(invert_flip){
				// Bottom-up storage: undo the flip before reuploading
				int bytes=tmp->pitch;
				char *row=(char*)malloc(bytes);
				if(row){
					char *lines=(char*)tmp->pixels;
					for(int a=0,b=out.h-1;a<b;a++,b--){
						memcpy(row,lines+a*bytes,bytes);
						memcpy(lines+a*bytes,lines+b*bytes,bytes);
						memcpy(lines+b*bytes,row,bytes);
					}
					free(row);
				}
			}

			// Invert the RGB channels, preserve alpha
			if(SDL_MUSTLOCK(tmp)){
				SDL_LockSurface(tmp);
			}
			for(int y=0;y<out.h;y++){
				Uint32 *pixels=(Uint32*)((char*)tmp->pixels+y*tmp->pitch);
				for(int x=0;x<out.w;x++){
					Uint8 r,g,b,a;
					SDL_GetRGBA(pixels[x],tmp->format,&r,&g,&b,&a);
					pixels[x]=SDL_MapRGBA(tmp->format,
							255-r,255-g,255-b,a);
				}
			}
			if(SDL_MUSTLOCK(tmp)){
				SDL_UnlockSurface(tmp);
			}

			// Copy the inverted strip back over the composed frame
			SDL_Texture *patch=SDL_CreateTexture(EDLRenderer,
					SDL_PIXELFORMAT_ABGR8888,
					SDL_TEXTUREACCESS_STREAMING,
					out.w,out.h);
			if(patch){
				void *pixels;
				int pitch;
				if(SDL_LockTexture(patch,NULL,&pixels,&pitch)==0){
					char *dst=(char*)pixels;
					char *src=(char*)tmp->pixels;
					for(int y=0;y<out.h;y++){
						memcpy(dst+y*pitch,src+y*tmp->pitch,
								out.w*4);
					}
					SDL_UnlockTexture(patch);
					SDL_SetTextureBlendMode(patch,
							SDL_BLENDMODE_NONE);
					SDL_RenderCopy(EDLRenderer,patch,NULL,&out);
				}
				SDL_DestroyTexture(patch);
			}
		}
		SDL_FreeSurface(tmp);
	}
	invert_count=0;
}
