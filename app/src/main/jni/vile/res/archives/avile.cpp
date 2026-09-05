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

#include "avile.h"

ArchiveViLE::ArchiveViLE(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(path.c_str(),"rb");
	if(rfile!=NULL){
		unsigned char hdr[32];
		if(fread(hdr,1,32,rfile)>0){
			if(!strncmp((char*)hdr,"ViLEPACK",8)){
				// Moder kind of cabinet with a header!
				int dlen=hdr[12]|(hdr[13]<<8)|(hdr[14]<<16)|(hdr[15]<<24);
				fseek(rfile,0,SEEK_SET);
				unsigned char *dbuf=new unsigned char[dlen];
				if(fread(dbuf,1,dlen,rfile)>0){
					for(int i=16;i<dlen;i+=32){
						// Create new entry
						int start=dbuf[i+24]|(dbuf[i+25]<<8)|
							(dbuf[i+26]<<16)|(dbuf[i+27]<<24);
						int size=dbuf[i+28]|(dbuf[i+29]<<8)|
							(dbuf[i+30]<<16)|(dbuf[i+31]<<24);
						char backup=dbuf[i+24];
						dbuf[i+24]=0;
						AddItem((char*)dbuf+i,start,size);
						dbuf[i+24]=backup;
					}
				}
				delete [] dbuf;
			}
		}
		fclose(rfile);
	}
}

