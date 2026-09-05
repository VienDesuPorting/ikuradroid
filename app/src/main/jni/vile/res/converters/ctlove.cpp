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

#include "ctlove.h"

SDL_Surface *CTLove::mrs(RWops *Object){
	SDL_Surface *retval=0;
	if(Object){
		Uint8 hdr[12];
		Object->Seek(0,SEEK_SET);
		Object->Read(hdr,12);
		Uint16 w=hdr[4]|(hdr[5]<<8);
		Uint16 h=hdr[6]|(hdr[7]<<8);
		bool test1=!hdr[8] && !hdr[9] && !hdr[10] && !hdr[11];
		bool test2=hdr[1]=='D' || hdr[1]=='O';
		if(hdr[0]==0x43 && hdr[2]==0 && hdr[3]==0 && test1 && test2){
			// Parse palette
			Uint16 colors=hdr[1]=='D'?16:256;
			Uint8 palette[0x300];
			if(Object->Read(palette,0x300)==0x300){
				// Parse data
				int c=Object->Size()-Object->Tell();
				if(c>0){
					Uint8 inbuffer[c];
					if(Object->Read(inbuffer,c)==c){
						// Decode input
						unsigned int s=w*h;
						Uint8 outbuffer[s];
						Uint8 *dst=outbuffer;
						Uint8 *dste=outbuffer+s;
						Uint8 *src=inbuffer;
						Uint8 *srce=inbuffer+c;
						while((dst<dste) && (src<srce)){
							int tmp=*src++;
							if(tmp&0x80){
								// LZ
								tmp&=~0x80;
								int offset=((tmp&0xF)<<8)|*src++;
								tmp=(tmp>>4)&0xF;
								if(tmp){
									tmp+=2;
								}
								else{
									tmp=(*src++)+0x0A;
								}
								if(dst+tmp>dste){
									tmp=dste-dst;
								}
								while(tmp--){
									Uint8 byte=dst[-offset-1];
									*dst++=byte;
								}
							}
							else if(tmp&0x40){
								// RLE
								tmp&=~0x40;
								if(!tmp){
									tmp=(*src++)+0x40;
								}
								tmp++;
								if(dst+tmp>dste){
									tmp=dste-dst;
								}
								memset(dst,dst[-1],tmp);
								dst+=tmp;
							}
							else{
								// Uncompressed
								if(!tmp){
									tmp=(*src++)+0x40;
								}
								if(dst+tmp>dste){
									tmp=dste-dst;
								}
								while(tmp--){
									*dst++=*src++;
								}
							}
						}

						// Convert to surface
						retval=EDL_CreateSurface(w,h);
						if(SDL_MUSTLOCK(retval)){
							if(SDL_LockSurface(retval)<0){
								return 0;
							}
						}
						Uint8 r=0x00;
						Uint8 g=0x00;
						Uint8 b=0x00;
						Uint8 a=0xFF;
						unsigned int k=0;
						for(unsigned int l=0;l<h;l++){
							for(unsigned int m=0;m<w;m++){
								if(colors==16){
									Uint8 p=outbuffer[k++]*3;
									g=palette[p+2];
									r=palette[p+1];
									b=palette[p+0];
									if(p+2>0x300){
									LogTest("16:%d:%d:%d:%d",p,r,g,b);
									}
								}
								if(colors==256){
									LogError("256b MSR decoding not supported");
									SDL_FreeSurface(retval);
									return 0;
								}


								// Set bitmap pixel
								Uint8 *pixels=static_cast<Uint8*>
									(retval->pixels)+ 
									(l*retval->pitch)+
									(m*retval->format->BytesPerPixel);
								pixels[0]=r;
								pixels[1]=g;
								pixels[2]=b;
								pixels[3]=a;
							}
						}
						if(SDL_MUSTLOCK(retval)){
							SDL_UnlockSurface(retval);
						}
					}
				}
			}
		}
	}
	return retval;
}

