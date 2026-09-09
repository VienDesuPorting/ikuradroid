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

#include "cwill.h"

/*! \brief Decodes WIPF images (First frame in multipicture files)
 *  \param Object Resource blob to decode
 *  \return Decoded surface
 */
SDL_Surface *CWill::wip(RWops *Object){
	return wip(Object,0);
}

/*! \brief Decodes collection WIPF images
 *  \param Object Resource blob to decode
 *  \return Array of decoded surfaces
 */
SDL_Surface **CWill::wipa(RWops *Object){
	SDL_Surface **retval=0;
	if(Object){
		Uint8 hdr[24];
		Object->Seek(0,SEEK_SET);
		Object->Read(hdr,8);
		unsigned int count=hdr[4]|(hdr[5]<<8);
		unsigned int bpp=hdr[6]|(hdr[7]<<8);
		if(hdr[0]=='W' && hdr[1]=='I' && hdr[2]=='P' && hdr[3]=='F' && count){
			LogVerbose("WIPF animation: %u frames at %u bpp",count,bpp);
			retval=new SDL_Surface*[count+1];
			for(unsigned int i=0;i<count;i++){
				retval[i]=wip(Object,i);
			}
			retval[count]=0;
			if(!retval[0]){
				// The first frame is essential for all callers;
				// reject the whole resource instead of crashing later
				LogError("WIPF: frame 0 failed to decode, rejecting animation (%u frames)",count);
				for(unsigned int i=0;i<count;i++){
					if(retval[i]){
						SDL_FreeSurface(retval[i]);
					}
				}
				delete [] retval;
				retval=0;
			}
		}
	}
	return retval;
}

/*! \brief Decodes indexed WIPF frames
 *  \param Object Resource blob to decode
 *  \return Decoded surface
 */
