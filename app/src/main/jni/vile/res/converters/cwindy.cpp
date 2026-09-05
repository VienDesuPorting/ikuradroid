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

#include "cwindy.h"

SDL_Surface *CWindy::bet_32b(RWops *Object){
	SDL_Surface *retval=0;
	if(Object){
		// Poke the header
		Uint8 magic[8];
		Uint8 rlen[8];
		Uint8 ctype;
		Uint8 lastchar;
		Object->Seek(0,SEEK_SET);
		Object->Read(magic,8);
		Object->Read(&ctype,1);
		Object->Read(&lastchar,1);
		Object->Read(&rlen,4);

		// Verify basic header data and ctype type
		if(magic[0]=='S' && magic[1]=='Z' && 
				magic[2]=='D' && magic[3]=='D' &&
				magic[4]==0x88 && magic[5]==0xF0 && 
				magic[6]==0x27 && magic[7]==0x33 &&
				ctype=='A'){
			// Convert values
			Uint32 reallength=rlen[3]<<24|rlen[2]<<16|rlen[1]<<8|rlen[0];
			Uint32 stringlen=0;
			Uint32 stringpos=0;

			// Prepp target
			Uint8 *tbuffer=new Uint8[reallength];
			Uint32 tindex=0;

			// Prepare table
			Uint8 table[0x1000]={0x20};
			Uint32 curtabent=0xFF0;
			Uint16 bytetype=0;

			// Decompress using microsofts SZDD algorithm
			while(tindex<reallength){
				Uint8 b;
				if(stringlen){
					b=table[stringpos];
					stringpos=(stringpos+1)&0xFFF;
					stringlen--;
				}
				else{
					if(!(bytetype&0x100)){
						if (1!=Object->Read(&b,1)){
							break;
						}
						bytetype=b|0xFF00;
					}
					if(bytetype&1){
						if (1!=Object->Read(&b,1)){
							break;
						}
					}
					else{
						Uint8 b1,b2;
						if (1!=Object->Read(&b1,1))		break;
						if (1!=Object->Read(&b2,1))		break;

						/* Format:
						 * b1 b2
						 * AB CD
						 * where CAB is the stringoffset in the table
						 * and D+3 is the len of the string
						 */
						stringpos=b1|((b2&0xf0)<<4);
						stringlen=(b2&0xf)+2;

						/* 3, but we use a  byte already below ... */
						b=table[stringpos];
						stringpos=(stringpos+1)&0xFFF;
					}
					bytetype>>=1;
				}
			
				/* store b in table */
				table[curtabent++]=b;
				curtabent&=0xFFF;
				tbuffer[tindex++]=b;
			}

			// Extract special header
			Uint32 w=tbuffer[3]<<24|tbuffer[2]<<16|tbuffer[1]<<8|tbuffer[0];
			Uint32 h=tbuffer[7]<<24|tbuffer[6]<<16|tbuffer[5]<<8|tbuffer[4];
			Uint16 d=tbuffer[9]<<8|tbuffer[8];
			tindex=10;
			if(d==24){
				// Convert result to a 32bit surface while flipping colors
				retval=EDL_CreateSurface(w,h);
				Uint32 rshift=retval->format->Rshift;
				Uint32 gshift=retval->format->Gshift;
				Uint32 bshift=retval->format->Bshift;
				Uint32 ashift=retval->format->Ashift;
				Uint32 rmask=retval->format->Rmask;
				Uint32 gmask=retval->format->Gmask;
				Uint32 bmask=retval->format->Bmask;
				Uint32 amask=retval->format->Amask;
				for (unsigned int y=h;y>0;y--){
					for (unsigned int x=0;x<w;x++) {
						Uint32 b=((~tbuffer[tindex++])<<bshift)&bmask;
						Uint32 g=((~tbuffer[tindex++])<<gshift)&gmask;
						Uint32 r=((~tbuffer[tindex++])<<rshift)&rmask;
						Uint32 a=(0xFF<<ashift)&amask;
						Uint32 *pixel=((Uint32*)(retval->pixels))+
							(((y-1)*(retval->pitch/4))+x);
						*pixel=r|g|b|a;
					}
				}
			}
			else if(d==8){
				// Drop palette
				Uint32 pindex=tindex;
				tindex+=1024;
				retval=EDL_CreateSurface(w,h);
				Uint32 rshift=retval->format->Rshift;
				Uint32 gshift=retval->format->Gshift;
				Uint32 bshift=retval->format->Bshift;
				Uint32 ashift=retval->format->Ashift;
				Uint32 rmask=retval->format->Rmask;
				Uint32 gmask=retval->format->Gmask;
				Uint32 bmask=retval->format->Bmask;
				Uint32 amask=retval->format->Amask;
				for (unsigned int y=h;y>0;y--){
					for (unsigned int x=0;x<w;x++) {
						Uint32 p=tbuffer[tindex++]*4;
						Uint32 r=tbuffer[pindex+p+0];
						Uint32 g=tbuffer[pindex+p+1];
						Uint32 b=tbuffer[pindex+p+2];
						Uint32 a=0xFF;
						b=(b<<bshift)&bmask;
						g=(g<<gshift)&gmask;
						r=(r<<rshift)&rmask;
						a=(a<<ashift)&amask;
						Uint32 *pixel=((Uint32*)(retval->pixels))+
							(((y-1)*(retval->pitch/4))+x);
						*pixel=r|g|b|a;
					}
				}
			}
			else{
				LogError("Invalid bitlength for BET: %d",d);
			}
			delete [] tbuffer;
		}
	}
	return retval;
}

