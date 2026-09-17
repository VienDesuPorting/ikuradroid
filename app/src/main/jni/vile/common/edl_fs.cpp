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

#include "edl_fs.h"
#include "config.h"

/*! \brief Expands a filemask to a comma separated list of files
 *  \param Filemask Filemask with relative or absolute path
 *  \return List of files (Empty if none or fail)
 */
Stringlist EDL_Expandname(uString Filemask){
	Stringlist retval;
#if VILE_ARCH_MICROSOFT
	// Extract path
	int i=Filemask.length();
	uString folder;
	for(;i>0;i--){
		if(Filemask[i-1]=='/' || Filemask[i-1]=='\\'){
			break;
		}
	}
	if(i>0){
		folder=Filemask.substr(0,i);
	}

	// Search for files matching input mask
	WIN32_FIND_DATA FindFileData;
	HANDLE findhandle=FindFirstFile(Filemask.c_str(),&FindFileData);
	if(findhandle!=INVALID_HANDLE_VALUE){
		// Handle first file
		bool run=true;
		while(run){
			// Handle found file
			uString filename=FindFileData.cFileName;
			if(filename!="." && filename!=".."){
				retval.AddString(folder+filename);
			}

			// Get next file
			run=FindNextFile(findhandle,&FindFileData);
		}

		// Close search
		FindClose(findhandle);
	}
#elif HAVE_WORDEXP_H
	// Parse foldername and escape any spaces
	int lastsep=-1;
	uString tmp;
	uString folder;
	for(unsigned int i=0;i<Filemask.length();i++){
		if(Filemask[i]=='/'){
			lastsep=tmp.length();
		}
		if(Filemask[i]==' '){
			tmp+="\\";
		}
		tmp+=Filemask[i];
	}

	// Check all files in the given folder
	Filemask=tmp;
	uString mask;
	if(lastsep>=0){
		mask=tmp.substr(0,lastsep+1);
	}
	mask+="*";
	wordexp_t p;
	if(wordexp(mask.c_str(),&p,0)==0){
		for(unsigned int i=0;i<p.we_wordc;i++){
			if(!fnmatch(Filemask.c_str(),p.we_wordv[i],FNM_CASEFOLD)){
				retval.AddString(p.we_wordv[i]);
			}
		}

		// Clean up
		wordfree(&p);
	}

#elif VILE_ARCH_NDS
//#error "File enumeration not supported"
#else
//#error "File enumeration not supported"
#endif
	return retval;
}

/*! \brief Gets proper casing of filenames on a unixsystem
 *  \param Path Input path (Will not be checked)
 *  \param Filename Input filename (Might be a directory)
 *  \return Pathname with correct casing
 *
 *  This method finds casing of a filename in a case sensitive path
 */
uString EDL_Realname(uString Path,uString Filename){
#if HAVE_WORDEXP_H
	// Escape spaces in input string
	uString mask;
	for(unsigned int i=0;i<Path.length();i++){
		if(Path[i]==' '){
			mask+="\\";
		}
		mask+=Path[i];
	}
	if(mask.length() && mask[mask.length()-1]!='/'){
		mask+="/";
	}
	mask+="*";

	// Check case of filenames
	wordexp_t p;
	if(wordexp(mask.c_str(),&p,0)==0){
		// Convert Filename to lower case in order to search for it
		Filename=EDL_Lower(Filename);
		for(unsigned int k=0;k<p.we_wordc;k++){
			// Get next filename and convert to lower case
			int lastsep=-1;
			uString pathname=EDL_Lower(p.we_wordv[k]);
			for(unsigned int l=0;l<pathname.length();l++){
				if(pathname[l]=='/'){
					lastsep=l;
				}
			}
			if(lastsep>=0){
				pathname=pathname.substr(lastsep+1);
			}

			// Check if input file matches
			if(pathname==Filename){
				Filename=p.we_wordv[k];
				if(lastsep>=0){
					Filename=Filename.substr(lastsep+1);
				}
				break;
			}
		}
		wordfree(&p);
	}
#elif VILE_ARCH_MICROSOFT
	// Casing is already ignored under windwoes
#elif VILE_ARCH_NDS
//#error "File case-ignoring not supported"
#else
//#error "File case-ignoring not supported"
#endif
	return Path+Filename;
}

