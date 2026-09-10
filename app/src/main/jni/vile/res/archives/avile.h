/*! \class ArchiveViLE
 *	\brief Extracts resources from the common ViLE format
 */
#ifndef _AVILE_H_
#define _AVILE_H_

#include "abase.h"

class ArchiveViLE : public ArchiveBase {
	public:
		ArchiveViLE(uString Path);
		ArchiveViLE(const unsigned char *Data,int Size);
		virtual RWops *GetResource(uString Name);
	private:
		void IndexPack(const unsigned char *Data,int Size);
		const unsigned char *memdata;
};

#endif

