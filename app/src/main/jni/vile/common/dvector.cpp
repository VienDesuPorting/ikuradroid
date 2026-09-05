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

#include "dvector.h"

/*! \brief Creates a new DVector and copy the content from an existing one
 *  \param DVector Source vector
 */
DVector::DVector(const DVector &Src){
	reg=0;
	len=0;
	(*this)=Src;
}

/*! \brief Creates a new vector with a preallocated start size
 *  \param Preload Number of bytes to preallocate
 *
 *  The preallocated bytes are all set to NULL. You can preallocate a smaller
 *  or bigger buffer, but dynamic growing/and schrinking the buffer takes a
 *  lot of time and resources so a "correct" size is much more efficient.
 */
DVector::DVector(int Preload){
	if(Preload){
		len=Preload;
		reg=new Uint8[len];
		for(Uint32 i=0;i<len;i++){
			reg[i]=0;
		}
	}
	else{
		reg=0;
		len=0;
	}
}

DVector::~DVector(){
	if(reg){
		delete [] reg;
	}
}

/*! \brief Copy operator
 *  \param Src Source DVector object
 */
DVector& DVector::operator=(const DVector &Src) {
	if(this!=&Src){
		// Drop own reg and copy source
		if(reg){
			delete reg;
		}
		len=Src.len;
		reg=new Uint8[len];
		for(Uint32 i=0;i<len;i++){
			reg[i]=Src.reg[i];
		}
	}
	return *this;
}

/*! \brief Provides direct access to the internal buffer
 *  \param Buffer Pointer to internal buffer if any
 *  \param Length Length of internal buffer
 *  \return True if an internal buffer was found
 */
bool DVector::GetBuffer(Uint8 **Buffer,Uint32 *Length){
	if(reg){
		*Buffer=reg;
		*Length=len;
	}
	return reg;
}

/*! \brief Copies passed buffer to internal values
 *  \param Buffer Source data
 *  \param Length of source data
 *  \return True if source buffer was copied
 */
bool DVector::SetBuffer(Uint8 *Buffer,Uint32 Length){
	if(Buffer && Length){
		if(reg){
			delete [] reg;
		}
		reg=new Uint8[Length];
		len=Length;
		for(Uint32 i=0;i<Length;i++){
			reg[i]=Buffer[i];
		}
	}
	return Buffer;
}

/*! \brief Reads a single bit from the buffer
 *  \param Pos Bitposition to read
 *  \returns Value of indexed bit (False if position is invalid)
 */
bool DVector::GetBit(Uint32 Pos){
	// Calculate positions
	Uint32 bytepos=Pos/8;
	Uint32 bitpos=Pos%8;
	if(bytepos>=len){
		return false;
	}
	else{
		return reg[bytepos]&(1<<bitpos);
	}
}

/*! \brief Reads a signed integer from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Sint32 DVector::GetSint32(Uint32 Pos){
	if(len && (Pos+1)/4<=len){
		Sint32 *tmp=(Sint32*)reg;
		return tmp[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Reads an unsigned integer from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Uint32 DVector::GetUint32(Uint32 Pos){
	if(len && (Pos+1)/4<=len){
		Uint32 *tmp=(Uint32*)reg;
		return tmp[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Reads a signed word from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Sint16 DVector::GetSint16(Uint32 Pos){
	if(len && (Pos+1)/2<=len){
		Sint16 *tmp=(Sint16*)reg;
		return tmp[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Reads an unsigned word from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Uint16 DVector::GetUint16(Uint32 Pos){
	if(len && (Pos+1)/2<=len){
		Uint16 *tmp=(Uint16*)reg;
		return tmp[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Reads a signed byte from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Sint8 DVector::GetSint8(Uint32 Pos){
	if(Pos<len){
		return (Sint8)reg[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Reads an unsigned byte from the buffer
 *  \param Pos Position to read
 *  \returns Read value (Null if failed)
 */
Uint8 DVector::GetUint8(Uint32 Pos){
	if(Pos<len){
		return reg[Pos];
	}
	else{
		return 0;
	}
}

