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

#include "resources.h"
#include "../common/config.h"

Resources::Resources(){
	stack=0;
}

Resources::~Resources(){
	// Release indexed archives
	while(stack){
		ArchiveBase *tmpptr=stack->NextPtr;
		delete stack;
		stack=tmpptr;
	}
}

/*! \brief Autodetects and opens an archive
 *  \param Path Path to archive resource
 *
 *  Game implementations should preopen the archive manually before adding
 *  it to assert archive type and reduce the overhead at boot-time. This is
 *  intended as a convenience function for the commandline tools.
 */
int Resources::AddResource(uString Path){
	uString Filename=EDL_Realname(Path);
	ArchiveBase *testarc=0;

    if(!testarc){
		testarc=new ArchiveViLE(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a ViLE archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#ifdef VILE_SUPPORT_WINDY
	// Attempt to open the archive as windy format
	if(!testarc){
		testarc=new ArchiveWindy(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Windy archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}

	// Attempt to open the archive in system21 format
	if(!testarc){
		testarc=new ArchiveS21(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a System21 archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_JAST
	// Attempt to open the archive in JAST USA format
	if(!testarc){
		testarc=new ArchiveJAST(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a JAST archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_CWARE
	// Attempt to open the archive in CWare format
	if(!testarc){
		testarc=new ArchiveCWareDL1(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a CWare DL1 archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
	if(!testarc){
		testarc=new ArchiveCWareARC2(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as CWare ARC2",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_WILL
	// Attempt to open the archive in WILL formats
	if(!testarc){
		testarc=new ArchiveWillARC(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Will ARC archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_TLOVE
	// Attempt to open the archive as True love cabinet
	if(!testarc){
		testarc=new ArchiveTLove(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Truelove archive",Filename.c_str());
		}
		else{
			if(testarc) delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_IKURA
	// Attempt to open the archive as IkuraGDL format
	//! \todo Improve type-testing of IkuraGDL format
	if(!testarc){
		testarc=new ArchiveIkura(Filename);
				LogError("Opened %s as a ArchiveIkura",Filename.c_str());
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a IkuraGDL archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

#ifdef VILE_SUPPORT_CROWD
	// Attempt to open the archive as Crowd SCE format
	//! \todo Improve type-testing of Crowd SCE format
	if(!testarc){
		testarc=new ArchiveCrowdSCE(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Crowd SCE archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}

	// Attempt to open the archive as Crowd PCK format
	if(!testarc){
		testarc=new ArchiveCrowdPCK(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Crowd PCK archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}

	// Attempt to open the archive as Crowd SNN format
	if(!testarc){
		testarc=new ArchiveCrowdSNN(Filename);
		if(testarc->GetCount()){
			LogVerbose("Opened %s as a Crowd SNN archive",Filename.c_str());
		}
		else{
			if(testarc)	delete testarc;
			testarc=0;
		}
	}
#endif

	// Attempt to open as native ViLE cabinet


	// Register as file
	if(!testarc){
		testarc=new ArchiveFiles(Filename);
	}
	return AddResource(testarc);
}


/*! \brief Registers a preloaded archive
 *  \param Archive Archive object to register
 */
int Resources::AddResource(ArchiveBase *Archive){
	int retval=Archive?Archive->GetCount():0;
	if(retval){
		if(stack){
			ArchiveBase *tmpptr=stack;
			while(tmpptr->NextPtr){
				tmpptr=tmpptr->NextPtr;
			}
			tmpptr->NextPtr=Archive;
		}
		else{
			stack=Archive;
		}
		Archive->NextPtr=0;
	}
	else if(Archive){
		delete Archive;
	}
	return retval;
}

/*! \brief Get the best available resource
 *	\param Name of the desired resource (With extension if applicable
 *	\returns RWops with raw data
 *
 *	This method will try to locate the given resource in a prioritized
 *	sequence. The returned data will be in its raw form, and should usually
 *	be decoded by a specialized method in one of the derived classes.
 */
RWops *Resources::GetResource(uString Name){
	// Search stacked archives
	RWops *retval=0;
	ArchiveBase *tmpptr=stack;
	while(retval==0 && tmpptr){
		retval=tmpptr->GetResource(Name);
		tmpptr=tmpptr->NextPtr;
	}
	return retval;
}

/*! \brief Writes out the allocated contents to a ViLE cabinet
 *  \param Archive Filename to write (Existing files will be overwritten)
 *  \return True if any resources was written to the archive
 */
bool Resources::Write(uString Archive){
	bool retval=false;
	Stringlist list;
	LogMessage("Write start");
	if(EnumerateResources(&list)>0){
		int count=list.GetCount();
	    LogMessage("count %d ",count);

		FILE *file=fopen(Archive.c_str(),"wb");
		if(file){
			// Write temporary header
			unsigned int length=16+(32*list.GetCount());
			unsigned char *table=new unsigned char[length];
			memset(table,0,length);
			strcpy((char*)table,"ViLEPACK");
			table[12]=length&0xFF;
			table[13]=(length>>8)&0xFF;
			table[14]=(length>>16)&0xFF;
			table[15]=(length>>24)&0xFF;
			fwrite(table,length,1,file);

			// TODO: Write items
			unsigned int offset=length;
			for(int i=0;i<count;i++){
				uString name=list.GetString(i);
				RWops *resource=GetResource(list.GetString(i));
				if(resource && resource->Size()){
					// Format the name for the table
					uString n=EDL_FileName(name);
					uString e=EDL_FileExtension(name);
					if(e.length()){
						n+=".";
						n+=e;
					}
					if(n.length()>24){
						n=n.substr(0,24);
					}

					// Store resource
					unsigned int size=resource->Size();
					unsigned char *buffer=new unsigned char[size];
					if(resource->Read(buffer,size)>0){
						// Store position and size
						strcpy((char*)table+(i*32)+16,n.c_str());
						table[(i*32)+16+24]=offset&0xFF;
						table[(i*32)+16+25]=(offset>>8)&0xFF;
						table[(i*32)+16+26]=(offset>>16)&0xFF;
						table[(i*32)+16+27]=(offset>>24)&0xFF;
						table[(i*32)+16+28]=size&0xFF;
						table[(i*32)+16+29]=(size>>8)&0xFF;
						table[(i*32)+16+30]=(size>>16)&0xFF;
						table[(i*32)+16+31]=(size>>24)&0xFF;

						// Update output file
						LogMessage("Storing %s from %d to %d",
								name.c_str(),offset,offset+size);
						fwrite(buffer,size,1,file);
						offset+=size;
					}
					else{
						strcpy((char*)table+(i*32)+16,"ERROR");
					}
					delete [] buffer;
					delete resource;
				}
				else{
					LogError("Invalid resource %d: %s",i,name.c_str());
				}
			}

			// TODO: Wrap up cabinet
			fseek(file,0,SEEK_SET);
			fwrite(table,length,1,file);
			fclose(file);
			delete [] table;
		}
		else{
			LogError("Failed to write: %s",Archive.c_str());
		}
	}
	else{
		LogError("No resources to write");
	}
	return retval;
}

/*! \brief Enumerates all loaded archives
 *  \param Pointer to stringlist for the results
 *  \return Number of archives
 */
int Resources::EnumerateArchives(Stringlist *List){
	*List=list;
	return list.GetCount();
}

/*! \brief Enumerates all available resources
 *  \param Pointer to stringlist for the results
 *  \return Number of resources added
 */
int Resources::EnumerateResources(Stringlist *List){
	// Iterate stacked archives
	int retval=0;
	ArchiveBase *tmpptr=stack;
	while(tmpptr){
		retval+=tmpptr->Enumerate(List);
		tmpptr=tmpptr->NextPtr;
	}
	return retval;
}

SDL_Surface *Resources::GetImage(uString Name){
	// Get resource and convert it to an image
	unsigned char h[4];
	SDL_Surface *retval=0;
	RWops *blob=GetResource(Name);
	if(blob && blob->Read(h,4)>(int)0){
		// Reseek for SDL_image autodetection
		blob->Seek(0,SEEK_SET);

#ifdef VILE_SUPPORT_CROWD
		// Check for Crowd CWP content
		if(!retval && h[0]=='C' && h[1]=='W' && h[2]=='D' && h[3]=='P'){
			retval=CCrowd::cwp(blob);
		}
		if(!retval && h[0]==0x53 && h[1]==0x5A && h[2]==0x44 && h[3]==0x44){
			retval=CCrowd::zbm(blob);
		}
#endif

#ifdef VILE_SUPPORT_IKURA
		// Check for Ikura content
		if(!retval && h[0]==0xB9 && h[1]==0xAA && h[2]==0xB3 && h[3]==0xB3){
			retval=CIkura::ggd_24b(blob);
		}
		if(!retval && h[0]==0xAB && h[1]==0xAD && h[2]==0xAA && h[3]==0xBA){
			retval=CIkura::ggd_24b(blob);
		}
		if(!retval && h[0]==0xB7 && h[1]==0xB6 && h[2]==0xB8 && h[3]==0xB7){
			retval=CIkura::ggd_24b(blob);
		}
		if(!retval && h[0]==0xCD && h[1]==0xCA && h[2]==0xC9 && h[3]==0xB8){
			retval=CIkura::ggd_8b(blob);
		}
		if(!retval && h[0]==0x47 && h[1]==0x47 && h[2]==0x41 && h[3]==0x30){
			retval=CIkura::gga_32b(blob);
		}
		if(!retval && h[0]==0x47 && h[1]==0x47 && h[2]=='P' && h[3]=='F'){
			retval=CIkura::ggp_32b(blob);
		}
#endif

#ifdef VILE_SUPPORT_WINDY
		// Check for Windy content
		if(!retval && h[0]==0x53 && h[1]==0x5A && h[2]==0x44 && h[3]==0x44){
			retval=CWindy::bet_32b(blob);
		}
#endif

#ifdef VILE_SUPPORT_CWARE
		// Check for CWare content
		if(!retval && h[0]==0x4C && h[1]==0x5A){
			retval=CCWare::lz_32b(blob);
		}
#endif

#ifdef VILE_SUPPORT_WILL
		// Check for Will content
		if(!retval && h[0]==0x57 && h[1]==0x49 && h[2]==0x50 && h[3]==0x46){
			retval=CWill::wip(blob);
		}
#endif

#ifdef VILE_SUPPORT_TLOVE
		// Check for Truelove content
		if(!retval && h[0]==0x43 && h[1]==0x44 && h[2]==0x00 && h[3]==0x00){
			retval=CTLove::mrs(blob);
		}
		if(!retval && h[0]==0x43 && h[1]==0x4F && h[2]==0x00 && h[3]==0x00){
			retval=CTLove::mrs(blob);
		}
#endif

		// Check for standard formats
		if(!retval){
			retval=IMG_Load_RW(blob->ops,0);
		}
	}

	// Free input
	if(blob){
		delete blob;
	}
	return retval;
}

SDL_Surface **Resources::GetAnimation(uString Name){
	unsigned char h[4];
	SDL_Surface **retval=0;
	RWops *blob=GetResource(Name);
	if(blob && blob->Read(h,4)>(int)0){
		blob->Seek(0,SEEK_SET);
#ifdef VILE_SUPPORT_IKURA
		if(!retval && h[0]=='G' && h[1]=='A' && h[2]=='N' && h[3]=='M'){
			retval=CIkura::gan(blob);
		}
#endif

#ifdef VILE_SUPPORT_WILL
		if(!retval && h[0]==0x57 && h[1]==0x49 && h[2]==0x50 && h[3]==0x46){
			retval=CWill::wipa(blob);
		}
#endif
	}

	if(blob){
		delete blob;
	}

	return retval;
}

