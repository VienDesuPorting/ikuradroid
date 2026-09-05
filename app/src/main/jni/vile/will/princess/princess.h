/*! \class PrincessWaltz
 *	\brief Yume Miru Kusuri implementation
 */

#ifndef _PRINCESSWALTZ_H_
#define _PRINCESSWALTZ_H_

#include "../will.h"


class PrincessWaltz : public EngineWill {
	private:
	public:
		PrincessWaltz(uString Path);

		// Overrides
		virtual bool OP21();				//!< Play music
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif


