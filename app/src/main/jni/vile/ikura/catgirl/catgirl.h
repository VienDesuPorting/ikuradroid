/*! \class CatGirl
 *  \brief Loader class for Cat Girl Alliance
 */
#ifndef _CATGIRL_H_
#define _CATGIRL_H_

#include "../ikuradecoder.h"

class CatGirl : public IkuraDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();

		// Macro for fancy textview
		void CreateTextview();
	public:
		CatGirl(uString Path);
};

#endif

