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

#include "awillarc.h"

ArchiveWillARC::ArchiveWillARC(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(path.c_str(),"rb");
	if(rfile!=NULL){
		fseek(rfile,0,SEEK_END);
		unsigned int size=ftell(rfile);
		unsigned char b[18];
		fseek(rfile,0,SEEK_SET);
		if(size && fread(b,1,4,rfile)>0){
			// Do a verification run to compensate for missing magic header
			unsigned int types=b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24);
			unsigned int offset=0;
			bool ok=(types>0);
			for(unsigned int i=0;ok && i<types;i++){
				if(fread(b,1,12,rfile)==12 && b[3]==0){
					unsigned int start=b[8]|(b[9]<<8)|(b[10]<<16)|(b[11]<<24);
					ok&=start>offset;
					ok&=start<size;
					offset=start;
				}
				else{
					ok=false;
				}
			}

			// Parse index if ok
			for(unsigned int i=0;ok && i<types;i++){
				fseek(rfile,4+(12*i),SEEK_SET);
				if(fread(b,1,12,rfile)==12 && b[3]==0){
					unsigned int items=b[4]|(b[5]<<8)|(b[6]<<16)|(b[7]<<24);
					unsigned int start=b[8]|(b[9]<<8)|(b[10]<<16)|(b[11]<<24);
					uString type=(char*)b;
					for(unsigned int j=0;j<items;j++){
						fseek(rfile,start+(17*j),SEEK_SET);
						if(fread(b,1,17,rfile)==17 && b[8]==0){
							unsigned int size=b[9]|(b[10]<<8)|
								(b[11]<<16)|(b[12]<<24);
							unsigned int offset=b[13]|(b[14]<<8)|
								(b[15]<<16)|(b[16]<<24);
							uString name=(char*)b;
							name+=".";
							name+=type;
							AddItem(name,offset,size);
						}
					}
				}
			}
		}
		fclose(rfile);
	}
}

