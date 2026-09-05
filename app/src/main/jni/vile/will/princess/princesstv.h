#ifndef _PRINCESSTV_H_
#define _PRINCESSTV_H_

#include "../../dialogs/textview.h"

#define PRINCESSTV_BACKGROUND		0
#define PRINCESSTV_BUTTON_QSAVE		1
#define PRINCESSTV_BUTTON_QLOAD		2
#define PRINCESSTV_BUTTON_SKIP		3
#define PRINCESSTV_BUTTON_AUTO		4
#define PRINCESSTV_BUTTON_HISTORY	5
#define PRINCESSTV_BUTTON_REPEAT	6
#define PRINCESSTV_BUTTON_THING		7
#define PRINCESSTV_BUTTON_HEADER	25

class PrincessWaltz;

class PrincessTextview : public Textview {
	private:
		// Widgets
		ValueButton *w_save;
		BitmapButton *w_load;
		ValueButton *w_skip;
		ValueButton *w_auto;
		BitmapButton *w_history;
		BitmapButton *w_repeat;
		BitmapButton *w_thing;
		Printer *header;
	public:
		PrincessTextview(PrincessWaltz *Engine);
		~PrincessTextview();

		void PrintText(uString Title,uString Text);
		void PrintText(uString Text);

		// Event overrides
		virtual bool InputOk(Widget *Object);
};

#endif

