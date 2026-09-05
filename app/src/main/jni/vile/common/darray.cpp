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

#include "darray.h"

DArray::DArray(const DArray &Src){
	vectors=0;
	len=0;
	(*this)=Src;
}

DArray::DArray(int Width,int Height){
	if(Width>0 && Height>0){
		len=Width;
		vectors=new DVector*[len];
		for(int i=0;i<len;i++){
			vectors[i]=new DVector(Height);
		}
	}
	else{
		vectors=0;
		len=0;
	}
}

DArray::~DArray(){
	if(vectors){
		for(int i=0;i<len;i++){
			delete vectors[i];
		}
		delete [] vectors;
	}
}

DArray& DArray::operator=(const DArray &Src) {
	if(this!=&Src){
		// Drop own reg and copy source
		if(vectors){
			for(int i=0;i<len;i++){
				delete vectors[i];
			}
			delete [] vectors;
		}
		len=Src.len;
		if(len<1){
			len=1;
		}
		vectors=new DVector*[len];
		for(int i=0;i<len;i++){
			vectors[i]=new DVector();
			vectors[i]=Src.vectors[i];
		}
	}
	return *this;
}


void DArray::assertvector(int Index){
	if(Index>=len){
		int i;
		DVector **newvectors=new DVector*[Index+1];
		for(i=0;i<len;i++){
			newvectors[i]=vectors[i];
		}
		for(;i<=Index;i++){
			newvectors[i]=new DVector();
		}
		if(vectors){
			delete [] vectors;
		}
		vectors=newvectors;
		len=Index+1;
	}
}

void DArray::SetUint32(int Index,int SubIndex,Uint32 Value){
	assertvector(Index);
	vectors[Index]->SetUint32(SubIndex,Value);
}

void DArray::SetUint16(int Index,int SubIndex,Uint16 Value){
	assertvector(Index);
	vectors[Index]->SetUint16(SubIndex,Value);
}

void DArray::SetUint8(int Index,int SubIndex,Uint8 Value){
	assertvector(Index);
	vectors[Index]->SetUint8(SubIndex,Value);
}

void DArray::SetSint32(int Index,int SubIndex,Sint32 Value){
	assertvector(Index);
	vectors[Index]->SetSint32(SubIndex,Value);
}

void DArray::SetSint16(int Index,int SubIndex,Sint16 Value){
	assertvector(Index);
	vectors[Index]->SetSint16(SubIndex,Value);
}

void DArray::SetSint8(int Index,int SubIndex,Sint8 Value){
	assertvector(Index);
	vectors[Index]->SetSint8(SubIndex,Value);
}

void DArray::SetBit(int Index,int SubIndex,bool Value){
	assertvector(Index);
	vectors[Index]->SetBit(SubIndex,Value);
}

Uint32 DArray::GetUint32(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetUint32(SubIndex);
	}
	else{
		return 0;
	}
}

Uint16 DArray::GetUint16(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetUint16(SubIndex);
	}
	else{
		return 0;
	}
}


Uint8 DArray::GetUint8(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetUint8(SubIndex);
	}
	else{
		return 0;
	}
}


Sint32 DArray::GetSint32(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetSint32(SubIndex);
	}
	else{
		return 0;
	}
}


Sint16 DArray::GetSint16(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetSint16(SubIndex);
	}
	else{
		return 0;
	}
}


Sint8 DArray::GetSint8(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetSint8(SubIndex);
	}
	else{
		return 0;
	}
}


bool DArray::GetBit(int Index,int SubIndex){
	if(Index<len){
		return vectors[Index]->GetBit(SubIndex);
	}
	else{
		return 0;
	}
}



