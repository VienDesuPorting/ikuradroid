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

#include "stringlist.h"

Stringlist::Stringlist(const Stringlist &Source){
	stringlist=0;
	count=0;
	(*this)=Source;
}

/*! \brief Preloads a given number of empty string objects
 *  \param Preload Number of strings to preload
 *
 *  Preloading empty strings gives a hefty speed advantage as you
 *  do not have to reload and the string objects every time you
 *  add a new one.
 */
Stringlist::Stringlist(int Preload){
	if(Preload>0){
		stringlist=new uString[Preload];
		count=Preload;
	}
	else{
		stringlist=0;
		count=0;
	}
}

Stringlist::~Stringlist(){
	if(stringlist){
		delete [] stringlist;
	}
	stringlist=0;
	count=0;
}

Stringlist &Stringlist::operator=(const Stringlist &Source) {
	if(this!=&Source){
		Clear();
		int preload=Source.GetCount();
		stringlist=new uString[preload];
		count=preload;
		for(int i=0;i<Source.count;i++){
			SetString(i,Source.stringlist[i].c_str());
		}
	}
	return *this;
}

/*! \brief Enumerates all strings into a comma separated string
 *  \returns Comma separated string
 */
uString Stringlist::Enumerate(){
	uString retval;
	for(int i=0;i<count;i++){
		retval+=stringlist[i]+",";
	}
	int l=retval.length();
	if(l>0){
		retval=retval.substr(0,l-1);
	}
	return retval;
}

/*! \brief Counts number of strings in the object
 *  \return Number of strings in the object
 */
int Stringlist::GetCount() const{
	return count;
}

/*! \brief Removes all loaded strings
 */
void Stringlist::Clear(){
	if(stringlist){
		delete [] stringlist;
		stringlist=0;
		count=0;
	}
}

/*! \brief Append a string and the end of the list
 *  \param Text Text to add
 */
void Stringlist::AddString(uString Text){
	SetString(count,Text);
}

/*! \brief Adds a list of strings to the end of the list
 *  \param List List of strings to add
 */
void Stringlist::AddStringlist(Stringlist List){
	int length=List.GetCount();
	for(int i=0;i<length;i++){
		SetString(count+i,List.GetString(i));
	}
}

/*! \brief Sets an indexed string
 *  \param Pos Index of the string
 *  \param Text New string
 *
 *  This method will allocate new strings if necessary.
 */
void Stringlist::SetString(int Pos,uString Text){
	if(Pos<count){
		// Replace existing string
		stringlist[Pos]=Text;
	}
	else{
		// Make room for new strings
		uString *newlist=new uString[Pos+1];
		for(int i=0;i<count;i++){
			newlist[i]=stringlist[i];
		}
		newlist[Pos]=Text;
		count=Pos+1;

		// Delete old list
		delete [] stringlist;
		stringlist=newlist;
	}
}

bool Stringlist::GetString(int Pos,uString *Text){
	bool retval=false;
	if(Pos<count){
		*Text=stringlist[Pos];
		retval=true;
	}
	return retval;
}

uString Stringlist::GetString(int Pos){
	uString retval;
	GetString(Pos,&retval);
	return retval;
}

int Stringlist::GetString(uString Text){
	for(int i=0;i<count;i++){
		if(stringlist[i]==Text){
			return i;
		}
	}
	return -1;
}


