#ifndef _MAYCLUBTV_H_
#define _MAYCLUBTV_H_

#include "../../dialogs/textview.h"

class Mayclub;

class MCTextview : public Textview {
	private:
		// Widgets
		BitmapButton *w_load;
		BitmapButton *w_save;
		BitmapButton *w_back;
		BitmapButton *w_skip;

		// Cache graphics
		SDL_Surface *sbuttons;
		SDL_Surface *swindow;

		// Helpers for mapping the screen
		bool GetTimeofday(Uint16 TOD,SDL_Rect *R);
		bool GetDayofweek(Uint16 DOW,SDL_Rect *R);
		bool GetDigit(Uint16 Number,SDL_Rect *R);
	public:
		MCTextview(Mayclub *Engine);
		~MCTextview();

		// Specialized methods
		bool SetTickets(Uint16 Tickets);
		bool SetTime(Uint16 Month,Uint16 Day,Uint16 TOD,Uint16 DOW);

		// Event overrides
		virtual bool InputOk(Widget *Object);
};

#endif

