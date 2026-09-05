/*! \class Yume MiruKusuri
 *	\brief Yume Miru Kusuri implementation
 */

#ifndef _YUMEMIRUKUSURI_H_
#define _YUMEMIRUKUSURI_H_

#include "../will.h"
#include "yumetv.h"

class YumeMiruKusuri : public EngineWill {
	private:
	public:
		YumeMiruKusuri(uString Path);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif


