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

#include "acwaredl1.h"

ArchiveCWareDL1::ArchiveCWareDL1(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(EDL_Realname(Path).c_str(),"rb");
	if(rfile!=NULL){
		unsigned char h[14];
		if(fread(h,1,14,rfile)>0 && !strncmp((const char*)h,"DL1.0",5)){
			fseek(rfile,0,SEEK_END);
			unsigned int size=ftell(rfile);
			unsigned int count=(h[8] | h[9]<<8);
			unsigned int offset=(h[10] | h[11]<<8 | h[12]<<16 | h[13]<<24);
			fseek(rfile,offset,SEEK_SET);
			if(count>0 && count<0xFFFF && offset>0 && offset<size){
				unsigned int position=0x10;
				unsigned char buffer[16];
				for(unsigned int i=0;i<count;i++){
					if(fread(buffer,1,16,rfile)>0){
						unsigned int length=(buffer[12] | (buffer[13]<<8) | 
								(buffer[14]<<16) | (buffer[15]<<24));
						buffer[12]=0;
						AddItem((char*)buffer,position,length);
						position+=length;
					}
				}
			}
		}
		fclose(rfile);
	}
}

