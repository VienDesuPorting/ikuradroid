/*! \class ArchiveCrowdPCK
 *	\brief Handles Crowd engine 3 PCK archives
 */
#ifndef _ACROWDPCK_H_
#define _ACROWDPCK_H_

#include "abase.h"

class ArchiveCrowdPCK : public ArchiveBase {
	private:
		int ParseIndex(unsigned char *Buffer,int Length);
	public:
		ArchiveCrowdPCK(uString Path);
};

#endif

