/*! \class Idols
 *  \brief Loader class for Idols Galore!
 */
#ifndef _IDOLS_H_
#define _IDOLS_H_

#include "../ikuradecoder.h"

class Idols : public IkuraDecoder {
	private:
		// Override game attributes
		const uString NativeID();
		const uString NativeName();

		// Macro for fancy textview
		void CreateTextview();
	public:
		Idols(uString Path);
};

#endif

