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

#include "acwarearc2.h"

ArchiveCWareARC2::ArchiveCWareARC2(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(EDL_Realname(Path).c_str(),"rb");
	if(rfile!=NULL){
		unsigned char b[32];
		if(fread(b,1,14,rfile)>0 && !strncmp((char*)b,"arc2",4)){
			fseek(rfile,0,SEEK_END);
			unsigned int size=ftell(rfile);
			unsigned int count=b[4]|(b[5]<<8)|(b[6]<<16)|(b[7]<<24);
			unsigned int offset=b[8]|(b[9]<<8)|(b[10]<<16)|(b[11]<<24);
			fseek(rfile,offset,SEEK_SET);
			if(count>0 && count<0xFFFFFF && offset>0 && offset<size){
				//unsigned int position=0x10;
				for(unsigned int i=0;i<count;i++){
					if(fread(b,1,32,rfile)>0 && b[0]){
						if(b[0]==0){
							break;
						}
						int p=b[16]|(b[17]<<8)|(b[18]<<16)|(b[19]<<24);
						int l=b[20]|(b[21]<<8)|(b[22]<<16)|(b[23]<<24);
						//int p3=b[24]|(b[25]<<8)|(b[26]<<16)|(b[27]<<24);
						//int p4=b[28]|(b[29]<<8)|(b[30]<<16)|(b[31]<<24);
						AddItem((char*)b,p,l);
					}
					else{
						break;
					}
				}
			}
		}
		fclose(rfile);
	}
}


