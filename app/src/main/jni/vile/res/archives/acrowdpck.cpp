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

#include "acrowdpck.h"

ArchiveCrowdPCK::ArchiveCrowdPCK(uString Path) : ArchiveBase(Path){
	FILE *rfile=fopen(EDL_Realname(Path).c_str(),"rb");
	if(rfile!=NULL){
		// Read file header
		unsigned char rb[12];
		fseek(rfile,0,SEEK_SET);
		if((fread(rb,1,12,rfile)>(int)0)){
			// Get record count and calculate index offsets
			Uint32 rcnt=((rb[3]&0xFF)<<24)|
						((rb[2]&0xFF)<<16)|
						((rb[1]&0xFF)<<8)|
						((rb[0]&0xFF));
			unsigned int npos=4+(rcnt*12);

			// Read header size from first record
			Uint32 ilen=((rb[8+3]&0xFF)<<24)|
						((rb[8+2]&0xFF)<<16)|
						((rb[8+1]&0xFF)<<8)|
						((rb[8+0]&0xFF));

			// Get total file length
			fseek(rfile,0,SEEK_END);
			unsigned int tlen=ftell(rfile);
			if(rcnt>0 && rcnt<0xFFFF && ilen<tlen && ilen>npos){
				// Read entire index from file
				fseek(rfile,0,SEEK_SET);
				unsigned char *tb=new unsigned char[ilen];
				if((fread(tb,1,ilen,rfile)>(int)0)){
					// Index items
					ParseIndex(tb,ilen);
				}
				delete [] tb;
			}

		}
		// Clean up
		fclose(rfile);
	}
}

int ArchiveCrowdPCK::ParseIndex(unsigned char *Buffer,int Length){
	// Get record count and calculate index offsets
	int returnval=0;
	Uint32 rcnt=((Buffer[3]&0xFF)<<24)|
				((Buffer[2]&0xFF)<<16)|
				((Buffer[1]&0xFF)<<8)|
				((Buffer[0]&0xFF));
	unsigned int rpos=4;
	unsigned int npos=4+(rcnt*12);
	if(rcnt>0 && rcnt<0xFFFF){
		// Iterate records
		for(unsigned int i=0;i<rcnt;i++){
			// Skip dummy
			rpos+=4;

			//Read offset
			Uint32 offset=((Buffer[rpos+3]&0xFF)<<24)|
						  ((Buffer[rpos+2]&0xFF)<<16)|
						  ((Buffer[rpos+1]&0xFF)<<8)|
						  ((Buffer[rpos+0]&0xFF));

			//Read filesize 
			Uint32 fsize= ((Buffer[rpos+7]&0xFF)<<24)|
						  ((Buffer[rpos+6]&0xFF)<<16)|
						  ((Buffer[rpos+5]&0xFF)<<8)|
						  ((Buffer[rpos+4]&0xFF));

			// Read filename
			rpos+=8;
			unsigned int fnlen=0;
			char fb[MAX_FILENAME_LENGTH];
			while(fnlen<MAX_FILENAME_LENGTH && Buffer[npos]){
				fb[fnlen++]=Buffer[npos++];
			}
			fb[fnlen++]=0;
			npos++;

			// Verify data and store index
			if(offset>0 && fsize>0 && fnlen>0 && fnlen<MAX_FILENAME_LENGTH){
				fb[fnlen]=0;
				AddItem(fb,offset,fsize);
				returnval++;
			}
			else{
				LogError("Error in Crowd PCK archive");
				returnval=0;
				break;
			}
		}
	}
	return returnval;
}

