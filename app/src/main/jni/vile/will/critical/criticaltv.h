#ifndef _CRITICALTV_H_
#define _CRITICALTV_H_

#include "../../dialogs/textview.h"

#define CRITICALTV_BACKGROUND		0
#define CRITICALTV_BUTTON_QSAVE		1
#define CRITICALTV_BUTTON_QLOAD		2
#define CRITICALTV_BUTTON_SKIP		3
#define CRITICALTV_BUTTON_AUTO		4
#define CRITICALTV_BUTTON_HISTORY	5
#define CRITICALTV_BUTTON_REPEAT	6
#define CRITICALTV_BUTTON_THING		7
#define CRITICALTV_BUTTON_HEADER	25

class CriticalPoint;

class CriticalTextview : public Textview {
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
		int boxx;		//!< Dialog origin on screen (children are absolute)
		int boxy;
	public:
		CriticalTextview(CriticalPoint *Engine);
		~CriticalTextview();

		void PrintText(uString Title,uString Text);
		void PrintText(uString Text);

		// Event overrides
		virtual bool InputOk(Widget *Object);
};

#endif

