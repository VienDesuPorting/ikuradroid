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

#include "abase.h"

ArchiveBase::ArchiveBase(uString Path){
	// Create top- and bottom items for the tree
	items=new RedBlackTree(new ArchiveNil(),new ArchiveRoot());
	path=EDL_Realname(Path);
	count=0;

	// Unset external pointer
	NextPtr=0;
}

ArchiveBase::~ArchiveBase(){
	delete items;
}

/*! \brief Indexes an archive item
 *  \param Name Name of the resource to index
 *  \param Offset Byte offset in the archive
 *  \param Size Size in archive
 */
void ArchiveBase::AddItem(uString Name,int Offset,int Size){
	ArchiveItem *i=new ArchiveItem(Name,Offset,Size,Size);
	items->Insert(i,SORT_STRICT);
	count++;
}

/*! \brief Indexes an archive item
 *  \param Name Name of the resource to index
 *  \param Offset Byte offset in the archive
 *  \param CSize Compressed image size (i.e. physical size in file)
 *  \param DSize Decompressed size (i.e. size of final resource)
 */
void ArchiveBase::AddItem(uString Name,int Offset,int CSize,int DSize){
	ArchiveItem *i=new ArchiveItem(Name,Offset,CSize,DSize);
	items->Insert(i,SORT_STRICT);
	count++;
}

/*! \brief Retrieves a previously indexed resource item from the archive
 *  \param Name Name of the resource to get
 *  \param Offset Byte offset in the archive
 *  \param Size Compressed Physical size in file
 */
bool ArchiveBase::GetItem(uString Name,int &Offset,int &Size){
	bool retval=false;
	ArchiveItem s(Name,0,0,0);
	ArchiveItem *r=(ArchiveItem*)items->Search(&s,SORT_STRICT);
	if(!r){
		r=(ArchiveItem*)items->Search(&s,SORT_NOEXT);
	}
	if(r){
		Offset=r->offset;
		Size=r->csize;
		retval=true;
	}
	return retval;
}

/*! \brief Retrieves a previously indexed resource item from the archive
 *  \param Name Name of the resource to get
 *  \param Offset Byte offset in the archive
 *  \param CSize Compressed image size (i.e. physical size in file)
 *  \param DSize Decompressed size (i.e. size of final resource)
 */
bool ArchiveBase::GetItem(uString Name,int &Offset,int &CSize,int &DSize){
	bool retval=false;
	ArchiveItem s(Name,0,0,0);
	ArchiveItem *r=(ArchiveItem*)items->Search(&s,SORT_STRICT);
	if(!r){
		r=(ArchiveItem*)items->Search(&s,SORT_NOEXT);
	}
	if(r){
		Offset=r->offset;
		CSize=r->csize;
		DSize=r->dsize;
		retval=true;
	}
	return retval;
}

RWops *ArchiveBase::GetResource(uString Name){
	RWops *retval=0;
	int offset,size;
	if(GetItem(Name,offset,size)){
		retval=new RWops();
		if(!retval->OpenFile(path.c_str(),offset,size)){
			delete retval;
			retval=0;
		}
	}
	return retval;
}

/*! \brief Gets physical disk location of resource file
 *  \param Name Name of resource file
 *  \return Full path with directory and extension if available
 *
 *  If resources added with AddItem have directories in their name, this will
 *  be parsed and remembered in the corresponding ArchiveItem so we can pull
 *  it out of the final blob -- Necessary for video resources and ArchiveFiles
 *  objects.
 */
uString ArchiveBase::GetFilename(uString Name){
	uString retval;
	ArchiveItem s(Name,0,0,0);
	ArchiveItem *r=(ArchiveItem*)items->Search(&s,SORT_STRICT);
	if(!r){
		r=(ArchiveItem*)items->Search(&s,SORT_NOEXT);
	}
	if(r){
		if(r->ext.length()){
			retval=r->dir+r->base+uString(".")+r->ext;
		}
		else{
			retval=r->dir+r->base;
		}
	}
	return retval;
}

/*! \brief Enumerates all available resources
 *  \param Pointer to stringlist for the results
 *  \return Number of resources added
 */
int ArchiveBase::Enumerate(Stringlist *List){
	int retval=0;
	int size=items->Count();
	if(size>0){
		RedBlackLeave *buffer[size];
		retval=items->Enumerate(buffer,size);
		for(int i=0;i<retval;i++){
			uString base=((ArchiveItem*)buffer[i])->base;
			uString ext=((ArchiveItem*)buffer[i])->ext;
			if(ext.length()){
				List->AddString(base+uString(".")+ext);
			}
			else if(base.length()){
				List->AddString(base);
			}
		}
	}
	return retval;
}

int ArchiveBase::GetCount(){
	return count;
}


