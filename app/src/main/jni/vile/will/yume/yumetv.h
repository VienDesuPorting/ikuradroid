#ifndef _YUMETV_H_
#define _YUMETV_H_

#include "../../dialogs/textview.h"

#define YUMETV_BACKGROUND		0
#define YUMETV_BUTTON_QSAVE		1
#define YUMETV_BUTTON_QLOAD		2
#define YUMETV_BUTTON_SKIP		3
#define YUMETV_BUTTON_AUTO		4
#define YUMETV_BUTTON_HISTORY	5	
#define YUMETV_BUTTON_REPEAT	6
#define YUMETV_BUTTON_THING		7
#define YUMETV_BUTTON_HEADER	25

class YumeMiruKusuri;

class YumeTextview : public Textview {
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
		YumeTextview(YumeMiruKusuri *Engine);
		~YumeTextview();

		void PrintText(uString Title,uString Text);
		void PrintText(uString Text);

		// Event overrides
		virtual bool InputOk(Widget *Object);
};

#endif

