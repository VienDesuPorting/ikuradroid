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

#include "afiles.h"


/*! \brief Opens one or more files and pretends its an archive
 *  \param Pathname Path to files (Wildcard supported)
 *
 *  This method will expand wildcarded patterns and add matching files to
 *  the archive. All pathnames are case insensitive to make autodetection
 *  on unix systems feasable.
 *
 *  Please note that there is no defined behaviour for handling directories,
 *  only files are supported input.
 */
ArchiveFiles::ArchiveFiles(uString Path) : ArchiveBase(Path){
	Stringlist list=EDL_Expandname(Path);
	int items=list.GetCount();
	for(int i=0;i<items;i++){
		uString pathname=list.GetString(i);
		FILE *tf=fopen(pathname.c_str(),"rb");
		if(tf){
			fseek(tf,0,SEEK_END);
			int size=ftell(tf);
			fclose(tf);
			if(size>0){
				AddItem(pathname,0,size);
			}
		}
	}
}

RWops *ArchiveFiles::GetResource(uString Name){
	RWops *retval=0;
	int offset,size;
	if(GetItem(Name,offset,size)){
		// Parse real path
		uString path=EDL_Realname(GetFilename(Name));

		// Open file as a blob
		retval=new RWops();
		if(!retval->OpenFile(path.c_str(),"rb")){
			delete retval;
			retval=0;
		}
	}
	return retval;
}


