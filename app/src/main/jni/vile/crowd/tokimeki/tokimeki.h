/*! \class Tokimeki
 *	\brief Uses the crowd engine to stage a Tokimeki Check-in! game
 */
#ifndef _TOKIMEKI_H_
#define _TOKIMEKI_H_

#include "../crowd1.h"

class Tokimeki : public EngineCrowd1 {
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
		Tokimeki(uString Path);
		~Tokimeki();
};

#endif

