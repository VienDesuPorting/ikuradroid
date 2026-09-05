#ifndef _STARRYTV_H_
#define _STARRYTV_H_

#include "../../dialogs/textview.h"

#define STARRYTV_BACKGROUND		0
#define STARRYTV_BUTTON_QSAVE		1
#define STARRYTV_BUTTON_QLOAD		2
#define STARRYTV_BUTTON_SKIP		3
#define STARRYTV_BUTTON_AUTO		4
#define STARRYTV_BUTTON_HISTORY	5
#define STARRYTV_BUTTON_REPEAT	6
#define STARRYTV_BUTTON_THING		7
#define STARRYTV_BUTTON_HEADER	25

class StarrySky;

class StarryTextview : public Textview {
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
		StarryTextview(StarrySky *Engine);
		~StarryTextview();

		void PrintText(uString Title,uString Text);
		void PrintText(uString Text);

		// Event overrides
		virtual bool InputOk(Widget *Object);
};

#endif

