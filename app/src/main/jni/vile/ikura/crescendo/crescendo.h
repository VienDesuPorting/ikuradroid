/*! \class Crescendo
 *  \brief Loader class for the Ikuragame
 */
#ifndef _CRESCENDO_H_
#define _CRESCENDO_H_

#include "../ikuradecoder.h"

class Crescendo : public IkuraDecoder {
	private:
		// Override game attributes
		virtual const uString NativeID();
		virtual const uString NativeName();

		// Override opcode handlers
		virtual bool iop_wp(const Uint8 *Data,int Length);
	public:
		Crescendo(uString Path);
};

#endif

