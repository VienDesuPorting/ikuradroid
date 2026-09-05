/*! \class KanaOkaeri
 *  \brief Loader class for the Ikuragame
 */
#ifndef _KANAOKA_H_
#define _KANAOKA_H_

#include "../ikuradecoder.h"

class KanaOkaeri : public IkuraDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();
	public:
		KanaOkaeri(uString Path);
};

#endif

