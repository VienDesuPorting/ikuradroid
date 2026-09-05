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

#include "cikura.h"

/*! \brief Decompresses and converts an 24bit GAN animation (Ikura games)
 *  \param Object Input data
 *  \returns Array of converted imagedata or NULL if failed
 *
 *  The last element in the array will be NULL, denoting the end of the
 *  animation cycle.
 */
SDL_Surface **CIkura::gan(RWops *Object){
	// Get number of frames
	SDL_Surface **retval=0;
	unsigned char hdr[16];
	Object->Seek(12,SEEK_SET);
	Object->Read(hdr,4);
	int framecount=hdr[0]|(hdr[1]<<8)|(hdr[2]<<16)|(hdr[3]<<24);
	int width=640;
	int height=480;
	if(framecount){
		retval=new SDL_Surface*[framecount+1];
		retval[framecount]=0;
	}
	else{
		retval=0;
	}
//*
	// Iterate frames
	unsigned char *lstbuf=0;
	for(int frame=0;frame<framecount;frame++){
		// Get single frame
		Object->Seek(0x2010+(frame*16),SEEK_SET);
		Object->Read(hdr,16);
		int ref=hdr[4]|(hdr[5]<<8)|(hdr[6]<<16)|(hdr[7]<<24);
		int offset=hdr[8]|(hdr[9]<<8)|(hdr[10]<<16)|(hdr[11]<<24);
		int srclen=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
		int dstlen=width*height*3;
		unsigned char *srcbuf=new unsigned char[srclen*2];
		unsigned char *dstbuf=new unsigned char[dstlen*2];
		Object->Seek(offset,SEEK_SET);
		Object->Read(srcbuf,srclen);

		// Decompress frame data
		int ipos=0;
		int opos=0;
		Uint32 last=0xFFFFFFFF;
		Uint32 color=0;
		Uint32 cnt=0;
		Uint32 length=0;
		if(ref){
			// Extract semitransparent image (Fill with previous frame)
			int hdrlen=srcbuf[0]|(srcbuf[1]<<8)|
				(srcbuf[2]<<16)|(srcbuf[3]<<24);
			int hpos=4;
			ipos=hdrlen;
			while(ipos<srclen && opos<dstlen){
				length=srcbuf[hpos++];
				if(length&0x80){
					length&=0x7F;
					length<<=8;
					length|=srcbuf[hpos++];
					if(length==0x7FFF){
						length=srcbuf[hpos+0]|(srcbuf[hpos+1]<<8)|
							(srcbuf[hpos+2]<<16)|(srcbuf[hpos+3]<<24);
						hpos+=4;
					}
				}
				while(length--){
					if(!cnt){
						color=srcbuf[ipos+0]|(srcbuf[ipos+1]<<8)|
							(srcbuf[ipos+2]<<16);
						dstbuf[opos++]=srcbuf[ipos++];
						dstbuf[opos++]=srcbuf[ipos++];
						dstbuf[opos++]=srcbuf[ipos++];
						if(last==color){
							cnt=srcbuf[ipos++];
							if(cnt&0x80){
								cnt&=0x7F;
								cnt<<=8;
								cnt|=srcbuf[ipos++];
								if(cnt==0x7FFF){
									cnt=srcbuf[ipos+0]|
										(srcbuf[ipos+1]<<8)|
										(srcbuf[ipos+2]<<16)|
										(srcbuf[ipos+3]<<24);
									ipos+=4;
								}
							}
							if(cnt>2){
								cnt-=2;
							}
							else{
								cnt=0;
							}
						}
						last=color;
					}
					else{
						cnt--;
						dstbuf[opos++]=last&0xFF;
						dstbuf[opos++]=(last>>8)&0xFF;
						dstbuf[opos++]=(last>>16)&0xFF;
					}
				}
				length=srcbuf[hpos++];
				if(length&0x80){
					length&=0x7F;
					length<<=8;
					length+=srcbuf[hpos++];
					if(length==0x7FFF){
						length=srcbuf[hpos+0]|(srcbuf[hpos+1]<<8)|
							(srcbuf[hpos+2]<<16)|(srcbuf[hpos+3]<<24);
						hpos+=4;
					}
				}
				while(lstbuf && length--){
					// Copy data from previous buffer
					int tpos=opos;
					dstbuf[tpos++]=lstbuf[opos++];
					dstbuf[tpos++]=lstbuf[opos++];
					dstbuf[tpos++]=lstbuf[opos++];
				}
			}
		}
		else{
			// Extract solid frame
			while(ipos<srclen && opos<dstlen){
				color=srcbuf[ipos+0]|(srcbuf[ipos+1]<<8)|
					(srcbuf[ipos+2]<<16);
				dstbuf[opos++]=srcbuf[ipos++];
				dstbuf[opos++]=srcbuf[ipos++];
				dstbuf[opos++]=srcbuf[ipos++];
				if(color==last){
					cnt=srcbuf[ipos++];
					if(cnt&0x80){
						cnt&=0x7F;
						cnt<<=8;
						cnt|=srcbuf[ipos++];
						if(cnt==0x7FFF){
							cnt=srcbuf[ipos+0]|(srcbuf[ipos+1]<<8)|
								(srcbuf[ipos+2]<<16)|(srcbuf[ipos+3]<<24);
							ipos+=4;
						}
					}
					if(cnt>2){
						cnt=cnt-2;
						while(cnt--){
							dstbuf[opos+0]=dstbuf[opos-3];
							dstbuf[opos+1]=dstbuf[opos-2];
							dstbuf[opos+2]=dstbuf[opos-1];
							opos+=3;
						}
					}
				}
				last=color;
			}
		}
		// Convert result to a 24bit surface
		retval[frame]=SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,24,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				0xff000000,0x00ff0000,0x0000ff00,0x00000000);
#else
				0x000000ff,0x0000ff00,0x00ff0000,0x00000000);
