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

#include "ustring.h"
#include <vector>
#include <ctype.h>
#include "edl_common.h"
#include "log.h"

// Include utf8cpp backend
#if VILE_UNICODE_SAFE
#include "../external/utf8cpp/checked.h"
#endif
#include "../external/utf8cpp/unchecked.h"


// ANSI constructors
uString::uString(){
	cbytes=0;
	cpoints=0;
}

uString::uString(const aString *Src){
	buffer=CHARTOUTF8((const Uint8*)Src->c_str(),cbytes,cpoints);
}

uString::uString(const char *Src){
	buffer=CHARTOUTF8((const Uint8*)Src,cbytes,cpoints);
}

uString::uString(const char &Src){
	aString tmp;
	tmp+=Src;
	buffer=CHARTOUTF8((const Uint8*)tmp.c_str(),cbytes,cpoints);
}

uString::uString(const uString &Src){
	cbytes=Src.cbytes;
	cpoints=Src.cpoints;
	buffer=Src.buffer;
}

uString::uString(const aString &Src){
	buffer=CHARTOUTF8((const Uint8*)Src.c_str(),cbytes,cpoints);
}

// ANSI operators
uString & uString::operator=(const char *Src) {
	aString tmp=Src;
	buffer=CHARTOUTF8((const Uint8*)tmp.c_str(),cbytes,cpoints);
	return *this;
}

uString & uString::operator=(const char &Src) {
	Uint8 tmp[2]={Src,0};
	buffer=CHARTOUTF8((const Uint8*)tmp,cbytes,cpoints);
	return *this;
}

uString & uString::operator=(const uString &Src) {
	cbytes=Src.cbytes;
	cpoints=Src.cpoints;
	buffer=Src.buffer;
	return *this;
}

uString & uString::operator=(const aString &Src) {
	buffer=CHARTOUTF8((const Uint8*)Src.c_str(),cbytes,cpoints);
	return *this;
}

