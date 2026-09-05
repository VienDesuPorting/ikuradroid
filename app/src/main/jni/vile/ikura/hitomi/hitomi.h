/*! \class Hitomi
 *  \brief Loader class for Hitomi, My Stepsister
 */
#ifndef _HITOMI_H_
#define _HITOMI_H_

#include "../ikuradecoder.h"

class Hitomi : public IkuraDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();

		// Macro for fancy textview
		void CreateTextview();
	public:
		Hitomi(uString Path);
};

#endif