SDL_Surface *CWill::wip(RWops *Object,int Index){
	SDL_Surface *retval=0;
	if(Object){
		Uint8 hdr[24];
		Object->Seek(0,SEEK_SET);
		Object->Read(hdr,8);
		unsigned int count=hdr[4]|(hdr[5]<<8);
		unsigned int bpp=hdr[6]|(hdr[7]<<8);
		if(hdr[0]=='W' && hdr[1]=='I' && hdr[2]=='P' && hdr[3]=='F' && count){
			unsigned int palettesize=0;
			if(bpp==8){
				palettesize=256*4;
			}
			else if(bpp!=24 && bpp!=32){
				LogError("WIPF: unsupported bpp %u (frame %d)",bpp,Index);
				return retval;
			}
			unsigned int indexpos=8;
			unsigned int datapos=8+(count*24);
			for(unsigned int i=0;i<count && Object->Read(hdr,24)==24;i++){
				unsigned int w=hdr[0]|(hdr[1]<<8)|(hdr[2]<<16)|(hdr[3]<<24);
				unsigned int h=hdr[4]|(hdr[5]<<8)|(hdr[6]<<16)|(hdr[7]<<24);
				unsigned int c=hdr[20]|(hdr[21]<<8)|(hdr[22]<<16)|(hdr[23]<<24);
				unsigned int s=w*h*(bpp/8);

				// Sanity check the frame header before allocating anything.
				// Corrupt or repacked files used to overflow the LZ output
				// buffer here and take the whole process down (SIGSEGV)
				if(!w || !h || w>4096 || h>4096 || !c || c>Object->Size()){
					LogError("WIPF: bogus frame header %ux%u, %u compressed bytes, file %u bytes (frame %d of %u, bpp %u)",
							w,h,c,Object->Size(),Index,count,bpp);
					break;
				}
				LogVerbose("WIPF frame %d/%u: %ux%u, %u bytes, bpp %u",Index,count,w,h,c,bpp);

				// Reseek to datapos and roll indexes
				int curpos=datapos;
				indexpos+=24;
				datapos+=c;
				datapos+=palettesize;

				// Skip unrelated data
				if(Index!=(int)i){
					continue;
				}

				// Read palette
				Object->Seek(curpos,SEEK_SET);
				Uint8 *palette=0;
				if(bpp==8){
					palette=new Uint8[palettesize];
					Object->Read(palette,palettesize);
				}

				// Prepp input/output
				Uint8 *outbuffer=new Uint8[s];
				Uint8 *inbuffer=new Uint8[c];
				if(Object->Read(inbuffer,c)==(int)c){
					// LZ decoding (Thanks to Animed 0.6.8.141)
					Uint32 SlidWindowIndex=0x1;
					Uint32 SlidWindowLength=0xFFF;
					Uint8 SlidingWindow[SlidWindowLength+1];
					for(unsigned int j=0;j<=SlidWindowLength;j++){
						SlidingWindow[j]=0;
					}
					unsigned int Temp1=0;
					unsigned int Temp2=0;
					unsigned int in=0;
					unsigned int out=0;
					Uint32 EAX,ECX,EDI;
					Uint8 AL,DI;
					// Every write to outbuffer is bounded by s=w*h*(bpp/8).
					// A valid stream decodes to exactly s bytes; anything
					// else means corrupt or repacked data and gets logged
					// instead of overflowing the heap
					while(c>in && out<s){
						EAX=Temp1;
						EAX=EAX>>1;
						Temp1=EAX;
						if(((EAX & 0xFF00) & 0x100)==0){
							AL=inbuffer[in++];
							EAX=AL|0xFF00;
							Temp1=EAX;
						}
						if(((Temp1 & 0xFF) & 0x1)!=0){
							AL=inbuffer[in++];
							outbuffer[out++]=AL;
							SlidingWindow[SlidWindowIndex]=AL;
							SlidWindowIndex++;
							SlidWindowIndex&=SlidWindowLength;
						}
						else{
							// A match consumes two bytes of input
							if((int)c-(int)in<2){
								LogError("WIPF: truncated LZ stream (frame %d, in %u of %u)",Index,in,c);
								break;
							}
							DI=inbuffer[in++];
							AL=inbuffer[in++];
							EDI=((DI<<8)|AL)>>4;
							EAX=(AL&0xF)+2;
							Temp2=EAX;
							ECX=0;
							if(EAX>0){
								while(ECX<Temp2 && out<s){
									EAX=((ECX+EDI)&SlidWindowLength);
									AL=SlidingWindow[EAX];
									outbuffer[out++]=AL;
									SlidingWindow[SlidWindowIndex]=AL;
									SlidWindowIndex++;
									SlidWindowIndex&=SlidWindowLength;
									ECX++;
								}
							}
						}
					}
					if(out<s){
						LogError("WIPF: decoded %u of %u bytes (frame %d, %u compressed) - archive or format mismatch",
								out,s,Index,c);
					}

					// Convert planar stream to 32bit surface
					retval=EDL_CreateSurface(w,h);
					if(SDL_MUSTLOCK(retval)){
						if(SDL_LockSurface(retval)<0){
							retval=0;
							return retval;
						}
					}
					Uint8 r=0x00;
					Uint8 g=0x00;
					Uint8 b=0x00;
					Uint8 a=0xFF;
					unsigned int k=0;
					for(unsigned int l=0;l<h;l++){
						for(unsigned int m=0;m<w;m++){
							// Read planar stream
							b=outbuffer[k];
							if(bpp==8){
								r=palette[(b*4)+0];
								g=palette[(b*4)+1];
								//a=palette[(b*4)+3];
								b=palette[(b*4)+2];
							}
							if(bpp>=24){
								g=outbuffer[k+(w*h)];
								r=outbuffer[k+(w*h*2)];
							}
							if(bpp==32){
								a=outbuffer[k+(w*h*3)];
							}
							k++;

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

				// Delete palette
				delete [] inbuffer;
				delete [] outbuffer;
				if(palette){
					delete [] palette;
					palette=0;
				}

				// We got what we came for ...
				break;
			}
		}
	}
	return retval;
}