/*! \brief Sets a single bit
 *  \param Pos Bitposition to change
 *  \param Value New bit value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetBit(Uint32 Pos,bool Value){
	// Calculate positions
	Uint32 bytepos=Pos/8;
	Uint32 bitpos=Pos%8;

	// Assert buffer
	if(!reg){
		// Create buffer from scratch
		reg=new Uint8[bytepos+1];
		for(Uint32 i=0;i<=bytepos;i++){
			reg[i]=0;
		}
		len=bytepos+1;
	}
	else if(bytepos>=len){
		// Resize buffer
		Uint8 *tmpbit=new Uint8[bytepos+1];
		for(Uint32 i=0;i<len/8;i++){
			tmpbit[i]=reg[i];
		}
		for(Uint32 i=len/8;i<=bytepos;i++){
			tmpbit[i]=0;
		}

		// Switch
		delete [] reg;
		reg=tmpbit;
		len=bytepos+1;
	}

	// Set bitvalue
	if(Value){
		reg[bytepos]|=(1<<bitpos);
	}
	else{
		reg[bytepos]&=~(1<<bitpos);
	}
}

/*! \brief Sets the value of a signed byte
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetSint8(Uint32 Pos,Sint8 Value){
	SetUint8(Pos,(Uint8)Value);
}

/*! \brief Sets the value of a signed word
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetSint16(Uint32 Pos,Sint16 Value){
	SetUint16(Pos,(Uint16)Value);
}

/*! \brief Sets the value of a signed integer
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetSint32(Uint32 Pos,Sint32 Value){
	SetUint32(Pos,(Uint32)Value);
}

/*! \brief Sets all values to NULL
 */
void DVector::Clear(){
	for(Uint32 i=0;i<len;i++){
		reg[i]=0;
	}
}

/*! \brief Sets the value of an unsigned byte
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetUint8(Uint32 Pos,Uint8 Value){
	if(!len){
		// Create buffer from scratch
		reg=new Uint8[Pos+1];
		for(Uint32 i=0;i<=Pos;i++){
			reg[i]=0;
		}
		len=Pos+1;
	}
	else if(Pos>=len){
		// Resize buffer
		Uint8 *tmp8=new Uint8[Pos+1];
		for(Uint32 i=0;i<len;i++){
			tmp8[i]=reg[i];
		}
		for(Uint32 i=len;i<=Pos;i++){
			tmp8[i]=0;
		}

		// Switch
		delete [] reg;
		reg=tmp8;
		len=Pos+1;
	}
	reg[Pos]=Value;
}

/*! \brief Sets the value of an unsigned word
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetUint16(Uint32 Pos,Uint16 Value){
	Uint16 *reg16=(Uint16*)reg;
	Uint32 len16=len/2;
	if(!len16){
		// Create buffer from scratch
		reg16=new Uint16[Pos+1];
		for(Uint32 i=0;i<=Pos;i++){
			reg16[i]=0;
		}
		len16=Pos+1;
	}
	else if(Pos>=len16){
		// Resize buffer
		Uint16 *tmp16=new Uint16[Pos+1];
		for(Uint32 i=0;i<len16;i++){
			tmp16[i]=reg16[i];
		}
		for(Uint32 i=len16;i<=Pos;i++){
			tmp16[i]=0;
		}

		// Switch
		delete [] reg;
		reg16=tmp16;
		len16=Pos+1;
	}

	// Set register
	reg16[Pos]=Value;
	reg=(Uint8*)reg16;
	len=len16*2;
}

/*! \brief Sets the value of an unsigned integer
 *  \param Pos Position to change
 *  \param Value New value
 *
 *  Buffer will automatically grow to fit the position!
 */
void DVector::SetUint32(Uint32 Pos,Uint32 Value){
	Uint32 *reg32=(Uint32*)reg;
	Uint32 len32=len/4;
	if(!len32){
		// Create buffer from scratch
		reg32=new Uint32[Pos+1];
		for(Uint32 i=0;i<=Pos;i++){
			reg32[i]=0;
		}
		len32=Pos+1;
	}
	else if(Pos>=len32){
		// Resize buffer
		Uint32 *tmp32=new Uint32[Pos+1];
		for(Uint32 i=0;i<len32;i++){
			tmp32[i]=reg32[i];
		}
		for(Uint32 i=len32;i<=Pos;i++){
			tmp32[i]=0;
		}

		// Switch
		delete [] reg;
		reg32=tmp32;
		len32=Pos+1;
	}

	// Set register
	reg32[Pos]=Value;
	reg=(Uint8*)reg32;
	len=len32*4;
}