#endif

		opos=0;
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				Uint8 *pixels=static_cast<Uint8*>
					(retval[frame]->pixels)+
					(y*retval[frame]->pitch)+
					(x*retval[frame]->format->BytesPerPixel);
				pixels[2]=dstbuf[opos++];
				pixels[1]=dstbuf[opos++];
				pixels[0]=dstbuf[opos++];
			}
		}

		// Clean up buffers
		if(lstbuf){
			delete [] lstbuf;
		}
		lstbuf=dstbuf;
		delete [] srcbuf;
	}
	if(lstbuf){
		delete [] lstbuf;
	}
	//*/

	return retval;
}


/*! \brief Decompresses and converts an 8bit GGD (Ikura games)
 *  \param Object Input data
 *  \returns Converted imagedata or NULL if failed
 *
 *  These images are used as hitmaps in newer ikura games as well as all
 *  opaque graphics found in the nocturnal illusion remake.
 */
SDL_Surface *CIkura::ggd_8b(RWops *Object){
	// Extract graphic attributes
	SDL_Surface *retval=0;
	unsigned char hdr[16];
	Object->Seek(0,SEEK_SET);
	Object->Read(hdr,16);
	int depth=8;
	int width=hdr[8]|(hdr[9]<<8)|(hdr[10]<<16)|(hdr[11]<<24);
	int height=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
	if(height<0){
		height=height*-1;
	}
	if(depth>0 && width>0 && height>0){
		// Copy source data to memory
		int srclen=Object->Seek(0,SEEK_END);
		int dstlen=(width+1)*(height+1);
		unsigned char *dstbuf=new unsigned char[dstlen];
		unsigned char *srcbuf=new unsigned char[srclen];
		Object->Seek(0,SEEK_SET);
		Object->Read(srcbuf,srclen);

		// Set input to 16b header+28b unknown
		int ipos=16+28;
		int opos=0;

		// Extract palette
		Uint32 palette[256];
		for(int i=0;i<256;i++){
			palette[i]=
				(srcbuf[ipos+(i*4)+0])|
				(srcbuf[ipos+(i*4)+1]<<8)|
				(srcbuf[ipos+(i*4)+2]<<16)|
				(srcbuf[ipos+(i*4)+3]<<24);
		}

		// Decompress imagedata
		ipos+=256*4;
		ipos+=4;

		int p=0;
		while(ipos<srclen && opos<dstlen){
			unsigned char ctrl=srcbuf[ipos++];
			for(int i=0;i<8;i++){
				if(ipos>=srclen){
					// Pragmatic hack to fix valgrind detected error
					break;
				}
				else if(ctrl&1){
					dstbuf[opos++]=srcbuf[ipos++];
					p++;
				}
				else{
					Uint32 backword=srcbuf[ipos];
					backword|=(srcbuf[ipos+1]&0xF0)<<4;
					backword=(p-backword-19)&0x0FFF;
					backword++;
					int leng=(srcbuf[ipos+1]&0x0F)+3;
					ipos+=2;
					int zero=backword-p;
					p+=leng;
					if(zero>0){
						if(leng<zero){
							zero=leng;
						}
						leng-=zero;
						for(int j=0;j<zero;j++){
							dstbuf[opos++]=0;
						}
					}
					while(leng--){
						dstbuf[opos]=dstbuf[opos-backword];
						opos++;
					}
				}
				ctrl>>=1;
			}
		}


		// Convert result to a 24bit surface
		retval=EDL_CreateSurface(width,height);
		opos=0;
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				Uint8 *pixels = static_cast<Uint8*>(retval->pixels)+
					(y*retval->pitch)+(x*retval->format->BytesPerPixel);
				Uint8 pixel=dstbuf[opos++];
				Uint8 r=palette[pixel]&0xFF;
				Uint8 g=(palette[pixel]>>8)&0xFF;
				Uint8 b=(palette[pixel]>>16)&0xFF;
				//Uint8 a=(palette[pixel]>>24)&0xFF;
				pixels[0]=b;
				pixels[1]=g;
				pixels[2]=r;
				pixels[3]=0xFF;
			}
		}

		// Clean up buffers
		delete [] srcbuf;
		delete [] dstbuf;
	}
	else{
		LogError("Invalid GGD Header");
	}
	return retval;
}