/*! \brief Gets proper casing of filenames on a unixsystem
 *  \param Filename Input filename
 *  \return Pathname with correct casing
 *
 *  Since these games stems from windows they do not not have a convention
 *  or standard on the casing of the resource files. This function takes the
 *  suggested pathname and checks it for real files in the filesystem.
 */
uString EDL_Realname(uString Filename){
#ifdef VILE_ARCH_MICROSOFT
	// Dont bother with real name on a case insensitive system
	return Filename;
#else
	// Check if input already comply
	struct stat ss;
	if(stat(Filename.c_str(),&ss)==0){
		if((ss.st_mode&S_IFMT)==S_IFDIR && Filename[Filename.length()-1]!='/'){
			Filename+="/";
		}
		return Filename;
	}

	// Resolve nested path
	uString relative;
	for(unsigned int i=0;i<Filename.length();i++){
		if(Filename[i]=='*'){
			return Filename;
		}
		if((Filename[i]=='/' && Filename[i+1]!='*') || i==Filename.length()-1){
			// Process initial relative path
			if(relative.length()){
				if(i==Filename.length()-1){
					relative+=EDL_Lower(Filename[i++]);
				}
				uString f1=EDL_Realname("",relative);
				uString f2=Filename.substr(i);
				Filename=f1+f2;
				i=f1.length();
				relative="";
			}

			// Check for nested paths
			uString folder;
			for(unsigned int j=i+1;j<Filename.length();j++){
				if(Filename[j]=='/'){
					// Test next item
					uString statname=Filename.substr(0,i+1)+folder;
					if(stat(statname.c_str(),&ss)==0){
						Filename=statname+Filename.substr(j);
					}
					else{
						uString f1=Filename.substr(0,i+1);
						uString f2=Filename.substr(j);
						Filename=EDL_Realname(f1,folder)+f2;
					}
					i=j-1;
					break;
				}
				else if(j==Filename.length()-1){
					// Assert final item
					if(stat(Filename.c_str(),&ss)!=0){
						folder+=Filename[j];
						uString f1=Filename.substr(0,i+1);
						uString f2=Filename.substr(j+1);
						Filename=EDL_Realname(f1,folder)+f2;
					}
					i=j;
					break;
				}
				else{
					// Extract lowercase foldername
					folder+=Filename[j];
				}
			}
		}
		else{
			// Accumelate relative path
			relative+=EDL_Lower(Filename[i]);
		}
	}

	// Assert trailing separator for directories
	if(stat(Filename.c_str(),&ss)==0){
		if((ss.st_mode&S_IFMT)==S_IFDIR && Filename[Filename.length()-1]!='/'){
			Filename+="/";
		}
	}

	return Filename;
#endif
}


/*! \brief Asserts lower case and drops path and extensions
 *  \param Filename Input filename
 *  \return Filename in lower case without path and extension
 */
uString EDL_Searchname(uString Filename){
	int spos=-1;
	int dpos=-1;
	Filename=EDL_Lower(Filename);
	for(unsigned int i=0;i<Filename.length();i++){
		if(Filename[i]=='.'){
			dpos=i;
		}
		if(Filename[i]=='\\' || Filename[i]=='/'){
			spos=i;
		}
	}
	if(dpos>0){
		Filename=Filename.substr(0,dpos);
	}
	if(spos>0){
		Filename=Filename.substr(spos+1);
	}
	return Filename;
}

/*! \brief Strips quotes and simular fuzz from a commandline path
 *  \param Path Input path
 *  \return Stripped version of Path
 */
uString EDL_StripPath(uString Path){
	// Strip quotes
	while(Path.length() && Path[0]=='"'){
		Path=Path.substr(1,Path.length()-1);
	}
	while(Path.length() && Path[Path.length()-1]=='"'){
		Path=Path.substr(0,Path.length()-1);
	}
	Path=EDL_Realname(Path);
	return Path;
}

/*! \brief Gets one and one file from the specified path
 *  \param Pathname Pathname to search
 *  \param Filename Filename if any
 *  \return True if a(nother) file could be named
 *
 *  Setting the pathname is key, subsequent calls for the same
 *  pathname will render a new file file until all files has been
 *  enumerated. Subsequent calls after a failed call will start over.
 */
