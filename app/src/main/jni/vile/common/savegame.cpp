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

#include "savegame.h"

Savegame::Savegame(uString Gamecode,int Index){
	filename=Savename(Gamecode,Index);
	buffer=0;
	length=0;
}

Savegame::~Savegame(){
	Clear();
}

/*! \brief Generates a filename for the savegame
 *  \param GameID Unique game identifier
 *  \param Index Savegame index for this game
 *  \returns Name for the savegame
 */
uString Savegame::Savename(uString GameID,int Index){
	// Compile filename
	char fnbuffer[GameID.length()+32];
	sprintf(fnbuffer,"%s%3.3d",GameID.c_str(),Index);
	uString path=Cfg::Path::save;
	if(path.length()){
		path+="/";
	}
	return path+fnbuffer;
}

/*! \brief Searches current buffer for a named resource
 *  \param Name Resource name to search for
 *  \param Start Start of resource if found
 *  \param End End of resource if found
 *  \return True if a valid entry was found
 */
bool Savegame::search(uString Name,Uint32 *Start,Uint32 *End){
	if(buffer && Name.length()){
		for(unsigned int i=0;i<length;){
			// Extract start and endpointers
			(*Start)=(buffer[i+SNLENGTH+0])|
				(buffer[i+SNLENGTH+1]<<8)|
				(buffer[i+SNLENGTH+2]<<16)|
				(buffer[i+SNLENGTH+3]<<24);
			(*End)=(buffer[i+SNLENGTH+4])|
				(buffer[i+SNLENGTH+5]<<8)|
				(buffer[i+SNLENGTH+6]<<16)|
				(buffer[i+SNLENGTH+7]<<24);

			// Compare
			if((*Start)<=(*End) || (*End)<=length || (*Start)==i+SHLENGTH){
				if(buffer[i]==Name.c_str()[0] && i+Name.length()<length){
					unsigned int pos=0;
					for(;pos<Name.length() &&
							buffer[i+pos]==Name.c_str()[pos];
							pos++);
					if(pos==Name.length()){
						return true;
					}
				}
			}
			else{
				LogError("Savegame is corrupted");
				return false;
			}

			// Jump to next pointer
			i=(*End);
		}
	}
	return false;
}

/*! \brief Formats and writes an entry to the current buffer
 *  \param Name Resource name
 *  \param Pos Position to start writing in the buffer
 *  \param Buffer Source buffer data
 *  \param Length Length of source buffer
 *  \return True if data was successfully written
 */
bool Savegame::write(uString Name,Uint32 Pos,Uint8 *Buffer,Uint32 Length){
	bool retval=(length>=Pos+Length+SHLENGTH);
	if(retval){
		int j=Pos;
		Uint32 start=Pos+SHLENGTH;
		Uint32 end=start+Length;
		for(unsigned int i=0;i<SNLENGTH-1 && i<Name.length();){
			buffer[j++]=Name.c_str()[i++];
		}
		for(;j-Pos<SNLENGTH;){
			buffer[j++]=0;
		}
		buffer[j++]=(start)&0xFF;
		buffer[j++]=(start>>8)&0xFF;
		buffer[j++]=(start>>16)&0xFF;
		buffer[j++]=(start>>24)&0xFF;
		buffer[j++]=(end)&0xFF;
		buffer[j++]=(end>>8)&0xFF;
		buffer[j++]=(end>>16)&0xFF;
		buffer[j++]=(end>>24)&0xFF;
		for(unsigned int i=0;i<Length;i++){
			buffer[j++]=Buffer[i];
		}
	}
	return retval;
}

/*! \brief Drops current buffer and updates from file
 *  \returns True if data was successfully read
 */
