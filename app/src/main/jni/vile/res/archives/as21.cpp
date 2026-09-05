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

#include "as21.h"

ArchiveS21::ArchiveS21(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(path.c_str(),"rb");
	if(rfile!=NULL){
		unsigned char b[255];
		if(fread(b,1,12,rfile)>0){
			if(b[0]==0x89 && b[1]==0xF5 && b[2]==0x8A && b[3]==0x79){
				int ilength=((b[7]<<24)|(b[6]<<16)|(b[5]<<8)|b[4])-4;
				int iread=12;
				int offset=ilength+4;
				while(ilength>iread && fread(b,1,24,rfile)>22){
					int dlength=(b[23]<<24)|(b[22]<<16)|(b[21]<<8)|b[20];
					AddItem((char*)b,offset,dlength);
					offset+=dlength;
					iread+=24;
				}
			}
		}
		fclose(rfile);
	}
}

