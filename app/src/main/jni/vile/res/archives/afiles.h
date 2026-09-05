/*! \class ArchiveFiles
 *  \brief Treats a collection of files as an archive
 */
#ifndef _AFILES_H_
#define _AFILES_H_

#include "abase.h"

class ArchiveFiles : public ArchiveBase {
	public:
		ArchiveFiles(uString Path);
		RWops *GetResource(uString Name);
};

#endif