bool Savegame::Read(){
	bool retval=false;
	FILE *rf=fopen(filename.c_str(),"rb");
	if(rf){
		fseek(rf,0,SEEK_END);
		int rs=ftell(rf);
		if(rs){
			fseek(rf,0,SEEK_SET);
			Uint8 *newbuffer=new Uint8[rs];
			if((int)fread(newbuffer,1,rs,rf)>0){
				if(buffer){
					delete [] buffer;
				}
				buffer=newbuffer;
				length=rs;
				retval=true;
			}
		}
		fclose(rf);
	}
	return retval;
}

/*! \brief Writes current buffer to file
 *  \returns True if data was successfully written
 */
bool Savegame::Write(){
	bool retval=false;
	if(buffer){
		FILE *wf=fopen(filename.c_str(),"wb");
		if(wf){
			fwrite(buffer,1,length,wf);
			fclose(wf);
			retval=true;
		}
	}
	return retval;
}

/*! \brief Tests wether savegame file exists
 *  \return True if file exists and size is nonzero
 */
bool Savegame::Exists(){
	FILE *tf=fopen(filename.c_str(),"r");
	if(tf!=NULL){
		fseek(tf,0,SEEK_END);
		int l=ftell(tf);
		fclose(tf);
		return l>0;
	}
	else{
		return false;
	}
}

/*! \brief Flushes buffered content
 */
void Savegame::Clear(){
	if(buffer){
		delete [] buffer;
		buffer=0;
		length=0;
	}
}

/*! \brief Flushes buffered content and deletes existing file
 */
void Savegame::Delete(){
	Clear();
#ifndef ANDROID
	unlink(filename.c_str());
#endif
}

/*! \brief Embeds a bitmap in the savestream
 *  \param Name Name of the saved resource
 *  \param Surface Surface to store
 *  \param W Optional parameter to resize the surface before storing it
 *  \param H Optional parameter to resize the surface before storing it
 *
 *  The surface will be converted to a stream of bytes and compressed with
 *  zlib before it is stored. This preserves alpha and provides lossless
 *  compression, but the current implementation will only work for 32bit
 *  surfaces.
 */
void Savegame::SaveSurface(uString Name,SDL_Surface *Surface,int W,int H){
	if(Surface && Surface->w && Surface->h){
		// Resize bitmap
		SDL_Surface *target=Surface;
		if(W && H){
			double rx=W/(double)Surface->w;
			double ry=H/(double)Surface->h;
			target=zoomSurface(Surface,rx,ry,1);
		}

		// Convert target surface to bytes
		Uint16 w=target->w;
		Uint16 h=target->h;
		Uint8 b=target->format->BytesPerPixel;
		int size=2+2+1+(w*h*b);
		Uint8 *buffer=new Uint8[size];
		int o=0;
		buffer[o++]=(w>>8)&0xFF;
		buffer[o++]=w&0xFF;
		buffer[o++]=(h>>8)&0xFF;
		buffer[o++]=h&0xFF;
		buffer[o++]=b*8;
		for(int y=0;y<h;y++){
			for(int x=0;x<w;x++){
				Uint32 *pixel=(Uint32*)target->pixels+y*target->pitch/4+x;
				buffer[o++]=((*pixel)>>24)&0xFF;
				buffer[o++]=((*pixel)>>16)&0xFF;
				buffer[o++]=((*pixel)>>8)&0xFF;
				buffer[o++]=(*pixel)&0xFF;
			}
		}

		// Compress buffer with zlib
		long unsigned int csize=(size*1.1)+12;
		Uint8 *cbuffer=new Uint8[csize+8];
		int result=compress(cbuffer+8,&csize,buffer,size);
		if(result==Z_OK){
			cbuffer[0]='Z';
			cbuffer[1]='L';
			cbuffer[2]='I';
			cbuffer[3]='B';
			cbuffer[4]=(size>>24)&0xFF;
			cbuffer[5]=(size>>16)&0xFF;
			cbuffer[6]=(size>>8)&0xFF;
			cbuffer[7]=size&0xFF;
			SaveBuffer(Name,cbuffer,csize+8);
		}
		else{
			LogError("ZLib error: %d",result);
		}

		// Clean up
		delete [] cbuffer;
		delete [] buffer;
		if(target!=Surface){
			SDL_FreeSurface(target);
		}
	}
}

