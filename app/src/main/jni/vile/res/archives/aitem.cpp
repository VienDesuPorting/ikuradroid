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

#include "aitem.h"

/*! \brief Creates an compressed item and parses the content
 *  \param Path Path to resource
 *  \param Offset Offset in archive
 *  \param CSize Compressed size in archive
 *  \param DSize Decompressed size
 */
ArchiveItem::ArchiveItem(uString Path,int Offset,int CSize,int DSize){
	Set(Path,Offset,CSize,DSize);
}

ArchiveItem::ArchiveItem(){
	offset=0;
	csize=0;
	dsize=0;
}

ArchiveNil::ArchiveNil() : ArchiveItem() {
}

ArchiveRoot::ArchiveRoot() : ArchiveItem() {
	int rootsize=128;
	char rootbuffer[rootsize+1];
	memset(rootbuffer,'}',rootsize);
	rootbuffer[rootsize]=0;
	base=rootbuffer;
	ext=base;
}

/*! \brief Creates an compressed item and parses the content
 *  \param Path Path to resource
 *  \param Offset Offset in archive
 *  \param CSize Compressed size in archive
 *  \param DSize Decompressed size
 */
void ArchiveItem::Set(uString Path,int Offset,int CSize,int DSize){
	// Parse and convert name to lower case
	int basestart=0;
	int baseend=0;
	Path=EDL_Lower(Path);
	for(int i=Path.length();i>0;i--){
		if(Path[i-1]=='.'){
			baseend=i-1;
		}
		else if(Path[i-1]=='/' || Path[i-1]=='\\'){
			basestart=i;
			break;
		}
	}

	// Set values
	offset=Offset;
	csize=CSize;
	dsize=DSize;
	dir=Path.substr(0,basestart);
	if(baseend){
		base=Path.substr(basestart,baseend-basestart);
		ext=Path.substr(baseend+1);
	}
	else{
		base=Path.substr(basestart);
		ext="";
	}
}


/*! \brief Compares two string items
 *  \param Leave Object to compare with
 *  \param Mode With or without extension
 *  \return NULL with match
 */
int ArchiveItem::Compare(RedBlackLeave *Leave,int Mode){
	int retval=base.compare(((ArchiveItem*)Leave)->base);
	if(!retval && Mode==SORT_STRICT){
		retval=ext.compare(((ArchiveItem*)Leave)->ext);
	}
	return retval;
}

