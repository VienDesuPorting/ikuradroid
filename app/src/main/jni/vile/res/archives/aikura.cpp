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

#include "aikura.h"

ArchiveIkura::ArchiveIkura(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(EDL_Realname(Path).c_str(),"rb");

	if(rfile!=NULL){
		unsigned char hdr[32];
		if(fread(hdr,1,32,rfile)>0){
			if(!strncmp((char*)hdr,"SM2MPX10",8)){
				// Moder kind of cabinet with a header!
				int dlen=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
				int dstart=hdr[28]|(hdr[29]<<8)|(hdr[30]<<16)|(hdr[31]<<24);
				fseek(rfile,0,SEEK_SET);
				unsigned char *dbuf=new unsigned char[dlen];
				if(fread(dbuf,1,dlen,rfile)>0){
					for(int i=dstart;i<dlen;i+=20){
						// Create new entry
						int start=dbuf[i+12]|(dbuf[i+13]<<8)|
							(dbuf[i+14]<<16)|(dbuf[i+15]<<24);
						int size=dbuf[i+16]|(dbuf[i+17]<<8)|
							(dbuf[i+18]<<16)|(dbuf[i+19]<<24);
						char backup=dbuf[i+12];
						dbuf[i+12]=0;
						AddItem((char*)dbuf+i,start,size);
						dbuf[i+12]=backup;
					}
				}
				delete [] dbuf;
			}
			else if(hdr[1] && hdr[1]==hdr[15] && !hdr[16] && !hdr[17]){
				// This cabinet doesnt have a magic header. Attempt decoding.
				int td1=0;
				for(int i=2;i<0x0D;i++){
					if(hdr[i]=='.'){
						td1=i;
						break;
					}
				}
				int td2=0;
				for(int i=12;i<0x1D;i++){
					if(hdr[i]=='.'){
						td2=i;
						break;
					}
				}
				Uint16 ti1=hdr[0]|(hdr[1]<<8);
				Uint16 ti2=hdr[0xE]|(hdr[0xF]<<8)|
					(hdr[0x10]<<16)|(hdr[0x11]<<24);
				if(td1>1 && td2>td1 && ti1>0 && ti2>ti1){
					// Go ahead with decoding
					int dlen=ti1;
					int dstart=2;
					fseek(rfile,0,SEEK_END);
					int fullsize=ftell(rfile);
					fseek(rfile,0,SEEK_SET);
					unsigned char *dbuf=new unsigned char[dlen];
					if(fread(dbuf,1,dlen,rfile)>0){
						// Get consequtive entries
						for(int i=dstart;i+16<dlen && dbuf[i+4];i+=16){
							// Extract offsets
							unsigned int start=dbuf[i+12]|(dbuf[i+13]<<8)|
								(dbuf[i+14]<<16)|(dbuf[i+15]<<24);
							unsigned int end=0;
							if(i+31<dlen){
								end=dbuf[i+28]|(dbuf[i+29]<<8)|
									(dbuf[i+30]<<16)|(dbuf[i+31]<<24);
							}
							char tbuf[14]={0};
							for(int j=0;j<12;j++){
									tbuf[j]=dbuf[i+j];
							}

							// Doublecheck for ending and add item
							if(end>start){
								AddItem(tbuf,start,end-start);
							}
							else{
								AddItem(tbuf,start,fullsize);
								break;
							}
						}
					}
					delete [] dbuf;
				}
			}
		}
		fclose(rfile);
	}
}