/*! \brief Extracts a bitmap from the loadstream
 *  \param Name Name of the saved resource
 *  \param Surface Pointer for new surface
 *  \param W Optional parameter to resize the surface after loading it
 *  \param H Optional parameter to resize the surface after loading it
 *  \return Extracted bitmap or NULL if failed
 *
 *  The new surface must be freed by the caller
 */
bool Savegame::LoadSurface(uString Name,SDL_Surface **Surface,int W,int H){
	Uint8 *b;
	Uint32 l;
	bool retval=false;
	if(LoadBuffer(Name.c_str(),&b,&l)){
		*Surface=0;
		if(b[0]=='B' && b[1]=='M'){
			// Load as bitmap (Backwards compability only)
			SDL_RWops *ops=SDL_RWFromMem(b,l);
			*Surface=SDL_LoadBMP_RW(ops,1);
		}
		else if(b[0]=='Z' && b[1]=='L' && b[2]=='I' && b[3]=='B'){
			// Decompress buffer with zlib (ViLE>=0.4.10)
			long unsigned int size=0;
			size|=b[5]<<24;
			size|=b[6]<<16;
			size|=b[7]<<8;
			size|=b[8];
			size=(size*1.1)+12;
			Uint8 *buffer=new Uint8[size];
			int result=uncompress(buffer,&size,b+8,l-8);
			if(result==Z_OK){
				Uint16 w=(buffer[0]<<8)|buffer[1];
				Uint16 h=(buffer[2]<<8)|buffer[3];
				Uint16 b=buffer[4];
				int i=5;
				*Surface=EDL_CreateSurface(w,h);
				SDL_Surface *s=*Surface;
				for(int y=0;y<h;y++){
					for(int x=0;x<w;x++){
						Uint32 *pixel=(Uint32*)s->pixels+y*s->pitch/4+x;
						*pixel=0;
						*pixel|=buffer[i++]<<24;
						*pixel|=buffer[i++]<<16;
						*pixel|=buffer[i++]<<8;
						*pixel|=buffer[i++];
					}
				}
			}
			else{
				LogError("ZLib error: %d",result);
			}
			delete [] buffer;
		}
		else{
			// Report errors
			LogError("Invalid surface header: %c%c%c%c",b[0],b[1],b[2],b[3]);
		}

		// Resize if applicable
		if(*Surface && W && H && W!=(*Surface)->w && H!=(*Surface)->h){
			double rx=W/(double)(*Surface)->w;
			double ry=H/(double)(*Surface)->h;
			SDL_Surface *rs=zoomSurface(*Surface,rx,ry,1);
			SDL_FreeSurface(*Surface);
			*Surface=rs;
		}

		// Clean up the mess
		retval=*Surface;
		delete [] b;
	}
	return retval;
}

/*! \brief Saves a named variable
 *  \param Name Name of the variable
 *  \param Value Value of the variable
 */
void Savegame::SaveUint8(uString Name,Uint8 Value){
	SaveBuffer(Name,&Value,1);
}

/*! \brief Saves a named variable
 *  \param Name Name of the variable
 *  \param Value Value of the variable
 */
void Savegame::SaveUint16(uString Name,Uint16 Value){
	Uint8 b[2];
	b[0]=(Value)&0xFF;
	b[1]=(Value>>8)&0xFF;
	SaveBuffer(Name,b,2);
}

/*! \brief Saves a named variable
 *  \param Name Name of the variable
 *  \param Value Value of the variable
 */
void Savegame::SaveUint32(uString Name,Uint32 Value){
	Uint8 b[4];
	b[0]=(Value)&0xFF;
	b[1]=(Value>>8)&0xFF;
	b[2]=(Value>>16)&0xFF;
	b[3]=(Value>>24)&0xFF;
	SaveBuffer(Name,b,4);
}

