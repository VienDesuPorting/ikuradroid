/*! \class Virgin
 *  \brief Loader class for the Ikuragame
 */
#ifndef _VIRGIN_H_
#define _VIRGIN_H_

#include "../ikuradecoder.h"

class virgin: public IkuraDecoder {
	private:
		// Override game attributes
		virtual const uString NativeID();
		virtual const uString NativeName();

		// Override opcode handlers
		virtual bool iop_pcend(const Uint8 *Data,int Length);
	public:
		virgin(uString Path);
};

#endif

