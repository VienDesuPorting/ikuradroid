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

#include "acrowdsce.h"

ArchiveCrowdSCE::ArchiveCrowdSCE(uString Path) : ArchiveBase(Path){
	curbuffer=0;
	curlength=0;
	FILE *rfile=fopen(EDL_Realname(Path).c_str(),"rb");
	if(rfile!=NULL){
		// Extract header to identify file
		unsigned char hbuffer[32];
		if((fread(hbuffer,1,32,rfile)>(int)0)){
			if(!strncmp((char*)hbuffer,"scenario",8)){
				// XChange 2 style with header
			}
			else if(hbuffer[0]=='G' && hbuffer[1]=='Q'){
				// XChange 3 style without header
				fseek(rfile,0,SEEK_END);
				unsigned int tlength=ftell(rfile);
				char *tbuffer=new char[tlength];
				fseek(rfile,0,SEEK_SET);
				if(tbuffer && (fread(tbuffer,1,tlength,rfile)>(int)0)){
					Decrypt(tbuffer,tlength);
					if(ParseIndex(tbuffer,tlength)){
						if(curbuffer){
							delete [] curbuffer;
						}
						curbuffer=tbuffer;
						curlength=tlength;
					}
				}
			}
			else if(hbuffer[0]==0xDB && hbuffer[1]==0xCE){
				// Inverted Crowd Engine 1 script
				fseek(rfile,0,SEEK_END);
				unsigned int tlength=ftell(rfile);
				char *tbuffer=new char[tlength];
				fseek(rfile,0,SEEK_SET);
				if(tbuffer && (fread(tbuffer,1,tlength,rfile)>(int)0)){
					for(int i=0;i<tlength;i++){
						tbuffer[i]=~tbuffer[i];
					}
					if(ParseIndex(tbuffer,tlength)){
						if(curbuffer){
							delete [] curbuffer;
						}
						curbuffer=tbuffer;
						curlength=tlength;
					}
				}
			}
			else if(hbuffer[0]=='$' && hbuffer[1]=='1'){
				// XChange 1 "why bother compiling anyway?" style
				fseek(rfile,0,SEEK_END);
				unsigned int tlength=ftell(rfile);
				char *tbuffer=new char[tlength];
				fseek(rfile,0,SEEK_SET);
				if(tbuffer && (fread(tbuffer,1,tlength,rfile)>(int)0)){
					if(ParseIndex(tbuffer,tlength)){
						if(curbuffer){
							delete [] curbuffer;
						}
						curbuffer=tbuffer;
						curlength=tlength;
					}
				}
			}
		}

		// Clean up
		fclose(rfile);
	}
}

ArchiveCrowdSCE::~ArchiveCrowdSCE(){
	if(curbuffer){
		delete [] curbuffer;
	}
}

/*! \brief Decrypts the entire script collection
 *	\param Buffer Buffer containing the encrypted script collection
 *	\param Length Length of the buffer
 *
 *	After execution, Buffer will be decrypted
 */
void ArchiveCrowdSCE::Decrypt(char *Buffer,int Length){
	// Decrypt data
	char sig[]="crowd script yeah ";
	int B=0,C=0,k1=0,k2=0;
	for(int i=0;i<Length;i++){
		int D=(B+C)%18;
		int x=D;
		D=(int)sig[D];
		C=((C&(k1&0xFF))|D)&0x800000FF;
		Buffer[i]=Buffer[i]^C;

		if(x==0){
			D=(k1+k2)%18;
			k1++;
			k2=(int)sig[D];
		}

		C=k2;
		B++;
	}
}

/*! \brief Parses the items in a SCE index
 *	\param Buffer Decrypted SCE data
 *	\param Length Size of Buffer
 *	\returns Indexed item count
 */
int ArchiveCrowdSCE::ParseIndex(char *Buffer,int Length){
	int retval=0;
	for(int i=0;i<Length;i++){
		if(Buffer[i]=='$'){
			// Found script, extract the name
			while(++i<Length && (Buffer[i]==' ' || Buffer[i]=='\r'));
			int j=i;
			while(++j<Length && Buffer[j]!=' ' && Buffer[j]!='\r');
			char fname[32]={0};
			if(j<Length){
				strncpy(fname,Buffer+i,j-i);
				fname[j-i]=0;
			}

			// Find end of script and record position
			int start=j;
			while(++j<Length && Buffer[j]!='$');
			int size=(j-1)-start;
			i=start+size;

			// Find name to next script
			while(++j<Length && (Buffer[j]==' ' || Buffer[j]=='\r'));
			while(++j<Length && Buffer[j]!=' ' && Buffer[j]!='\r');
			if(j<Length){
				size=j-start;
			}
			else{
				size=Length-start;
			}

			// Verify and add
			if(fname[0] && size>0 && start+size<=Length){
				// Normal script found
				AddItem(fname,start,size);
				retval++;
			}
			else{
				LogError("Invalid SCE item at index %d",retval);
				LogError("Name=(%s)  offset=%d  size=%d",fname,start,size);
			}
		}
	}
	return retval;
}

/*! \brief Locates and extracts a script from a Crowd SCE file
 *	\param Name Name of the script
 *	\returns RWops with extracted resource or NULL
 */
RWops *ArchiveCrowdSCE::GetResource(uString Name){
	RWops *retval=0;
	int offset,size;
	if(GetItem(Name,offset,size)){
		if(size){
			// Create a new copy in ram
			retval=new RWops();
			if(!retval->OpenROM(curbuffer+offset,size,false)){
				delete retval;
				retval=0;
			}
		}
	}
	return retval;
}

