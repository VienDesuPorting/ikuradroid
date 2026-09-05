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

#include "atlove.h"

#define GETBYTE(B)				((B)[0])
#define GETWORD(B)				((B)[0]|((B)[1]<<8))
#define GETDWORD(B)				((B)[0]|((B)[1]<<8)|((B)[2]<<16)|((B)[3]<<24))

ArchiveTLove::ArchiveTLove(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(path.c_str(),"rb");
	if(rfile!=NULL){
		fseek(rfile,0,SEEK_END);
		unsigned int size=ftell(rfile);
		unsigned char b[16];
		fseek(rfile,0,SEEK_SET);
		if(size>16 && fread(b,1,2,rfile)==2){
			Uint32 loffset=0;
			Uint16 slice=GETWORD(b);
			if(slice<size && slice/16.0==slice/16){
				// Strict input decoding (Unknown input, untyped target)
				int test=0;
				unsigned char buffer[slice];
				if(fread(buffer,1,slice,rfile)==slice){
					bool test1=GETDWORD(buffer+slice-16)==0;
					bool test2=GETDWORD(buffer+slice-12)==0;
					bool test3=GETDWORD(buffer+slice-8)==0;
					bool test4=GETDWORD(buffer+slice-4)==size;
					if(test1 && test2 && test3 && test4){
						uString pname;
						char name[12+1];
						for(int i=0;i<slice-16;i+=16){
							// Verify resource name name
							name[0]=0;
							name[12]=0;
							for(int j=0;j<12;j++){
								if(isascii(buffer[i+j])){
									name[j]=buffer[i+j];
								}
								else{
									break;
								}
							}
							if(!name[0]){
								test=0;
								break;
							}

							// Get offsets
							Uint32 offset=GETDWORD(buffer+i+12);
							bool test5=offset<size;
							bool test6=offset>loffset;
							if(test5 && test6){
								loffset=offset;
								test++;
							}
							else{
								test=0;
								break;
							}
						}

						// Index files if valid
						loffset=0;
						if(test>10 && fread(b,1,16,rfile)==16){
							for(int i=0;i<slice;i+=16){
								Uint32 offset=GETDWORD(buffer+i+12);
								strncpy(name,(const char*)buffer+i,12);
								if(pname.length()){
									AddItem(pname,loffset,offset-loffset);
								}
								pname=name;
								loffset=offset;
							}
						}
					}
				}

			}
		}
		fclose(rfile);
	}
}