// Enumeration state of EDL_GetFile, hoisted to file scope so that
// a stream left open by an interrupted session can be dropped
// before the next engine run probes the same folder (see
// EDL_GetFileReset, called from main()).
static uString curpath="";
#ifdef VILE_ARCH_MICROSOFT
static HANDLE finddir=INVALID_HANDLE_VALUE;
#else
static DIR *finddir=0;
#endif

bool EDL_GetFile(uString Pathname,uString *Filename){
	bool retval=false;
#ifdef VILE_ARCH_MICROSOFT
	if(finddir==INVALID_HANDLE_VALUE || Pathname!=curpath){
		// Find first file
		curpath=Pathname;
		uString filemask=Pathname+"*";
		WIN32_FIND_DATA FindFileData;
		if(finddir!=INVALID_HANDLE_VALUE){
			FindClose(finddir);
		}
		finddir=FindFirstFile(filemask.c_str(),&FindFileData);
		if(finddir!=INVALID_HANDLE_VALUE){
			*Filename=FindFileData.cFileName;
			retval=true;
		}
	}
	else{
		// Find next file
		WIN32_FIND_DATA FindFileData;
		if(FindNextFile(finddir,&FindFileData)){
			*Filename=FindFileData.cFileName;
			retval=true;
		}
		else{
			FindClose(finddir);
			finddir=INVALID_HANDLE_VALUE;
		}
	}
#else
	if(finddir==NULL || Pathname!=curpath){
		if(finddir!=NULL){
			closedir(finddir);
		}
		curpath=Pathname;
		finddir=opendir(curpath.c_str());
	}
	if(finddir!=NULL){
		dirent *entry=readdir(finddir);
		if(entry){
			*Filename=entry->d_name;
			retval=true;
		}
		else{
			closedir(finddir);
			finddir=0;
		}
	}
#endif
	return retval;
}

/*! \brief Drops any interrupted EDL_GetFile enumeration
 *
 *  The enumerator keeps its iteration state in statics; a session
 *  that exits mid-probe used to leak that state into the next
 *  engine run, whose first probe then hit a stale handle and
 *  failed spuriously. main() resets it alongside the
 *  configuration (Cfg::Reset).
 */
void EDL_GetFileReset(void){
	curpath="";
#ifdef VILE_ARCH_MICROSOFT
	if(finddir!=INVALID_HANDLE_VALUE){
		FindClose(finddir);
		finddir=INVALID_HANDLE_VALUE;
	}
#else
	if(finddir!=NULL){
		closedir(finddir);
		finddir=0;
	}
#endif
}

/*! \brief Tests wether a given file is readable
 *  \param Path Path to the believed filename
 *  \return True if the file was readable
 *
 *  This method will return false if the specified file was a directory.
 */
