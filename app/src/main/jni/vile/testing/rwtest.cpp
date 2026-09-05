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

#include "rwtest.h"

RWTest::RWTest(){
	// Start by testing basic ram blobs
	RWops *ops=new RWops();
	uString src="Testing blobs for ViLE";
	ops->OpenRAM((char*)src.c_str(),src.length(),false);
	TestObject("Read/write RAM Blobs",ops,false);
	ops->OpenROM(src.c_str(),src.length(),false);
	TestObject("Read-only RAM Blobs",ops,true);

	// Create a temporary file and test writing
	src=EDL_CreateTemporary()+"rwtest";
	Header("Writing with file blobs");
	Assert(ops->OpenFile(src.c_str(),"wb"),"Files opens for writing");
	Assert(ops->Size()==0,"Size starts at NULL");
	Assert(ops->Tell()==0,"Position pointer starts at NULL");
	Assert(ops->Seek(0,SEEK_END)==0,"Seeking to the end returns null");
	Assert(ops->Seek(0,SEEK_SET)==0,"Seeking to null returns null");
	Assert(ops->Write((void*)"Testing",1)==1,"Writing a byte returns one");
	Assert(ops->Tell()==1,"Position pointer increments at writing");
	Assert(ops->Seek(0,SEEK_END)==1,"Seeking to the end returns one");
	Assert(ops->Seek(0,SEEK_SET)==0,"Seeking to null returns null");
	Assert(ops->Size()==1,"Size is now one");
	Assert(ops->Write((void*)"Testing",1)==1,"Writing a byte returns one");
	Assert(ops->Tell()==1,"Position pointer increments at writing");
	Assert(ops->Seek(0,SEEK_END)==1,"Seeking to the end returns one");
	Assert(ops->Seek(0,SEEK_SET)==0,"Seeking to null returns null");
	Assert(ops->Size()==1,"Size is still one");
	Assert(ops->Write((void*)"Testing files",12)>1,"Writing strings works");
	Assert(ops->Tell()>1,"Position pointer increments at writing");
	Assert(ops->OpenFile(src.c_str(),"rb"),"Files reopens for reading");
	Assert(ops->Size()>0,"Size is larger than NULL");
	Assert(ops->Tell()==0,"Position pointer starts at NULL");
	TestObject("Read-only file blobs",ops,true);
	EDL_DeleteFile(src);
	Report();
}

void RWTest::TestObject(uString Name,RWops *Object,bool RO){
	Header(Name);
	int size=Object->Size();
	char buffer[size];
	Assert(Object->Tell()==0,"Position pointer starts at NULL");
	Assert(Object->Seek(0,SEEK_END)==size,"Seeking to the end returns size");
	Assert(Object->Seek(0,SEEK_SET)==0,"Seeking to null returns null");
	Assert(Object->Size()>0,"Size is positive");
	Assert(Object->Size()<100,"Size seems correct");
	Assert(Object->Read(buffer,size/2)==size/2,"Read returns read bytes");
	Assert(Object->Tell()==size/2,"Position pointer updated after read");
	Assert(Object->Seek(0,SEEK_SET)==0,"Seeking works");
	Assert(Object->Read(buffer,1)==1,"Single byte reading works");
	Assert(Object->Read(buffer,1)==1,"Consequtive reading works");
	Assert(Object->Tell()==2,"Position pointer updated after read");
	Assert(Object->Read(buffer,size)==size-2,"Reads beyond buffer is blocked");
	Assert(Object->Read(buffer,size)==0,"Reads after eof is blocked");
	Assert(Object->Seek(0,SEEK_SET)==0,"Seeking still works");
	Assert(Object->Read(buffer,size)==size,"Reading entire buffer works");
	Assert(Object->Read(buffer,1)==0,"Reads after eof is blocked");
	Assert(Object->Seek(0,SEEK_SET)==0,"Seeking still works");
	if(RO){
		Assert(Object->Write(buffer,size)==0,"Writing is not permitted");
		Assert(Object->Size()==size,"Size is unchanged");
	}
	else{
		Assert(Object->Write(buffer,size)==size,"Writing data works");
		Assert(Object->Size()==size,"Size is unchanged");
	}
}


