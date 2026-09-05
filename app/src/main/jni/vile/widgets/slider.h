/*! \class Slider
 *  \brief Allows the user to set a numeric value in a range
 */
#ifndef _SLIDER_H_
#define _SLIDER_H_

#include "swidget.h"

class Slider : public StateWidget {
	private:
		SDL_Surface *full[WIDGET_STATE_SIZE];
		SDL_Surface *empty[WIDGET_STATE_SIZE];
		bool focus;
		int value;
	public:
		Slider(SDL_Rect Position);
		~Slider();

		// Slider API
		void SetGraphics(WIDGET_STATE State,
				SDL_Surface *SFull,SDL_Rect *RFull,
				SDL_Surface *SEmpty,SDL_Rect *REmpty);
		void AutoGenerate();
		void SetValue(int Value);
		int GetValue();

		// Overrides
		virtual void ChangeState(WIDGET_STATE State);
		virtual bool MouseMove(int X,int Y);
		virtual bool MouseLeftDown(int X,int Y);
		virtual bool MouseLeftUp(int X,int Y);
};

#endif

