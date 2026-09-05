/*! \class ArchiveItem
 *  \brief Red-black leave for an archive item
 *
 *  See RedBlack classes for further details on sorting/stacking, and refer to
 *  the ArchiveBase class for the management.
 */
#ifndef _ARCHIVEITEM_H_
#define _ARCHIVEITEM_H_

#include "../../common/edl_common.h"
#include "../../common/redblack.h"
#include "../../common/log.h"

#define SORT_STRICT		1
#define SORT_NOEXT		2

class ArchiveItem : public RedBlackLeave {
	private:
		virtual int Compare(RedBlackLeave *Leave,int Mode);
	public:
		// Constructors
		ArchiveItem(uString Path,int Offset,int CSize,int DSize);
		ArchiveItem();

		// Helper for configuring existing items
		void Set(uString Path,int Offset,int CSize,int DSize);

		// Object data
		uString dir;			//!< Directory (if resource is a file)
		uString base;			//!< Resource filename (without extension)
		uString ext;			//!< Resource extension (if any)
		int offset;				//!< Resource offset in archive
		int csize;				//!< Size of compressed resource
		int dsize;				//!< Size of decompressed resource
};

/*! \class ArchiveRoot
 *  \brief Convenience class for instancing a root red/black object
 */
class ArchiveRoot : public ArchiveItem {
	public:
		ArchiveRoot();
};

/*! \class ArchiveNil
 *  \brief Convenience class for instancing a nil red/black object
 */
class ArchiveNil : public ArchiveItem {
	public:
		ArchiveNil();
};

#endif

