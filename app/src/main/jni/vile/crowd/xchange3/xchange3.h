/*! \class XChange3
 *	\brief Uses the crowd engine to stage a XChange 3 game
 */
#ifndef _XCHANGE3_H_
#define _XCHANGE3_H_

#include "../crowd3.h"

class XChange3 : public EngineCrowd3 {
	private:
		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
		virtual bool ShowSelection(Stringlist List);
		virtual void EventNew();

		// Keep graphics for selections
		SDL_Surface *selitem_normal;
		SDL_Surface *selitem_hover;
	public:
		XChange3(uString Path);
		~XChange3();
};

#endif

