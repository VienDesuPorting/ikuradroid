/*! \class XChange1
 *	\brief Uses the crowd engine to stage a XChange 1 game
 */
#ifndef _XCHANGE1_H_
#define _XCHANGE1_H_

#include "../crowd1.h"

class XChange1 : public EngineCrowd1 {
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
		XChange1(uString Path);
		~XChange1();
};

#endif

