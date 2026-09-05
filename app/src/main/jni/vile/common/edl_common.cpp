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

#include "edl_common.h"

/*! \brief Updates a hash for a byte
 *  \param Hash Previous hash value
 *  \param Byte Byte to calculate hash for
 *  \returns Updated hash value
 */
Uint32 EDL_HashUpdate(Uint32 Hash,Uint8 Byte){
	Uint32 retval=0;
#ifdef VILE_HASH_CCITT16
	// Initialize hash-table on demand
	static Uint16 ccitt_table[256];
	static Uint16 ccitt_init=false;
	static Uint16 ccitt_poly=0x1021;
	if(!ccitt_init){
		for(int i=0;i<256;i++){
			Uint16 tmp=0;
			Uint16 c=((Uint16)i<<8);
			for(int j=0;j<8;j++){
				if((tmp^c)&0x8000){
					tmp=(tmp<<1)^ccitt_poly;
				}
				else{
					tmp=tmp<<1;
				}
				c=c<<1;
			}
			ccitt_table[i]=tmp;
		}
		ccitt_init=true;
	}

	// Calculate checksum
	Uint16 crc=Hash;
	Uint16 sc=0x00FF&Byte;
	Uint16 tmp=(crc>>8)^sc;
	crc=(crc<<8)^ccitt_table[tmp];
	retval=crc;
#endif
#ifdef VILE_HASH_CRC16
	// Initialize hash-table on demand
	static Uint16 table[256];
	static Uint16 init=false;
	static Uint16 poly=0xA001;
	if(!init){
		for(int i=0;i<256;i++){
			Uint16 crc=0;
			Uint16 c=(Uint16)i;
			for(int j=0;j<8;j++){
				if((crc^c)&0x0001){
					crc=(crc>>1)^poly;
				}
				else{
					crc=crc>>1;
				}
				c=c>>1;
			}
			table[i]=crc;
		}
		init=true;
	}

	// Calculate checksum
	Uint16 crc=Hash;
	Uint16 sc=0x00FF&Byte;
	Uint16 tmp=crc^sc;
	crc=(crc>>8)^table[tmp&0xFF];
	retval=crc;
#endif
#ifdef VILE_HASH_CRC32
	// Initialize hash-table on demand
	static Uint32 table[256];
	static Uint16 init=false;
	static Uint32 poly=0xEDB88320L;
	if(!init){
		for(int i=0;i<256;i++){
			Uint32 crc=(Uint32)i;
			for(int j=0;j<8;j++){
				if(crc&0x00000001L){
					crc=(crc>>1)^poly;
				}
				else{
					crc=crc>>1;
				}
			}
			table[i]=crc;
		}
		init=true;
	}

	// Calculate checksum
	Uint32 crc=Hash;
	Uint32 lc=0x00FF&Byte;
	Uint32 tmp=crc^lc;
	crc=(crc>>8)^table[tmp&0xFF];
	retval=crc;
#endif
	return retval;

}

/*! \brief Updates a hash for the pointer to an object
 *  \param Hash Previous hash value
 *  \param Pointer Pointer to hash
 *  \returns Updated hash value
 */
Uint32 EDL_HashUpdate(Uint32 Hash,void *Pointer){
	Uint32 retval=Hash;
	retval=EDL_HashUpdate(retval,(*(Uint32*)Pointer>>24)&0xFF);
	retval=EDL_HashUpdate(retval,(*(Uint32*)Pointer>>16)&0xFF);
	retval=EDL_HashUpdate(retval,(*(Uint32*)Pointer>>8)&0xFF);
	retval=EDL_HashUpdate(retval,*(Uint32*)Pointer&0xFF);
	return retval;
}

/*! \brief Calculates a hash for a string
 *  \param Text String object
 *  \returns Hash of the input string
 */
Uint32 EDL_HashString(uString Text){
	return EDL_HashBuffer((Uint8*)Text.c_str(),Text.length());
}

/*! \brief Calculates a hash for a byte-buffer
 *  \param Buffer Pointer to bytes
 *  \param Length Size of the buffer
 *  \returns Hash of the input
 */
Uint32 EDL_HashBuffer(Uint8 *Buffer,Uint32 Length){
	Uint32 retval=0;
	for(Uint32 i=0;i<Length;i++){
		retval=EDL_HashUpdate(retval,Buffer[i]);
	}
	return retval;
}

/*! \brief Formats a text string (ie. portable sprintf)
 *  \param Text Format string
 */
uString EDL_Format(uString Text,...){
	va_list arg;
	va_start(arg,Text);
	int size=1024;
	char buffer[size];
	vsnprintf(buffer,size,Text.c_str(),arg);
	buffer[size-1]=0;
	va_end(arg);
	return buffer;
}

/*! \brief Converts a string to lower case
 *  \param Text String to convert
 *  \return Inputstring in lower case
 */
