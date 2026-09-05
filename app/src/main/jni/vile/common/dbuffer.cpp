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

#include "dbuffer.h"

DBuffer::DBuffer(){
	buffers=0;
	sizes=0;
	size=0;
}

DBuffer::~DBuffer(){
	if(buffers){
		for(int i=0;i<size;i++){
			if(buffers[i]){
				delete [] buffers[i];
			}
		}
		delete buffers;
		delete sizes;
	}
}

void DBuffer::SetBuffer(int Index,Uint8 *Buffer,int Size){
	if(Index>=size){
		// Rebuild a new bufferbuffer
		int i=0;
		Uint8 **newbuffers=new Uint8*[Index+1];
		int *newsizes=new int[Index+1];
		for(i=0;i<size;i++){
			newbuffers[i]=buffers[i];
			newsizes[i]=sizes[i];
		}
		for(;i<Index;i++){
			newbuffers[i]=0;
			newsizes[i]=0;
		}

		// Assign given buffer
		newbuffers[i]=Buffer;
		newsizes[i]=Size;

		// Replace old buffers
		if(buffers){
			delete buffers;
			delete sizes;
		}
		buffers=newbuffers;
		sizes=newsizes;
		size=Index+1;
	}
}

bool DBuffer::GetBuffer(int Index,Uint8 **Buffer,int *Size){
	if(Index<size){
		*Buffer=buffers[Index];
		*Size=sizes[Index];
		return true;
	}
	return false;
}