uString & uString::operator+=(const char *Src) {
	Uint32 tbytes,tpoints;
	buffer+=CHARTOUTF8((Uint8*)Src,tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString & uString::operator+=(const char &Src) {
	Uint32 tbytes,tpoints;
	Uint8 tmp[2]={Src,0};
	buffer+=CHARTOUTF8(tmp,tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString & uString::operator+=(const aString &Src) {
	Uint32 tbytes,tpoints;
	buffer+=CHARTOUTF8((Uint8*)Src.c_str(),tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString & uString::operator+=(const uString &Src) {
	buffer+=Src.buffer;
	cbytes+=Src.cbytes;
	cpoints+=Src.cpoints;
	return *this;
}

uString uString::operator+(const char *Src) {
	aString temporary=buffer;
	temporary+=Src;
	return temporary.c_str();
}

uString uString::operator+(const char &Src) {
	Uint8 tmp[2]={Src,0};
	uString temporary=buffer;
	temporary+=(char*)tmp;
	return temporary.c_str();
}

uString uString::operator+(const uString &Src) {
	aString temporary=buffer;
	temporary+=Src.buffer;
	return temporary.c_str();
}

bool uString::operator==(const char *Src) {
	return *this==uString(Src);
}

bool uString::operator==(const char &Src) {
	return *this==uString(Src);
}

bool uString::operator==(const aString &Src) {
	return *this==uString(Src);
}

bool uString::operator==(const uString &Src) {
	return buffer==Src.buffer;
}

bool uString::operator!=(const char *Src) {
	return *this!=uString(Src);
}

bool uString::operator!=(const char &Src) {
	return *this!=uString(Src);
}

bool uString::operator!=(const aString &Src) {
	return *this!=uString(Src);
}

bool uString::operator!=(const uString &Src) {
	return !(*this==Src);
}



#ifdef VILE_BUILD_UNICODE
// UNICODE constructors
uString::uString(const wString *Src){
	buffer=WIDETOUTF8(Src->c_str(),cbytes,cpoints);
}

uString::uString(const wchar_t *Src){
	buffer=WIDETOUTF8(Src,cbytes,cpoints);
}

uString::uString(const wchar_t &Src){
	wString tmp;
	tmp+=Src;
	buffer=WIDETOUTF8(tmp.c_str(),cbytes,cpoints);
}

uString::uString(const wString &Src){
	buffer=WIDETOUTF8(Src.c_str(),cbytes,cpoints);
}

// UNICODE operators
uString & uString::operator=(const wchar_t *Src) {
	buffer=WIDETOUTF8(Src,cbytes,cpoints);
	return *this;
}

uString & uString::operator=(const wchar_t &Src) {
	wString tmp;
	tmp+=Src;
	buffer=WIDETOUTF8(tmp.c_str(),cbytes,cpoints);
	return *this;
}

uString & uString::operator=(const wString &Src) {
	buffer=WIDETOUTF8(Src.c_str(),cbytes,cpoints);
	return *this;
}

uString & uString::operator+=(const wchar_t *Src) {
	Uint32 tbytes,tpoints;
	buffer+=WIDETOUTF8(Src,tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString & uString::operator+=(const wchar_t &Src) {
	Uint32 tbytes,tpoints;
	wString tmp;
	tmp+=Src;
	buffer+=WIDETOUTF8(tmp.c_str(),tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString & uString::operator+=(const wString &Src) {
	Uint32 tbytes,tpoints;
	buffer+=WIDETOUTF8(Src.c_str(),tbytes,tpoints);
	cbytes+=tbytes;
	cpoints+=tpoints;
	return *this;
}

uString uString::operator+(const wchar_t *Src) {
	uString temporary=buffer;
	temporary+=Src;
	return temporary.c_str();
}

uString uString::operator+(const wchar_t &Src) {
	wchar_t src[8]={0};
	src[0]=Src;
	uString temporary=buffer;
	temporary+=src;
	return temporary.c_str();
}

bool uString::operator==(const wchar_t *Src) {
	return *this==uString(Src);
}

bool uString::operator==(const wchar_t &Src) {
	return *this==uString(Src);
}

bool uString::operator==(const wString &Src) {
	return *this==uString(Src);
}

bool uString::operator!=(const wchar_t *Src) {
	return *this!=uString(Src);
}

bool uString::operator!=(const wchar_t &Src) {
	return *this!=uString(Src);
}

bool uString::operator!=(const wString &Src) {
	return *this!=uString(Src);
}

#endif


/*! \brief Find first occurrance of Text
 *  \param Text Text to search for
 *  \param Start Start index
 *  \param Length Maximum number of characters to search for
 *  \return Starting index of found text or negative if failed
 */
int uString::find_first(const uString &Text,int Start,int Length) const{
	int li=points();
	int le=Text.points();
	if(Start<li && le>0){
		const char *bi=buffer.c_str();
		const char *be=Text.buffer.c_str();
		li-=Start;
		Length=EDL_MIN(Length,li);
		if(Length<0){
			Length=li;
		}
		if(Start>0){
			utf8::unchecked::advance(bi,Start);
		}
		int matched=0;
		for(int i=0;i<Length;i++){
			Uint32 ui=utf8::unchecked::next(bi);
			Uint32 ue=utf8::unchecked::next(be);
			if(ui==ue){
				if(++matched==le){
					return Start+i;
				}
			}
			else{
				matched=0;
			}
		}
	}
	return -1;
}

/*! \brief Find last occurrance of Text
 *  \param Text Text to search for
 *  \param Start Start index
 *  \param Length Maximum number of characters to search for
 *  \return Starting index of found text or negative if failed
 *
 *  Iterating a UNICODE string is slow by nature. Using the find_first()
 *  and find_last() methods minimizes reiteration and constant loading of
 *  new string of string objects so it is recommended over the [] operators.
 */
int uString::find_last(const uString &Text,int Start,int Length) const{
	int li=points();
	int le=Text.points();
	if(Start<li && le>0){
		// Find boundries and buffer pointers
		const char *bi=buffer.c_str();
		const char *be=Text.buffer.c_str();
		li-=Start;
		if(Length<0){
			Length=li;
		}

		// Search for start of word
		utf8::unchecked::advance(bi,li);
		Length=EDL_MIN(Length,li);
		for(int i=0;i<Length;i++){
			Uint32 ui=utf8::unchecked::prior(bi);
			Uint32 ue=utf8::unchecked::prior(be);
			if(ui==ue){
				const char *ti=bi;
				const char *te=be;
				int matched=0;
				for(int j=0;j<le;j++){
					if(++matched==le){
						return Start+i;
					}
					Uint32 ui=utf8::unchecked::next(ti);
					Uint32 ue=utf8::unchecked::next(te);
					if(ui!=ue){
						break;
					}
				}
			}
		}
	}
	return -1;
}

/*! \brief Extracts substring
 *  \param Start Start index
 *  \param Length Length of string to extract
 *  \return Extracted string (Empty if failed)
 */

uString uString::substr(int Start,int Length) const {
    uString retval;
	int count=cpoints;
	if(Length<0){
		// Autolength
		Length=count;
	}

	if(0 && cbytes==cpoints){
		// Optimized standard
		retval=buffer.substr(Start,Length);
	}
	else if(Start>=count){
		// Out of boundry request
	}
	else{
		// Extract substring
		count-=Start;
		Length=EDL_MIN(count,Length);
		for(int i=0;i<Length;i++){
			retval+=(*this)[i+Start];
		}
	}
	return retval;
}

int uString::compare(const uString &str) const{
	if(0 && cbytes==cpoints){
		return buffer.compare(str.buffer);
	}
	else{
		int c1=cpoints;
		int c2=str.cpoints;
int t1=cbytes;
int t2=str.cbytes;
if((t1!=c1 || t2!=c2) && t1!=128 && t2!=128){
LogError("DIFF:%d/%d\t%d/%d  (%x)",t1,c1,t2,c2,buffer.c_str()[0]);
exit(0);
}
		const char *s1=buffer.c_str();
		const char *s2=str.buffer.c_str();
		int count=EDL_MIN(c1,c2);
		for(int i=0;i<count;i++){
			Uint32 u1=utf8::unchecked::next(s1);
			Uint32 u2=utf8::unchecked::next(s2);
			if(u1>u2){
				return 1;
			}
			if(u1<u2){
				return -1;
			}
		}
		//return c2-c1;
		return c1-c2;
	}
}

/*! \brief Picks an indexed character
 *  \param Index Index of character
 *  \return New uString object with single character
 */
uString uString::operator[](const unsigned int Index) const {
	if(0 && cbytes==cpoints){
		// Use optimized standard for ansi strings
		return buffer[Index];
	}
	else{
		// Manually parse utf8 data
		const char *bp=buffer.c_str();
		uString retval;
		if(Index<points()){
			if(Index>0){
				utf8::unchecked::advance(bp,Index);
			}
			char u[5]={0,0,0,0,0};
			utf8::unchecked::append(utf8::unchecked::next(bp),u);
			retval=u;
		}
		return retval;
	}
}

/*! \brief Counts number of characters in the string
 *  \return Number of characters in the string
 */
unsigned int uString::length() const {
	return cpoints;
}

/*! \brief Counts number of bytes in the string
 *  \return Number of bytes in the string
 */
unsigned int uString::bytes() const {
	return cbytes;
}

/*! \brief Counts number of characters in the string
 *  \return Number of characters in the string
 */
unsigned int uString::points() const {
	return cpoints;
}

/*! \brief Counts number of characters in a utf string
 *  \return Number of characters in the string
 */
unsigned int uString::points(const aString Text) const {
	if(Text.length()){
		aString text=Text;
		aString::iterator end_it=utf8::find_invalid(text.begin(),text.end());
#if VILE_UNICODE_SAFE
		return utf8::distance(text.begin(),end_it);
#else
		return utf8::unchecked::distance(text.begin(),end_it);
#endif
	}
	else{
		return 0;
	}
}

/*! \brief Extracts UTF8 encoded text string
 *  \return UTF8 encoded text string
 */
const char *uString::c_str() const {
	return buffer.c_str();
}

/*! \brief Extracts text string and converts it to UTF16
 *  \return UTF16 encoded text string
 */
const wchar_t *uString::c_wstr() {
	const char *in=buffer.c_str();
#ifdef VILE_ARCH_MICROSOFT
	std::vector<Uint16> out;
	utf8::unchecked::utf8to16(in,in+buffer.length(),back_inserter(out));
#else
	std::vector<Uint32> out;
	utf8::unchecked::utf8to32(in,in+buffer.length(),back_inserter(out));
#endif
	wbuffer=std::wstring(out.begin(),out.end());
	return wbuffer.c_str();
}


/*! \brief Gets ordinal of first character
 *  \return 32 Unicode codepoint
 */
Uint32 uString::to_code() const{
	if(points()){
		const char *ptr=buffer.c_str();
		return utf8::unchecked::next(ptr);
	}
	else{
		return 0;
	}
}

uString uString::to_lower() const{
	//! \todo UNICODE compliant case-conversion
	uString tmp;
	uString retval;
	int count=points();
	for(int i=0;i<count;i++){
		tmp=(*this)[i];
		if(tmp.length()==1){
			tmp=(char)tolower(tmp.c_str()[0]);
		}
		retval+=tmp;
	}
	return retval;
}

uString uString::to_upper() const{
	//! \todo UNICODE compliant case-conversion
	uString tmp;
	uString retval;
	int count=points();
	for(int i=0;i<count;i++){
		tmp=(*this)[i];
		if(tmp.length()==1){
			tmp=(char)toupper(tmp.c_str()[0]);
		}
		retval+=tmp;
	}
	return retval;
}

double uString::to_double() const{
	//! \todo UNICODE compliant string -> double conversion
	char *endptr,*str;
	str=(char*)buffer.c_str();
	double retval=strtod(str,&endptr);
	if(endptr==str){
		LogVerbose("Failed to convert double: %s",str);
	}
	return retval;
}

int uString::to_int(int Base) const{
	//! \todo UNICODE compliant string -> int conversion
	char *endptr,*str;
	str=(char*)buffer.c_str();
	Uint32 retval=strtol(str,&endptr,Base);
	if(endptr==str){
		LogVerbose("Failed to convert integer: %s",str);
	}
	return retval;
}

aString uString::UTF32TOUTF8(const Uint32 *Src,const unsigned int Length){
	aString retval;
	if(Length>0 && Length<16*1024){
		std::vector<char> out;
#if VILE_UNICODE_SAFE
		utf8::utf32to8(Src,Src+Length,back_inserter(out));
#else
		utf8::unchecked::utf32to8(Src,Src+Length,back_inserter(out));
#endif
		retval=aString(out.begin(),out.end());
	}
	return retval;
}


aString uString::UTF16TOUTF8(const Uint16 *Src,const unsigned int Length){
	aString retval;
	if(Length>0 && Length<16*1024){
		std::vector<char> out;
#if VILE_UNICODE_SAFE
		utf8::utf16to8(Src,Src+Length,back_inserter(out));
#else
		utf8::unchecked::utf16to8(Src,Src+Length,back_inserter(out));
#endif
		retval=aString(out.begin(),out.end());
	}
	return retval;
}

aString uString::UTF8TOUTF8(const Uint8 *Src,const unsigned int Length){
	aString retval;
	if(Length>0 && Length<16*1024){
		aString in=std::string((char*)Src,Length);
		std::string::iterator end_it=utf8::find_invalid(in.begin(),in.end());
		retval=aString(in.begin(),end_it);
		if(end_it!=in.end()) {
			LogError("Invalid UTF8 data found");
			uString dumpin,dumpout;
			for(unsigned int i=0;i<in.length();i++){
				dumpin+=EDL_Format("%02X ",in[i]&0xFF);
			}
			for(unsigned int i=0;i<retval.length();i++){
				dumpout+=EDL_Format("%02X ",retval[i]&0xFF);
			}
			LogError("\tIn : %s (%db)",dumpin.c_str(),Length);
			LogError("\tOut: %s (%db)",dumpout.c_str(),retval.length());
		}
	}
	return retval;
}

aString uString::WIDETOUTF8(const wchar_t *Src,Uint32 &Bytes,Uint32 &Points){
	wString src=Src;
#ifdef VILE_ARCH_MICROSOFT
	aString dst=UTF16TOUTF8((Uint16*)Src,src.length());
#else
	aString dst=UTF32TOUTF8((Uint32*)Src,src.length());
#endif
	Points=points(dst);
	Bytes=dst.length();
	return dst;
}

aString uString::CHARTOUTF8(const Uint8 *Src,Uint32 &Bytes,Uint32 &Points){
#ifdef VILE_BUILD_UNICODE
//#if 0
	aString src=(char*)Src;
	aString dst=UTF8TOUTF8(Src,src.length());
	Points=points(dst);
	Bytes=dst.length();
	return dst;
#else
	aString s=(char*)Src;
	Points=Bytes=s.length();
	return s;
#endif
}


