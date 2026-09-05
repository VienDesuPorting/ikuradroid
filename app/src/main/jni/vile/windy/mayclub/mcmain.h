#ifndef _MCMAIN_H_
#define _MCMAIN_H_

#include "../../dialogs/dlgbase.h"

class Mayclub;

class MCMain : public DialogBase {
	private:
		// Dialog widgets
		BitmapButton *w_button_newgame;
		BitmapButton *w_button_loadgame;
		BitmapButton *w_button_album;
		BitmapButton *w_button_exit;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		MCMain(Mayclub *Engine);
};

#endif