/*! \brief Decrypts 32bit GGP (Ikura games) and load PNG image data
 *  \param Object Input data
 *  \returns Converted imagedata or NULL if failed
 *
 *  This code is based upon AnimED by WinKillerStudio (http://wks.arai-kibou.ru/)
 */

SDL_Surface *CIkura::ggp_32b(RWops *Object){
	SDL_Surface *retval=0;
	unsigned char hdr[36];
	Object->Seek(0,SEEK_SET);
	Object->Read(hdr,36);
	//Uint32 bitdepth=hdr[8]|(hdr[9]<<8)|(hdr[10]<<16)|(hdr[11]<<24);
	//Uint32 offset=hdr[20]|(hdr[21]<<8)|(hdr[22]<<16)|(hdr[23]<<24);
	Uint32 length=hdr[24]|(hdr[25]<<8)|(hdr[26]<<16)|(hdr[27]<<24);
	//Uint32 regoffset=hdr[28]|(hdr[29]<<8)|(hdr[30]<<16)|(hdr[31]<<24);
	Uint32 reglength=hdr[32]|(hdr[33]<<8)|(hdr[34]<<16)|(hdr[35]<<24);
	if(!strncmp("GGPFAIKE",(char*)hdr,8) && length>0){
		// Decrypt imagedata
		unsigned char buffer[length];
		Object->Seek(36+reglength,SEEK_SET);
		Object->Read(buffer,length);
		for(unsigned int i=0;i<length;i++){
			buffer[i]=buffer[i]^hdr[i%8]^hdr[12+(i%8)];
		}

		// Convert to sdl surface
		SDL_RWops *op=SDL_RWFromMem(buffer,length);
		retval=IMG_LoadPNG_RW(op);
		SDL_RWclose(op);
	}
	return retval;
}

