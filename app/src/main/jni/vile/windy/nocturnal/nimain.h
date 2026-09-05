#ifndef _NIMAIN_H_
#define _NIMAIN_H_

#include "../../dialogs/dlgbase.h"

class Nocturnal;

class NIMain : public DialogBase {
	private:
		// Dialog widgets
		TextButton *w_button_newgame;
		TextButton *w_button_loadgame;
		TextButton *w_button_album;
		TextButton *w_button_exit;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		NIMain(Nocturnal *Engine);
};

#endif

