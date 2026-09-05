/*! \class CriticalPoint
 *	\brief Critical Point implementation
 *
 *  This implementation is just to test backwards compability of EngineWill.
 *
 *  The compability seems pretty horrible at first glance.
 */

#ifndef _CRITICAL_H_
#define _CRITICAL_H_

#include "../will.h"

class CriticalPoint : public EngineWill {
	private:
	public:
		CriticalPoint(uString Path);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif


