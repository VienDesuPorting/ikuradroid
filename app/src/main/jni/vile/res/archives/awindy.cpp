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

#include "awindy.h"

ArchiveWindy::ArchiveWindy(uString Path) : ArchiveBase(Path){
	uString listfile=path;
	for(int i=listfile.length();i>0;i--){
		if(listfile[i-1]=='.'){
			listfile=listfile.substr(0,i);
			listfile+="LST";
			listfile=EDL_Realname(listfile);
			break;
		}
		if(listfile[i-1]=='/'){
			break;
		}
	}
	FILE *rfile=fopen(listfile.c_str(),"rb");
	if(rfile!=NULL){
		unsigned char b[16];
		if(fread(b,1,16,rfile)>0){
			if(!strncmp((char*)b,"D_Lib -02-",10)){
				uString name;
				unsigned int offset=0;
				unsigned int size=0;
				int isize=0;
				while(fread(b,1,16,rfile)>14){
					// Use this items offset to calculate previous size
					unsigned int newoffset=b[12]|b[13]<<8|b[14]<<16|b[15]<<24;
					isize=newoffset-offset;
					if(isize<0){
						size=isize*-1;
					}
					else{
						size=isize;
					}
					if(size>0){
						// Add previous item (Last one is a dummy)
						AddItem(name,offset,size);
					}

					// Prepare for next item (Last one is a dummy)
					offset=newoffset;
					name=(char*)b;
				}
			}
		}
		fclose(rfile);
	}
}

