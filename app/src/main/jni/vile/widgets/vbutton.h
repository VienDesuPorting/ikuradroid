/*! \class ValueButton
 *  \brief Widget with state graphics
 */
#ifndef _VBUTTON_H_
#define _VBUTTON_H_

#include "swidget.h"

class ValueButton : public StateWidget {
	private:
		SDL_Surface *ssurfaces[WIDGET_STATE_SIZE];
		SDL_Surface *usurfaces[WIDGET_STATE_SIZE];
		bool value;
	public:
		ValueButton(SDL_Rect Position);
		ValueButton();
		~ValueButton();
		void SetState(int State,
				SDL_Surface *SSurface,SDL_Rect *SRect,
				SDL_Surface *USurface,SDL_Rect *URect);
		void SetState(int State,
				SDL_Surface *SSurface,int SX,int SY,int SW,int SH,
				SDL_Surface *USurface,int UX,int UY,int UW,int UH);
		void SetValue(bool Value);
		bool GetValue();

		// Overrides
		virtual void ChangeState(WIDGET_STATE State);
};

#endif

