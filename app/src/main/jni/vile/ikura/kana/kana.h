/*! \class Kana
 *  \brief Loader class for the drs classic 
 */
#ifndef _KANA_H_
#define _KANA_H_

#include "../drsdecoder.h"

class Kana : public DRSDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();
	public:
		Kana(uString Path);
};

#endif