uString EDL_Lower(uString Text){
	uString in=Text;
	uString out;
	for(unsigned int i=0;i<Text.length();i++){
		uString s=Text[i];
		if(s.bytes()==1){
			s=(char)tolower(s.c_str()[0]);
		}
		out+=s;
	}
	return out;
}

/*! \brief Converts a string to upper case
 *  \param Text String to convert
 *  \return Inputstring in upper case
 */
uString EDL_Upper(uString Text){
	uString in=Text;
	uString out;
	for(unsigned int i=0;i<Text.length();i++){
		uString s=Text[i];
		if(s.bytes()==1){
			s=(char)toupper(s.c_str()[0]);
		}
		out+=s;
	}
	return out;
}

/*! \brief Trims an unput string by filtering quotes and whitespaces
 *  \param Text String to filter
 *  \param Quotes Wether to drop quites
 *  \param Spaces Wether to drop whitespaces and tabs
 *  \returns Filtered string
 */
uString EDL_Trim(uString Text,bool Quotes,bool Spaces){
	uString retval;
	for(unsigned int i=0;i<Text.length();i++){
		if(Quotes && (Text[i]=='\"' || Text[i]=='\'')){
		}
		else if(Spaces && (Text[i]==' ' || Text[i]=='\t')){
		}
		else{
			retval+=Text[i];
		}
	}
	return retval;
}

/*! \brief Reads current unix time
 *  \return Number of seconds passed since 1. January 1970
 */
time_t EDL_UnixTime(){
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec;
}

/*! \brief Converts unix time to a date string (MM/DD/YYYY)
 *  \param UnixTime Time to convert (Use EDL_UnixTime() for current)
 *  \return Passed date as a string
 */
uString EDL_DateString(const time_t UnixTime){
	struct tm tid=*(gmtime(&UnixTime));
	int y=tid.tm_year+1900;
	int M=tid.tm_mon+1;
	int d=tid.tm_mday;
	char buffer[32];
	sprintf(buffer,"%2.2i/%2.2i/%4.4i",M,d,y);
	return buffer;
}

/*! \brief Converts unix time to a time string (HH:MM)
 *  \param UnixTime Time to convert (Use EDL_UnixTime() for current)
 *  \return Passed time as a string
 */
uString EDL_TimeString(const time_t UnixTime){
	struct tm tid=*(gmtime(&UnixTime));
	int h=tid.tm_hour;
	int m=tid.tm_min;
	char buffer[32];
	sprintf(buffer,"%2.2i:%2.2i",h,m);
	return buffer;
}

/*! \brief Convenience function for converting an integer to a string
 *  \param Integer Integer to convert
 *  \return Converted integer (Blank if failed)
 */
uString EDL_IntString(int Integer){
	char buffer[32];
	sprintf(buffer,"%d",Integer);
	return buffer;
}

/*! \brief Convenience function for converting a double to a string
 *  \param Double Double to convert
 *  \return Converted double (Blank if failed)
 */
uString EDL_DoubleString(double Double){
	char buffer[32];
	sprintf(buffer,"%f",Double);
	return buffer;
}

/*! \brief A digital coin-flipper
 *  \return Random ...
 */
bool EDL_Random(){
	return EDL_Random(0,1);
}

/*! \brief Random number generator
 *  \param Min Minimum number to generate
 *  \param Max Maximum number to generate
 *  \return Random integer within given range
 */
int EDL_Random(int Min,int Max){
	static int seed=-1;
	if(seed<0){
		seed=EDL_UnixTime()/SDL_GetTicks();
		seed=EDL_ABS(seed);
		srand(seed);
	}
	return (rand()%(Max+1))+Min;
}

SDL_Rect EDL_AddRect(SDL_Rect R1,SDL_Rect R2){
	// Check R1
	/*
	if(R1.x<R2.x){
		R2.w+=(R2.x-R1.x);
		R2.x=R1.x;
	}
	if(R1.y<R2.y){
		R2.h+=(R2.y-R1.y);
		R2.y=R1.y;
	}
	if(R1.x+R1.w>R2.x+R2.w){
		R2.w=(R1.x+R1.w)-R2.x;
	}
	if(R1.y+R1.h>R2.y+R2.h){
		R2.h=(R1.y+R1.h)-R2.y;
	}
	*/

	// Check R2
	if(R2.x<R1.x){
		R1.w+=(R1.x-R2.x);
		R1.x=R2.x;
	}
	if(R2.y<R1.y){
		R1.h+=(R1.y-R2.y);
		R1.y=R2.y;
	}
	if(R2.x+R2.w>R1.x+R1.w){
		R1.w=(R2.x+R2.w)-R1.x;
	}
	if(R2.y+R2.h>R1.y+R1.h){
		R1.h=(R2.y+R2.h)-R1.y;
	}
	return R1;
}


