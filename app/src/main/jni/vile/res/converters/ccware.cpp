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

#include "ccware.h"

SDL_Surface *CCWare::lz_32b(RWops *Object){
	SDL_Surface *retval=0;
	if(Object){
		Uint8 header[10]={0};
		Object->Seek(0,SEEK_SET);
		Object->Read(header,10);
		unsigned int csize=header[5]<<24|header[4]<<16|header[3]<<8|header[2];
		unsigned int usize=header[9]<<24|header[8]<<16|header[7]<<8|header[6];
		if(header[0]=='L' && header[1]=='Z'){
			// Prepp input/output
			Uint8 outbuffer[usize];
			Uint8 inbuffer[csize];
			Object->Read(inbuffer,csize);

			// Create ringbuffer
			Uint32 bufsize=0x1000;	// Ringbuffer size.
			Uint8 buffer[bufsize];
			Uint32 mask=bufsize-1;
			Uint32 rin=0xFEE;
			for(unsigned int i=0;i<bufsize;i++){
				buffer[i]=0;
			}
			Uint32 in=0;
			Uint32 out=0;
			Uint32 ops;
			while(in<csize){
				for(ops=inbuffer[in++]|0x100;ops!=1;ops>>=1){
					if(in<csize && out+1<usize){
						if((ops&1)==0){
							// Decompress
							Uint16 l=inbuffer[in++];
							Uint16 h=inbuffer[in++];
							Uint16 data=l|(h<<8);
							Uint16 rout=(data&0xFF)|((data>>4)&0xF00);
							Uint16 count=((data>>8)&0xF)+3;
							while(count--){
								outbuffer[out++]=buffer[rin]=buffer[rout];
								rin=(rin+1)&mask;
								rout=(rout+1)&mask;
							}
						}
						else{
							// Not compressed
							buffer[rin]=outbuffer[out++]=inbuffer[in++];
							rin=(rin+1)&mask;
						}
					}
				}
			}
			SDL_RWops *src=SDL_RWFromMem(outbuffer,usize);
			SDL_Surface *tmp=SDL_LoadBMP_RW(src,true);
			retval=EDL_CreateSurface(tmp->w,tmp->w);
			SDL_BlitSurface(tmp,0,retval,0);
			SDL_FreeSurface(tmp);
		}
	}
	return retval;
}

