/*! \class Checkbox
 *  \brief Widget with state graphics
 */
#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "vbutton.h"

class Checkbox : public ValueButton {
	private:
		bool leftsided;
		uString caption;
		void autogenerate();
	public:
		Checkbox(SDL_Rect Position,uString Caption,bool Leftsided=true);
		Checkbox(uString Caption,bool Leftsided=true);
		void SetCaption(uString Caption);
		void SetOrientation(bool Leftsided);

		// Overrides
		virtual bool KeyDown(SDL_Keycode Key);
		virtual bool MouseLeftDown(int X,int Y);
};

#endif

