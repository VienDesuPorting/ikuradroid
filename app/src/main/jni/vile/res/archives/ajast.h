/*! \class ArchiveJAST
 *	\brief Extracts resources from JAST USA Memorial Collection file cabinets
 */
#ifndef _AJAST_H_
#define _AJAST_H_

#include <zlib.h>
#include "abase.h"

class ArchiveJAST : public ArchiveBase {
	public:
		ArchiveJAST(uString Path);
		RWops *GetResource(uString Name);
};

#endif