/*! \brief Saves a list of strings
 *  \param Name Name of the stringlist
 *  \param List List of strings
 */
void Savegame::SaveStringList(uString Name,Stringlist *List){
	if(List && List->GetCount()){
		uString text;
		char name[32];
		int count=List->GetCount();
		SaveUint16(Name+".length",count);
		for(int i=0;i<count;i++){
			if(List->GetString(i,&text) && text.length()){
				sprintf(name,"%s.%d",Name.c_str(),i);
				SaveString(name,text);
			}
		}
	}
}

/*! \brief Saves a named variable
 *  \param Name Name of the variable
 *  \param Value Value of the variable
 */
void Savegame::SaveString(uString Name,uString Value){
	SaveBuffer(Name,(Uint8*)Value.c_str(),Value.length());
}

/*! \brief Saves a list of values
 *  \param Name Name of the vector
 *  \param Vector List of values
 */
void Savegame::SaveVector(uString Name,DVector *Vector){
	Uint8 *buffer;
	Uint32 length;
	if(Vector && Vector->GetBuffer(&buffer,&length)){
		SaveBuffer(Name,buffer,length);
	}
}

/*! \brief Saves a buffer using a resource name
 *  \param Name Resource name
 *  \param Buffer Source data
 *  \param Length Size of source data
 */
void Savegame::SaveBuffer(uString Name,Uint8 *Buffer,Uint32 Length){
	Uint32 start,end;
	if(!buffer){
		// Write directly
		length=Length+SHLENGTH;
		buffer=new Uint8[length];
		write(Name,0,Buffer,Length);
	}
	else if(search(Name,&start,&end)){
		// Inject into existing
		Uint32 newlength=length+Length+SHLENGTH-((end-start)+SHLENGTH);
		Uint8 *newbuffer=new Uint8[newlength];
		int j=0;
		for(unsigned int i=0;i<start-SHLENGTH;i++){
			newbuffer[j++]=buffer[i];
		}
		for(unsigned int i=end;i<length;){
			Uint32 sstart=(buffer[i+SNLENGTH+0])|(buffer[i+SNLENGTH+1]<<8)|
				(buffer[i+SNLENGTH+2]<<16)|(buffer[i+SNLENGTH+3]<<24);
			Uint32 send=(buffer[i+SNLENGTH+4])|(buffer[i+SNLENGTH+5]<<8)|
				(buffer[i+SNLENGTH+6]<<16)|(buffer[i+SNLENGTH+7]<<24);
			Uint32 dstart=j+SHLENGTH;
			Uint32 dend=dstart+(send-sstart);
			for(unsigned int k=i;k<i+SNLENGTH;k++){
				newbuffer[j++]=buffer[k];
			}
			newbuffer[j++]=(dstart)&0xFF;
			newbuffer[j++]=(dstart>>8)&0xFF;
			newbuffer[j++]=(dstart>>16)&0xFF;
			newbuffer[j++]=(dstart>>24)&0xFF;
			newbuffer[j++]=(dend)&0xFF;
			newbuffer[j++]=(dend>>8)&0xFF;
			newbuffer[j++]=(dend>>16)&0xFF;
			newbuffer[j++]=(dend>>24)&0xFF;
			for(unsigned int k=sstart;k<send;k++){
				newbuffer[j++]=buffer[k];
			}
			i=send;
		}
		delete [] buffer;
		buffer=newbuffer;
		length=newlength;
		write(Name,j,Buffer,Length);
	}
	else{
		// Append to ending
		int j=0;
		Uint32 newlength=length+Length+SHLENGTH;
		Uint8 *newbuffer=new Uint8[newlength];
		for(unsigned int i=0;i<length;i++){
			newbuffer[j++]=buffer[i];
		}
		delete [] buffer;
		buffer=newbuffer;
		length=newlength;
		write(Name,j,Buffer,Length);
	}
}