bool EDL_ReadableFile(uString Path){
#ifdef VILE_ARCH_MICROSOFT
	int test=GetFileAttributes(Path.c_str());
	return (test!=-1 && !(test&FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat ss;
	return (stat(Path.c_str(),&ss)==0 && ((ss.st_mode&S_IFMT)!=S_IFDIR));
#endif
}

/*! \brief Tests wether a given file is writable
 *  \param Path Path to the believed filename
 *  \return True if the file was writable
 *
 *  This method will return false if the specified file was a directory.
 */
bool EDL_WritableFile(uString Path){
#ifdef VILE_ARCH_MICROSOFT
	int test=GetFileAttributes(Path.c_str());
	return (test!=-1 &&
			!test&FILE_ATTRIBUTE_DIRECTORY &&
			!test&FILE_ATTRIBUTE_READONLY);
#else
	struct stat ss;
	return (stat(Path.c_str(),&ss)==0 &&
			((ss.st_mode&S_IFMT)!=S_IFDIR) &&
			(ss.st_mode&S_IWUSR));
#endif
}

/*! \brief Changes the current directory (CWD)
 *  \param Path New directory path
 *  \returns True if path was accepted
 */
bool EDL_SetDirectory(uString Path){
#ifdef HAVE_WORDEXP_H
	wordexp_t p;
	if(wordexp(Path.c_str(),&p,0)==0){
		Path=p.we_wordv[0];
		wordfree(&p);
	}
	return (chdir(Path.c_str())==0);
#else
	return (chdir(Path.c_str())==0);
#endif
}

/*! \brief Creates a new directory (duh!)
 *  \param Path Path of new directory
 *  \returns True if directory was successfully created
 */
bool EDL_CreateDirectory(uString Path){
#ifdef HAVE_WORDEXP_H
	// Expand path before changing path
	wordexp_t p;
	if(wordexp(Path.c_str(),&p,0)==0){
		Path=p.we_wordv[0];
		wordfree(&p);
	}
	return (mkdir(Path.c_str(),0777)==0);
#elif VILE_ARCH_MICROSOFT
	return (mkdir(Path.c_str())==0);
#else
	return (mkdir(Path.c_str(),0777)==0);
#endif
}

/*! \brief Tests wether a given directory is readable
 *  \param Path Path to the believed directory
 *  \return True if the directory was readable
 *
 *  This method will return false if the specified directory was a file.
 */
bool EDL_ReadableDirectory(uString Path){
	struct stat ss;
	return (stat(Path.c_str(),&ss)==0 && ((ss.st_mode&S_IFMT)==S_IFDIR));
}

/*! \brief Tests wether a given directory is writable
 *  \param Path Path to the believed directory
 *  \return True if the directory was writeable
 *
 *  This method will return false if the specified directory was a file.
 */
bool EDL_WritableDirectory(uString Path){
	struct stat ss;
	return (stat(Path.c_str(),&ss)==0 &&
			((ss.st_mode&S_IFMT)==S_IFDIR) &&
			(ss.st_mode&S_IWUSR));
}

/*! \brief Deletes a file from the filesystem
 *  \param Path Path to the file to delete
 *  \return True if the file was deleted
 */
bool EDL_DeleteFile(uString Path){
	return (unlink(Path.c_str())==0);
}

/*! \brief Deletes a directory from the filesystem
 *  \param Path Path to the directory to delete
 *  \return True if the directory was deleted
 */
bool EDL_DeleteDirectory(uString Directory){
	bool retval=false;
#ifdef VILE_ARCH_MICROSOFT
	// "Double nullterminate" pathname
	char path_from[Directory.length()+2];
	strncpy(path_from,Directory.c_str(),Directory.length());
	path_from[Directory.length()]=0;
	path_from[Directory.length()+1]=0;

	// Use Windows API to remove the file
	SHFILEOPSTRUCT fileop;
	fileop.hwnd=NULL;
	fileop.wFunc=FO_DELETE;
	fileop.pFrom=path_from;
	fileop.pTo=NULL;
	fileop.fFlags=FOF_NOCONFIRMATION | FOF_SILENT;
	fileop.fAnyOperationsAborted=FALSE;
	fileop.lpszProgressTitle=NULL;
	fileop.hNameMappings=NULL;
	retval=(SHFileOperation(&fileop)==0);
#elif VILE_ARCH_NDS
#warning Directory deletion not supported under Nintendo DS
#else
	// Assert trailing separator and traverse directory
	if(Directory[Directory.length()-1]!='\\'){
		Directory+="/";
	}
	struct dirent *pent=NULL;
	DIR *pdir=opendir(Directory.c_str());
	while(pdir && (pent=readdir(pdir))){
		if(pent==0){
			// Ooops
			break;
		}
		if(!strcmp(pent->d_name,".")){
			// Ignore dots
		}
		else if(!strcmp(pent->d_name,"..")){
			// Ignore dots
		}
		else{
			// Recursively delete item
			uString file=Directory+pent->d_name;
			if(EDL_WritableDirectory(file)){
				EDL_DeleteDirectory(file);
			}
			else{
				EDL_DeleteFile(file);
			}
		}
	}

	// Close traverser and actually delete directory
	closedir(pdir);
	retval=(rmdir(Directory.c_str())==0);
#endif
	return retval;
}

/*! \brief Creates an uniquely named temporary directory
 *  \return Path to new temporary directory
 *
 *  Remember to delete the temporary directory after use!
 */
uString EDL_CreateTemporary(){
	uString retval;
#ifdef VILE_ARCH_MICROSOFT
	TCHAR lpPathBuffer[MAX_PATH];
	DWORD dwRetVal=GetTempPath(MAX_PATH,lpPathBuffer);
	if(dwRetVal<=MAX_PATH)
	{
		// Shift directory to string and get unique filename
		uString path=lpPathBuffer;
		if(GetTempFileName(path.c_str(),"ViLEVN",0,lpPathBuffer)!=0){
			// Create directory
			EDL_DeleteFile(lpPathBuffer);
			if(CreateDirectory(lpPathBuffer,0)!=0){
				retval=lpPathBuffer;
			}
		}
	}
#else
	uString tmps="/var/tmp/";
	if(!EDL_WritableDirectory(tmps)){
		tmps="/tmp/";
		if(!EDL_WritableDirectory(tmps)){
			tmps="";
		}
	}
	tmps+="ViLE-XXXXXX";
	char tmpc[tmps.length()];
	strcpy(tmpc,tmps.c_str());
	retval=mkdtemp(tmpc);
	if(retval.length()){
		retval+="/";
	}
#endif
	return retval;
}

/*! \brief Parses the directory name from a path
 *  \param Path Path with directory and filename
 *  \return Path with filename and extension stripped off
 */
uString EDL_FileDirectory(uString Path){
	uString retval;
	if(EDL_ReadableDirectory(Path)){
		// Given path is already a directory
		retval=Path;
		if(Path[Path.length()-1]!='/' && Path[Path.length()-1]!='\\'){
			retval+="/";
		}
	}
	else{
		// Parse last separator
		for(int i=Path.length()-1;i>=0;i--){
			if(Path[i]=='/' || Path[i]=='\\'){
				retval=Path.substr(0,i+1);
				break;
			}
		}
	}
	return retval;
}

/*! \brief Parses the basename from a path (Without extension)
 *  \param Path Path with optional directory and/or extension
 *  \return Path with directory and extension stripped off
 */
uString EDL_FileName(uString Path){
	uString dirname=EDL_FileDirectory(Path);
	uString extname=EDL_FileExtension(Path);
	uString filename=Path.substr(dirname.length());
	if(extname.length()){
		// Drop extension
		filename=filename.substr(0,filename.length()-(extname.length()+1));
	}
	else if(filename.length()){
		// Drop trailing dot
		if(filename[filename.length()-1]=='.'){
			return filename.substr(0,filename.length()-1);
		}
	}
	return filename;
}

/*! \brief Strips directory and basename from the given path
 *  \param Path Path to be stripped
 *  \return File extension (Without leading dot)
 */
uString EDL_FileExtension(uString Path){
	// Parse last separator
	uString retval;
	for(int i=Path.length()-1;i>=0;i--){
		if(Path[i]=='/' || Path[i]=='\\'){
			break;
		}
		if(Path[i]=='.'){
			retval=Path.substr(i+1);
			break;
		}
	}
	return retval;
}

/*! \brief Enforces a new extension even if path already has one
 *  \param Path Filename with optional directory
 *  \param Extension New extension
 *  \return Path with new extension
 */
uString EDL_ForceExtension(uString Path,uString Extension){
	if(Path.length()){
		for(unsigned int i=Path.length();i>0;i--){
			if(Path[i-1]=='.'){
				// Change existing extension
				Path=Path.substr(0,i-1);
				if(Path[Path.length()-1]!='.' && Extension[0]!='.'){
					Path+=".";
				}
				Path+=Extension;
				break;
			}
			if(Path[i-1]=='/' || Path[i-1]=='\\'){
				if(i==Path.length()){
					// Append extension
					if(Path[Path.length()-1]!='.' && Extension[0]!='.'){
						Path+=".";
					}
					Path+=Extension;
				}
				break;
			}
		}
		uString oldext=EDL_FileExtension(Path);
		if(oldext==""){
			if(Path[Path.length()-1]!='.' && Extension[0]!='.'){
				Path+=".";
			}
			Path+=Extension;
		}
	}
	return Path;
}

/*! \brief Sets extension if none is previously set
 *  \param Filename with optional directory
 *  \param Extension New extension
 *  \return Path with asserted extension
 */
uString EDL_DefaultExtension(uString Path,uString Extension){
	if(Path.length() && Extension.length()){
		uString oldext=EDL_FileExtension(Path);
		if(oldext==""){
			if(Path[Path.length()-1]!='.' && Extension[0]!='.'){
				Path+=".";
			}
			Path+=Extension;
		}
	}
	return Path;
}

