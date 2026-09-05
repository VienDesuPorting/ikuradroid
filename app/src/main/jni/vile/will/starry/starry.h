/*! \class StarrySky
 *	\brief Starry Sky implementation
 */

#ifndef _STARRYSKY_H_
#define _STARRYSKY_H_

#include "../will.h"
#include "starrytv.h"

class StarrySky : public EngineWill {
	private:
	public:
		StarrySky(uString Path);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif


