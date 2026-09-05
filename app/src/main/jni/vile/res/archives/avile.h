/*! \class ArchiveViLE
 *	\brief Extracts resources from the common ViLE format
 */
#ifndef _AVILE_H_
#define _AVILE_H_

#include "abase.h"

class ArchiveViLE : public ArchiveBase {
	public:
		ArchiveViLE(uString Path);
};

#endif

