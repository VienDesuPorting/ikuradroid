/*! \class BitmapButton
 *  \brief You run-of-the-mill button-type object
 */
#ifndef _BBUTTON_H_
#define _BBUTTON_H_

#include "swidget.h"

class BitmapButton : public StateWidget {
	private:
		SDL_Surface *surfaces[WIDGET_STATE_SIZE];
		int state;
	public:
		BitmapButton(SDL_Rect Position);
		BitmapButton(int X,int Y,int Widht,int Height);
		BitmapButton();
		~BitmapButton();

		// For setting custom graphics
		void SetState(int State,SDL_Surface *Src,SDL_Rect *SRect);
		void SetState(int State,SDL_Surface *Src,int X,int Y,int W,int H);

		// Autogeneration
		void SetCaption(uString String);

		// Overrides
		virtual void ChangeState(WIDGET_STATE State);
};

#endif

