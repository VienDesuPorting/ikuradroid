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

#include "rwops.h"

/*! \brief Opens an empty, generic resource blob
 */
RWops::RWops(){
	type=RW_NONE;
	buffer=0;
	size=0;
	offset=0;
	ops=0;
}

/*! \brief Makes sure that the resource is closed before exiting
 */
RWops::~RWops(){
	Close();
}

/*! \brief Opens a resource from an existing RWops objects
 *  \param Ops Blob to use as source
 *  \param FreeSource Original source is deleted if true
 *  \return True if resource could be opened
 */
bool RWops::OpenRW(RWops *Ops,bool FreeSource){
	if(Ops){
		return OpenRW(Ops,0,Ops->Size(),FreeSource);
	}
	else{
		return false;
	}
}

/*! \brief Opens a partial resource from an existing RWops objects
 *  \param Ops Blob to use as source
 *  \param Offset Byte offset of resource
 *  \param Size Size of the resource in bytes
 *  \param FreeSource Original source is deleted if true
 *  \return True if resource could be opened
 */
bool RWops::OpenRW(RWops *Ops,int Offset,int Size,bool FreeSource){
	bool retval=false;
	if(Ops && Ops!=this){
		Close();
		if(Ops->type==RW_NONE){
		}
		else if(Ops->type==RW_FILE){
			// Open as file
			retval=OpenFile(Ops->filename.c_str(),Ops->offset+Offset,Size);
		}
		else if(FreeSource){
			// Move content before deleting blob
			buffer=Ops->buffer;
			offset=Ops->offset;
			size=Ops->size;
			type=Ops->type;
			Ops->buffer=0;
			Ops->offset=0;
			Ops->size=0;
			Ops->type=RW_NONE;
			retval=size;
		}
		else if(Ops->size>=Offset+Size && Size>0){
			// Copy content
			char *b=new char[Size];
			Ops->Seek(Offset,SEEK_SET);
			Ops->Read(buffer,Size);
			retval=OpenRAM(b,Size,false);
		}

		// Close source
		if(FreeSource){
			delete Ops;
		}
	}
	return retval;
}

/*! \brief Opens a partial file
 *  \param Filename Path to the file to open
 *  \param Offset Byte offset of resource
 *  \param Size Size of the resource in bytes
 *  \return True if resource could be opened
 *
 *  Partial file resources is always read-only
 */
bool RWops::OpenFile(const char *Filename,int Offset,int Size){
	Close();
	if(OpenFile(Filename,"rb")){
		if(size>=Offset+Size){
			size=Size;
			offset=Offset;
			Seek(0,SEEK_SET);
		}
		else{
			Close();
		}
	}
	return ops;
}

/*! \brief Opens a while file as a resource
 *  \param Filename Path to the file to open
 *  \param Offset Byte offset of resource
 *  \param Size Size of the resource in bytes
 *  \return True if resource could be opened
 */
bool RWops::OpenFile(const char *Filename,const char *Filemode){
	Close();
	if((ops=SDL_RWFromFile(Filename,Filemode))){
		size=SDL_RWseek(ops,0,SEEK_END);
		Seek(0,SEEK_SET);
		filename=Filename;
		type=RW_FILE;
	}
	return ops;
}

/*! \brief Opens a read-only memory resource
 *  \param Memory Source memory address
 *  \param Size Size of the resource in bytes
 *  \param FreeSource Original source is deleted if true
 *  \return True if resource could be opened
 */
bool RWops::OpenROM(const char *Memory,int Size,bool FreeSource){
	Close();
	if(FreeSource){
		// Use memory buffer instead of deleting it
		if((ops=SDL_RWFromConstMem(Memory,Size))){
			buffer=(void*)Memory;
			size=Size;
			type=RW_ROBUFFER;
		}
	}
	else if(Size){
		// Copy buffer and call it recursively
		char *b=new char[Size];
		for(int i=0;i<Size;i++){
			b[i]=Memory[i];
		}
		OpenROM((const char*)b,Size,true);
	}
	return ops;
}

/*! \brief Opens a RAM resource
 *  \param Memory Source memory address
 *  \param Size Size of the resource in bytes
 *  \param FreeSource Original source is deleted if true
 *  \return True if resource could be opened
 */
bool RWops::OpenRAM(char *Memory,int Size,bool FreeSource){
	Close();
	if(FreeSource){
		// Use memory buffer instead of deleting it
		if((ops=SDL_RWFromMem(Memory,Size))){
			buffer=(void*)Memory;
			size=Size;
			type=RW_RWBUFFER;
		}
	}
	else if(Size){
		// Copy buffer and call it recursively
		char *b=new char[Size];
		for(int i=0;i<Size;i++){
			b[i]=Memory[i];
		}
		OpenRAM(b,Size,true);
	}
	return ops;
}

/*! \brief Reads data to a target buffer
 *  \param Data Target buffer
 *  \param Size Number of bytes to read
 *  \return Number of bytes read (-1 indicates error)
 */
int RWops::Read(void *Data,int Size){
	int retval=-1;
	if(ops){
		int current=SDL_RWtell(ops)-offset;
		if(current<0 || current>size){
			// Out of scope
		}
		else if(current+Size>size){
			// Limit to size
			Size=size-current;
			if(Size<0){
				Size=0;
			}
			retval=SDL_RWread(ops,Data,1,Size);
		}
		else{
			// Read directly
			retval=SDL_RWread(ops,Data,1,Size);
		}
	}
	return retval;
}

/*! \brief Writes data from a target buffer
 *  \param Data Target buffer
 *  \param Size Number of bytes to write
 *  \return Number of bytes written (-1 indicates error)
 */
int RWops::Write(void *Data,int Size){
	int retval=-1;
	if(ops){
		if(type!=RW_ROBUFFER){
			retval=SDL_RWwrite(ops,Data,1,Size);
		}
		else{
			retval=0;
		}
		if(retval>0){
			int newpos=Tell();
			if(newpos>size){
				size=newpos;
			}
		}
	}
	return retval;
}

/*! \brief Seeks to a given position in the resource
 *  \param Offset Offset to seek to
 *  \param Whence The usual SEEK_SET/SEEK_END/SEEK_CUR specifier
 *  \return New offset relative to start of file
 */
int RWops::Seek(int Offset,int Whence){
	int retval=-1;
	if(ops){
		if(Whence==SEEK_SET){
			retval=SDL_RWseek(ops,offset+Offset,SEEK_SET)-offset;
		}
		else if(Whence==SEEK_CUR){
			retval=SDL_RWseek(ops,Offset,SEEK_CUR)-offset;
		}
		else if(Whence==SEEK_END){
			retval=SDL_RWseek(ops,offset+size+Offset,SEEK_SET)-offset;
		}
	}
	return retval;
}

/*! \brief Tells the current read/write position in the resource
 *  \returns Byte position relative to the start of the resource
 */
int RWops::Tell(){
	int retval=-1;
	if(ops){
		retval=SDL_RWtell(ops)-offset;
	}
	return retval;
}

/*! \brief Tells the total size of the resource
 *  \returns Resource size in bytes
 */
int RWops::Size(){
	return size;
}

/*! \brief Closes down and frees the resource data
 */
void RWops::Close(){
	if(ops){
		SDL_RWclose(ops);
	}
	if(buffer){
		char *tmp=(char*)buffer;
		delete [] tmp;
	}
	filename="";
	type=RW_NONE;
	offset=0;
	buffer=0;
	size=0;
	ops=0;
}

