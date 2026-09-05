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

#include "acrowdsnn.h"

ArchiveCrowdSNN::ArchiveCrowdSNN(uString Path) : ArchiveBase(Path){
	uString listfile=path;
	for(int i=listfile.length();i>0;i--){
		if(listfile[i-1]=='.'){
			listfile=listfile.substr(0,i);
			listfile+="INX";
			listfile=EDL_Realname(listfile);
			break;
		}
		if(listfile[i-1]=='/'){
			break;
		}
	}
	FILE *rfile=fopen(listfile.c_str(),"rb");
	if(rfile!=NULL){
		unsigned char b[72];
		while(fread(b,1,72,rfile)>0){
			int size=b[0]|(b[1]<<8)|(b[2]<<16)|(b[3]<<24);
			uString name=(char*)b+4;
			int offset=b[68]|(b[69]<<8)|(b[70]<<16)|(b[71]<<24);
			if(name.length()){
				AddItem(name,offset,size);
			}
			else{
				break;
			}
		}
		fclose(rfile);
	}
}

