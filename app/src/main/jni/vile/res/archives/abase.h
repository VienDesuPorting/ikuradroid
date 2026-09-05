/*! \class ArchiveBase
 *	\brief Defines an archive interface
 *
 *	There are two ways to use this class; Derived classes will index all
 *	available items when opening a resource as well as overriding the
 *	GetResource() method to assure that the underlying source properly
 *	populates the returning blob.
 *
 *	Client classes will usually call the GetResource() method to get a
 *	given resource by name. It is the callers responebility to free the
 *	resource when done with it.
 *
 *  The CSize and DSize values are for archives that uses inline compression,
 *  such as JAST USA Memorial Collection which has zipped bitmaps that doesnt
 *  really make sense until they are decompressed.
 *
 *	NextPtr is not used internally except for setting it to NULL at start
 */
#ifndef _ABASE_H_
#define _ABASE_H_

#include "aitem.h"
#include "../rwops.h"
#include "../../common/edl_fs.h"
#include "../../common/stringlist.h"
#include "../../common/dstack.h"

#define MAX_FILENAME_LENGTH		32

class ArchiveBase {
	private:
		// Binary search tree for data
		RedBlackTree *items;
		int count;
	protected:
		// An API for caching and searching indexes
		void AddItem(uString Name,int Offset,int CSize,int DSize);
		bool GetItem(uString Name,int &Offset,int &CSize,int &DSize);
		void AddItem(uString Name,int Offset,int Size);
		bool GetItem(uString Name,int &Offset,int &Size);

		// Real path to source archive
		uString path;
	public:
		ArchiveBase(uString Path);
		virtual ~ArchiveBase();

		// Generic Archive API
		virtual RWops *GetResource(uString Name);
		uString GetFilename(uString Name);
		int Enumerate(Stringlist *List);
		int GetCount();

		//! Optional pointer that can be used for stacking
		ArchiveBase *NextPtr;
};

#endif

