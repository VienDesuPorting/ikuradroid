/*! \unit File system extensions to SDL
 *  \todo Implement UNICODE compliant filesystem integration
 *
 *  EDL is a ViLE-specific extension of SDL. EDL_FS handles filesystem tasks
 *  in a portable manner, and allows ViLE to check read/write permissions,
 *  get the correct casing of a path, etc.
 */

#ifndef _EDL_FS_H_
#define _EDL_FS_H_

#include "edl_common.h"
#include "stringlist.h"
#ifdef VILE_ARCH_MICROSOFT
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>
#include <stdio.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#ifdef HAVE_WORDEXP_H
#include <wordexp.h>
#include <fnmatch.h>
#endif


// ----- Filename utilities -----
extern uString EDL_StripPath(uString Path);
extern uString EDL_Searchname(uString Filename);
extern uString EDL_FileDirectory(uString Path);
extern uString EDL_FileName(uString Path);
extern uString EDL_FileExtension(uString Path);
extern uString EDL_ForceExtension(uString Path,uString Extension);
extern uString EDL_DefaultExtension(uString Path,uString Extension);


// ----- File utilities -----
extern bool EDL_SetDirectory(uString Path);
extern bool EDL_ReadableDirectory(uString Path);
extern bool EDL_ReadableFile(uString Path);
extern bool EDL_WritableDirectory(uString Path);
extern bool EDL_WritableFile(uString Path);
extern bool EDL_CreateDirectory(uString Path);
extern bool EDL_DeleteDirectory(uString Path);
extern bool EDL_DeleteFile(uString Path);
extern bool EDL_GetFile(uString Pathname,uString *Filename);
extern uString EDL_CreateTemporary();
extern uString EDL_Realname(uString Filename);
extern Stringlist EDL_Expandname(uString Filename);


#endif

