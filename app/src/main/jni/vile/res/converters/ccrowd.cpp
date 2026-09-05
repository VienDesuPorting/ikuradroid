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

#include "ccrowd.h"

/*! \brief Demangles and converts a CWP (Crowd games)
 *  \param Object Input data
 *  \returns Converted imagedata or NULL if failed
 */
SDL_Surface *CCrowd::cwp(RWops *Object){
	// Copy source data to memory
	SDL_Surface *retval=0;
	int srclen=Object->Seek(0,SEEK_END);
	char *dstbuf=new char[srclen+31];
	char *srcbuf=new char[srclen];
	Object->Seek(0,SEEK_SET);
	Object->Read(srcbuf,srclen);

	// Reconstruct png header
	int i=0;
	dstbuf[i++]=137;
	dstbuf[i++]='P';
	dstbuf[i++]='N';
	dstbuf[i++]='G';
	dstbuf[i++]=13;
	dstbuf[i++]=10;
	dstbuf[i++]=26;
	dstbuf[i++]=10;
	dstbuf[i++]=0;
	dstbuf[i++]=0;
	dstbuf[i++]=0;
	dstbuf[i++]=13;
	dstbuf[i++]='I';
	dstbuf[i++]='H';
	dstbuf[i++]='D';
	dstbuf[i++]='R';
	for(int j=4;j<25;j++){
		dstbuf[i++]=srcbuf[j];
	}
	dstbuf[i++]='I';
	dstbuf[i++]='D';
	dstbuf[i++]='A';
	dstbuf[i++]='T';
	for(int j=25;j<srclen;j++){
		dstbuf[i++]=srcbuf[j];
	}
	dstbuf[i++]=0;
	dstbuf[i++]=0;
	dstbuf[i++]=0;
	dstbuf[i++]='I';
	dstbuf[i++]='E';
	dstbuf[i++]='N';
	dstbuf[i++]='D';
	dstbuf[i++]=174;
	dstbuf[i++]=66;
	dstbuf[i++]=96;
	dstbuf[i++]=130;

	// Convert back to rwops and then surface
	SDL_RWops *dest=SDL_RWFromMem(dstbuf,i);
	retval=IMG_Load_RW(dest,0);
	if(retval){
		// Flip colors and reset alpha
		Uint32 tmp=retval->format->Rshift;
		retval->format->Rshift=retval->format->Bshift;
		retval->format->Bshift=tmp;
		tmp=retval->format->Rmask;
		retval->format->Rmask=retval->format->Bmask;
		retval->format->Bmask=tmp;
		EDL_SetAlpha(retval,0,255);
	}

	// Clean up buffers
	SDL_FreeRW(dest);
	delete [] srcbuf;
	delete [] dstbuf;

	return retval;
}

SDL_Surface *CCrowd::zbm(RWops *Object){
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

			// Decompress using microsofts SZDD algorithm (content is xored)
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

			// Verify bitmap content and decrypt it
			if(tbuffer[0]^0xFF=='B' && tbuffer[1]^0xFF=='M'){
				for(int i=0;i<100;i++){
					tbuffer[i]^=0xFF;
				}
/*
FILE *f=fopen("zbmdump.bmp","wb");
fwrite(tbuffer,tindex,1,f);
fclose(f);
exit(0);
*/


				SDL_RWops *dest=SDL_RWFromMem(tbuffer,tindex);
				SDL_RWseek(dest,0,SEEK_SET);
				SDL_Surface *tmp=SDL_LoadBMP_RW(dest,1);
				if(tmp){
					// Convert to native depth
					retval=EDL_CreateSurface(tmp->w,tmp->h);
					EDL_BlitSurface(tmp,0,retval,0);
					SDL_FreeSurface(tmp);
				}
			}
		}
	}
	return retval;
}

