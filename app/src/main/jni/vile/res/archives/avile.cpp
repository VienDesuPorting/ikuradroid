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

ArchiveViLE::ArchiveViLE(uString Path) : ArchiveBase(Path),memdata(0){
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
					IndexPack(dbuf,dlen);
				}
				delete [] dbuf;
			}
		}
		fclose(rfile);
	}
}

/*! \brief Indexes an embedded cabinet (VILE_PACK_DATA)
 *
 *  Same ViLEPACK layout as the file constructor, but the bytes live
 *  in the binary itself and resources are served straight from memory.
 */
ArchiveViLE::ArchiveViLE(const unsigned char *Data,int Size) :
			ArchiveBase(""),memdata(Data){
	if(memdata && Size>16 && !strncmp((const char*)Data,"ViLEPACK",8)){
		int dlen=Data[12]|(Data[13]<<8)|(Data[14]<<16)|(Data[15]<<24);
		if(dlen>16 && dlen<=Size){
			IndexPack(Data,dlen);
		}
	}
}

/*! \brief Parses a cabinet header table and indexes every item
 */
void ArchiveViLE::IndexPack(const unsigned char *dbuf,int dlen){
	for(int i=16;i<dlen;i+=32){
		// Create new entry (copy the name: embedded cabinets are const)
		int start=dbuf[i+24]|(dbuf[i+25]<<8)|
			(dbuf[i+26]<<16)|(dbuf[i+27]<<24);
		int size=dbuf[i+28]|(dbuf[i+29]<<8)|
			(dbuf[i+30]<<16)|(dbuf[i+31]<<24);
		char name[25];
		int j=0;
		while(j<24 && dbuf[i+j]){
			name[j]=dbuf[i+j];
			j++;
		}
		name[j]=0;
		AddItem(name,start,size);
	}
}

/*! \brief Gets a resource, serving embedded data straight from memory
 */
RWops *ArchiveViLE::GetResource(uString Name){
	RWops *retval=0;
	if(memdata){
		int offset,size;
		if(GetItem(Name,offset,size)){
			retval=new RWops();
			if(!retval->OpenROM((const char*)memdata+offset,size,false)){
				delete retval;
				retval=0;
			}
		}
		return retval;
	}
	return ArchiveBase::GetResource(Name);
}
