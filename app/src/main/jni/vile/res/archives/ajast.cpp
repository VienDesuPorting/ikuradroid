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

#include "ajast.h"

ArchiveJAST::ArchiveJAST(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(path.c_str(),"rb");
	if(rfile!=NULL){
		unsigned char h[10];
		if(fread(h,1,10,rfile)>0){
			uString name;
			fseek(rfile,0,SEEK_END);
			unsigned int size=ftell(rfile);
			unsigned int offset=(h[6] | h[7]<<8 | h[8]<<16 | h[9]<<24)-1;
			if(size>offset && !strncmp((char*)h,"VFTech",6)){
				fseek(rfile,offset,SEEK_SET);
				unsigned char b[(size-offset)+1];
				size=fread(b,1,size-offset,rfile);
				for(unsigned int i=0;i+8<size;){
					// Extract offset and compressed size
					int o=(b[i+0]|(b[i+1]<<8)|(b[i+2]<<16)|(b[i+3]<<24))-1;
					int s=b[i+4]|(b[i+5]<<8)|(b[i+6]<<16)|(b[i+7]<<24);
					if(o<0 || s<0){
						break;
					}

					// Extract filename and skip dummy padding
					uString n=(char*)b+i+8;
					i+=8;
					while(b[i++]);
					while(b[i++]);
					while(b[i++]);
					while(b[i++]);

					// Register item and iterate
					AddItem(n,o,s);
				}
			}
		}
		fclose(rfile);
	}
}

/*! \brief Extracts and decompress a resource from the archive
 *	\param Name Resource name
 *  \return RWops with the resource data (Might be encrypted!)
 */
RWops *ArchiveJAST::GetResource(uString Name){
	RWops *retval=0;
	int offset,csize,dsize;
	if(GetItem(Name,offset,csize)){
		// Read compressed input data
		FILE *rf=fopen(path.c_str(),"rb");
		if(rf){
			unsigned char *cbuffer=new unsigned char[csize];
			fseek(rf,offset,SEEK_SET);
			if(fread(cbuffer,1,csize,rf)>(int)0){
				// Decompress with zlib
				dsize=cbuffer[0]|(cbuffer[1]<<8)|
					(cbuffer[2]<<16)|(cbuffer[3]<<24);
				unsigned char *dbuffer=new unsigned char[dsize];
				uLongf lcsize=csize-4;
				uLongf ldsize=dsize;
				uncompress(dbuffer,&ldsize,cbuffer+4,lcsize);

				// Create blob with input data
				retval=new RWops();
				if(!retval->OpenROM((char*)dbuffer,dsize,true)){
					delete [] dbuffer;
					delete retval;
					retval=0;
				}
			}
			delete [] cbuffer;
			fclose(rf);
		}
	}
	return retval;
}

