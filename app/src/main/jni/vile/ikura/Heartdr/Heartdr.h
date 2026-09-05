/*! \class Heartdr
 *  \brief Loader class for the Ikuragame
 */
#ifndef _HEARTDR_H_
#define _HEARTDR_H_

#include "../ikuradecoder.h"

class Heartdr : public IkuraDecoder {
	private:
		// Override game attributes
		virtual const uString NativeID();
		virtual const uString NativeName();

		// Override opcode handlers
		virtual bool iop_wp(const Uint8 *Data,int Length);
	public:
		Heartdr(uString Path);
};

#endif

