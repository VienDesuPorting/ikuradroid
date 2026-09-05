/*! \class ArchiveS21
 *	\brief Extracts resources from S21 file cabinets
 */
#ifndef _AS21_H_
#define _AS21_H_

#include "abase.h"

class ArchiveS21 : public ArchiveBase {
	public:
		ArchiveS21(uString Path);
};

#endif

