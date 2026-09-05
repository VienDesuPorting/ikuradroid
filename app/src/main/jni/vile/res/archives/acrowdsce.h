/*! \class ArchiveCrowdSCE
 *	\brief Handles Crowd engine 3 scenario files
 *
 *	The SCE format is not really an archive, but an entire game in scripted
 *	form. This class will decrypt the entire file, keep it in RAM, and extract
 *	each subscript by name as it was a real archive.
 */
#ifndef _ACROWDSCE_H_
#define _ACROWDSCE_H_

#include "abase.h"

class ArchiveCrowdSCE : public ArchiveBase {
	private:
		char *curbuffer;		//!< Holds the decrypted scripts
		int curlength;			//!< Length of the entire script collection
		void Decrypt(char *Buffer,int Length);
		int ParseIndex(char *Buffer,int Length);
	public:
		ArchiveCrowdSCE(uString Path);
		~ArchiveCrowdSCE();
		int Open(uString Archive);
		RWops *GetResource(uString Name);
};

#endif