/*! \brief Loads a named variable
 *  \param Name Name of the variable
 *  \param Value Pointer to local variable
 *  \returns True if the variable was found (and copied)
 */
bool Savegame::LoadUint8(uString Name,Uint8 *Value){
	Uint8 *b;
	Uint32 l;
	bool retval=false;
	if(LoadBuffer(Name,&b,&l)){
		if(l==1){
			(*Value)=b[0];
			retval=true;
		}
		delete [] b;
	}
	return retval;
}

/*! \brief Loads a named variable
 *  \param Name Name of the variable
 *  \param Value Pointer to local variable
 *  \returns True if the variable was found (and copied)
 */
bool Savegame::LoadUint16(uString Name,Uint16 *Value){
	Uint8 *b;
	Uint32 l;
	bool retval=false;
	if(LoadBuffer(Name,&b,&l)){
		if(l==2){
			(*Value)=(b[0])|(b[1]<<8);
			retval=true;
		}
		delete [] b;
	}
	return retval;
}

/*! \brief Loads a named variable
 *  \param Name Name of the variable
 *  \param Value Pointer to local variable
 *  \returns True if the variable was found (and copied)
 */
bool Savegame::LoadUint32(uString Name,Uint32 *Value){
	Uint8 *b;
	Uint32 l;
	bool retval=false;
	if(LoadBuffer(Name,&b,&l)){
		if(l==4){
			(*Value)=(b[0])|(b[1]<<8)|(b[2]<<16)|(b[3]<<24);
			retval=true;
		}
		delete [] b;
	}
	return retval;
}

/*! \brief Loads a named list of variables
 *  \param Name Name of the vector
 *  \param Vector Pointer to local list
 *  \returns True if the list was found (and copied)
 */
bool Savegame::LoadVector(uString Name,DVector *Vector){
	bool retval=false;
	Uint8 *buffer;
	Uint32 length;
	if(Vector && LoadBuffer(Name,&buffer,&length)){
		Vector->SetBuffer(buffer,length);
		delete [] buffer;
		retval=true;
	}
	return retval;
}

/*! \brief Loads a named list of strings
 *  \param Name Name of the list
 *  \param List Pointer to local list
 *  \returns True if the list was found (and copied)
 */
void Savegame::LoadStringList(uString Name,Stringlist *List){
	if(List){
		Uint16 length;
		char name[32];
		uString text;
		if(LoadUint16(Name+".length",&length)){
			for(int i=0;i<length;i++){
				sprintf(name,"%s.%d",Name.c_str(),i);
				if(LoadString(name,&text)){
					List->SetString(i,text);
				}
			}
		}
	}
}

/*! \brief Loads a named string
 *  \param Name Name of the string
 *  \param Value Pointer to local variable
 *  \returns True if the string was found (and copied)
 */
bool Savegame::LoadString(uString Name,uString *Value){
	Uint8 *b;
	Uint32 l;
	bool retval=false;
	if(LoadBuffer(Name,&b,&l)){
		char tb[l+1];
		for(unsigned int i=0;i<l;i++){
			tb[i]=b[i];
		}
		tb[l]=0;
		*Value=tb;
		retval=true;
		delete [] b;
	}
	return retval;
}

/*! \brief Loads a named resource from buffer
 *  \param Name Resource name
 *  \param Buffer Destination buffer pointer
 *  \param Length Size of returned buffer
 *  \return True if data was read and buffer allocated
 *
 *  Caller is responsible for freeing Buffer after usage
 */
bool Savegame::LoadBuffer(uString Name,Uint8 **Buffer,Uint32 *Length){
	Uint32 start,end;
	bool retval=false;
	if(search(Name,&start,&end)){
		*Length=end-start;
		if(*Length){
			retval=true;
			*Buffer=new Uint8[(*Length)];
			for(unsigned int i=0;i<(*Length);i++){
				(*Buffer)[i]=buffer[start+i];
			}
		}
	}
	return retval;
}


