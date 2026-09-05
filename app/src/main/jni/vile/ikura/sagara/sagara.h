/*! \class Sagara
 *  \brief Loader class for Sagara Family 
 */
#ifndef _SAGARA_H_
#define _SAGARA_H_

#include "../ikuradecoder.h"

class Sagara : public IkuraDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();

		// Macro for fancy textview
		void CreateTextview();
	public:
		Sagara(uString Path);
};

#endif