/*! \brief Decompresses and converts an 32bit GGA with Alpha (Ikura games)
 *  \param Object Input data
 *  \returns Converted imagedata or NULL if failed
 *
 *  This code is based upon AnimED by WinKillerStudio (http://wks.arai-kibou.ru/)
 *
 *  These images are used for transparent cg in ikura games
 */
SDL_Surface *CIkura::gga_32b(RWops *Object){
	SDL_Surface *retval=0;
	unsigned char hdr[24];
	Object->Seek(0,SEEK_SET);
	Object->Read(hdr,24);
	Uint16 width=hdr[8]|hdr[9]<<8;
	Uint16 height=hdr[10]|hdr[11]<<8;
	//Uint32 field_c=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
	Uint32 headersize=hdr[16]|(hdr[17]<<8)|(hdr[18]<<16)|(hdr[19]<<24);
	Uint32 cryptlength=hdr[20]|(hdr[21]<<8)|(hdr[22]<<16)|(hdr[23]<<24);
	if(!strncmp("GGA00000",(char*)hdr,8) && width>0 && height>0){
		// Copy source data to memory
		int srclen=Object->Seek(0,SEEK_END);
		int dstlen=width*height*4;
		unsigned char *dstbuf=new unsigned char[dstlen];
		unsigned char *srcbuf=new unsigned char[srclen];
		Object->Seek(0,SEEK_SET);
		Object->Read(srcbuf,srclen);

		// Note on per-pixel alpha gga's
		//
		// GGA is a per-pixel format, but these are sometimes all set to
		// opaque, awaiting a per-surface blend which SDL cannot do when
		// the per-pixel values are set. Hence we make a opaque surface if
		// all values are set to opaque.
		bool opaque_ff=true;
		bool opaque_00=true;

		// Decode data
		Uint8 ctrl;
		unsigned char arr[4];
		int tpos1,tpos2,opos=0,ipos=headersize;
		while(ipos-headersize<cryptlength){
			ctrl=srcbuf[ipos++];
			if(ctrl==0x00){
				opos-=4;
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				Uint8 len=srcbuf[ipos++];
				for(int j=len;j>0;j--){
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x01){
				opos-=4;
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				Uint8 h=srcbuf[ipos++];
				Uint8 l=srcbuf[ipos++];
				Uint16 len=(l<<8)|h;
				for(int j=len;j>0;j--){
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x02){
				tpos1=opos;
				Uint16 i=srcbuf[ipos++];
				opos=opos-(i<<2);
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else if(ctrl==0x03){
				Uint8 h=srcbuf[ipos++];
				Uint8 l=srcbuf[ipos++];
				Uint32 i=(l<<8)|h;
				tpos1=opos;
				opos=opos-(i<<2);
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else if(ctrl==0x04){
				Uint8 l=srcbuf[ipos++];
				tpos2=opos-(l<<2);
				Uint8 len=srcbuf[ipos++];
				for(int j=len;j>0;j--){
					tpos1=opos;
					opos=tpos2;
					tpos2+=4;
					arr[0]=dstbuf[opos++];
					arr[1]=dstbuf[opos++];
					arr[2]=dstbuf[opos++];
					arr[3]=dstbuf[opos++];
					opos=tpos1;
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x05){
				Uint8 l=srcbuf[ipos++];
				tpos2=opos-(l<<2);
				Uint8 h=srcbuf[ipos++];
				l=srcbuf[ipos++];
				Uint16 len=(l<<8)|h;
				for(int j=len;j>0;j--){
					tpos1=opos;
					opos=tpos2;
					tpos2+=4;
					arr[0]=dstbuf[opos++];
					arr[1]=dstbuf[opos++];
					arr[2]=dstbuf[opos++];
					arr[3]=dstbuf[opos++];
					opos=tpos1;
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x06){
				Uint8 h=srcbuf[ipos++];
				Uint8 l=srcbuf[ipos++];
				Uint32 i=(l<<8)|h;
				tpos2=opos-(i<<2);
				Uint8 len=srcbuf[ipos++];
				for(int j=len;j>0;j--){
					tpos1=opos;
					opos=tpos2;
					tpos2+=4;
					arr[0]=dstbuf[opos++];
					arr[1]=dstbuf[opos++];
					arr[2]=dstbuf[opos++];
					arr[3]=dstbuf[opos++];
					opos=tpos1;
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x07){
				Uint8 h=srcbuf[ipos++];
				Uint8 l=srcbuf[ipos++];
				Uint32 i=(l<<8)|h;
				tpos2=opos-(i<<2);
				h=srcbuf[ipos++];
				l=srcbuf[ipos++];
				i=(l<<8)|h;
				for(int j=i;j>0;j--){
					tpos1=opos;
					opos=tpos2;
					tpos2+=4;
					arr[0]=dstbuf[opos++];
					arr[1]=dstbuf[opos++];
					arr[2]=dstbuf[opos++];
					arr[3]=dstbuf[opos++];
					opos=tpos1;
					dstbuf[opos++]=arr[0];
					dstbuf[opos++]=arr[1];
					dstbuf[opos++]=arr[2];
					dstbuf[opos++]=arr[3];
				}
			}
			else if(ctrl==0x08){
				tpos1=opos;
				opos-=4;
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else if(ctrl==0x09){
				tpos1=opos;
				opos-=(width*4);
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else if(ctrl==0x0A){
				tpos1=opos;
				opos-=((width*4)+4);
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else if(ctrl==0x0B){
				tpos1=opos;
				opos-=((width*4)-4);
				arr[0]=dstbuf[opos++];
				arr[1]=dstbuf[opos++];
				arr[2]=dstbuf[opos++];
				arr[3]=dstbuf[opos++];
				opos=tpos1;
				dstbuf[opos++]=arr[0];
				dstbuf[opos++]=arr[1];
				dstbuf[opos++]=arr[2];
				dstbuf[opos++]=arr[3];
			}
			else{
				Uint8 len=ctrl-11;
				for(int j=len;j>0;j--){
					dstbuf[opos++]=srcbuf[ipos++];
					dstbuf[opos++]=srcbuf[ipos++];
					dstbuf[opos++]=srcbuf[ipos++];
					dstbuf[opos++]=srcbuf[ipos++];
					opaque_ff&=(srcbuf[ipos-1]==0xFF);
					opaque_00&=(srcbuf[ipos-1]==0x00);
				}
			}
		}

		// Convert result to a 32bit surface while flipping colors
		retval=EDL_CreateSurface(width,height);
		opos=0;
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				Uint32 rshift=retval->format->Rshift;
				Uint32 gshift=retval->format->Gshift;
				Uint32 bshift=retval->format->Bshift;
				Uint32 ashift=retval->format->Ashift;
				Uint32 rmask=retval->format->Rmask;
				Uint32 gmask=retval->format->Gmask;
				Uint32 bmask=retval->format->Bmask;
				Uint32 amask=retval->format->Amask;
				Uint32 b=(dstbuf[opos++]<<bshift)&bmask;
				Uint32 g=(dstbuf[opos++]<<gshift)&gmask;
				Uint32 r=(dstbuf[opos++]<<rshift)&rmask;
				Uint32 a=(dstbuf[opos++]<<ashift)&amask;
				if(opaque_00){
					// Set full opaque as default rather than full alpha
					a=(0xFF<<ashift)&amask;
				}
				Uint32 *pixel=((Uint32*)(retval->pixels))+
					((y*(retval->pitch/4))+x);
				*pixel=r|g|b|a;
			}
		}

		// Clean up buffers
		delete [] srcbuf;
		delete [] dstbuf;
	}
	else{
		LogError("Invalid GGA Header");
	}
	return retval;
}

/*! \brief Decompresses and converts an 24bit GGD (Ikura games)
 *  \param Object Input data
 *  \returns Converted imagedata or NULL if failed
 *
 *  These images are used for opaque cg in ikura games
 */
SDL_Surface *CIkura::ggd_24b(RWops *Object){
	// Extract graphic attributes
	SDL_Surface *retval=0;
	unsigned char hdr[8];
	Object->Seek(0,SEEK_SET);
	Object->Read(hdr,8);
	int width=hdr[4]|hdr[5]<<8;
	int height=hdr[6]|hdr[7]<<8;
	int depth=24;
	if(width>0 && height>0){
		// Copy source data to memory
		int srclen=Object->Seek(0,SEEK_END);
		int dstlen=width*height*(depth/3);
		unsigned char *dstbuf=new unsigned char[dstlen];
		unsigned char *srcbuf=new unsigned char[srclen];
		Object->Seek(0,SEEK_SET);
		Object->Read(srcbuf,srclen);

		// Decompress input data
		int o=0;
		unsigned char buf[8];
		for(int i=8;i<srclen && o<dstlen;){
			unsigned char ctrl=srcbuf[i++];
			if(ctrl==0){
				buf[0]=srcbuf[i++];
				o-=3;
				buf[1]=dstbuf[o++];
				buf[2]=dstbuf[o++];
				buf[3]=dstbuf[o++];
				for(int j=0;j<buf[0];j++){
					dstbuf[o++]=buf[1];
					dstbuf[o++]=buf[2];
					dstbuf[o++]=buf[3];
				}
			}
			else if(ctrl==1){
				buf[0]=srcbuf[i++];
				buf[1]=srcbuf[i++];
				int tpos=o-buf[1]*3;
				for(int j=0;j<buf[0];j++){
					buf[2]=dstbuf[tpos++];
					buf[3]=dstbuf[tpos++];
					buf[4]=dstbuf[tpos++];
					dstbuf[o++]=buf[2];
					dstbuf[o++]=buf[3];
					dstbuf[o++]=buf[4];
				}
			}
			else if(ctrl==2){
				buf[0]=srcbuf[i++];
				buf[1]=srcbuf[i++];
				buf[2]=srcbuf[i++];
				int tpos=o-(((buf[2]<<8)|buf[1])*3);
				for(int j=0;j<buf[0];j++){
					buf[3]=dstbuf[tpos++];
					buf[4]=dstbuf[tpos++];
					buf[5]=dstbuf[tpos++];
					dstbuf[o++]=buf[3];
					dstbuf[o++]=buf[4];
					dstbuf[o++]=buf[5];
				}
			}
			else if(ctrl==3){
				buf[0]=srcbuf[i++];
				int tpos=o-(buf[0]*3);
				buf[1]=dstbuf[tpos++];
				buf[2]=dstbuf[tpos++];
				buf[3]=dstbuf[tpos++];
				dstbuf[o++]=buf[1];
				dstbuf[o++]=buf[2];
				dstbuf[o++]=buf[3];
			}
			else if(ctrl==4){
				buf[0]=srcbuf[i++];
				buf[1]=srcbuf[i++];
				int tpos=o-(((buf[1]<<8)|buf[0])*3);
				buf[2]=dstbuf[tpos++];
				buf[3]=dstbuf[tpos++];
				buf[4]=dstbuf[tpos++];
				dstbuf[o++]=buf[2];
				dstbuf[o++]=buf[3];
				dstbuf[o++]=buf[4];
			}
			else{
				for(int j=0;j<(ctrl-4);j++){
					buf[0]=srcbuf[i++];
					buf[1]=srcbuf[i++];
					buf[2]=srcbuf[i++];
					dstbuf[o++]=buf[0];
					dstbuf[o++]=buf[1];
					dstbuf[o++]=buf[2];
				}
			}
		}

		// Convert result to a 24bit surface while flipping colors
		retval=EDL_CreateSurface(width,height);
		o=0;
		for (int y=0;y<height;y++) {
			for (int x=0;x<width;x++) {
				Uint8 *pixels = static_cast<Uint8*>(retval->pixels)+
					(y*retval->pitch)+(x*retval->format->BytesPerPixel);
				pixels[3]=0xFF;
				pixels[2]=dstbuf[o++];
				pixels[1]=dstbuf[o++];
				pixels[0]=dstbuf[o++];
			}
		}

		// Clean up buffers
		delete [] srcbuf;
		delete [] dstbuf;
	}
	else{
		LogError("Invalid GGD Header");
	}
	return retval;
}

