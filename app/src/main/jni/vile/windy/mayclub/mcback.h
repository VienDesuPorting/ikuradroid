#ifndef _MCBACK_H_
#define _MCBACK_H_

#include "../../dialogs/textview.h"

class Mayclub;

class MCBack : public DialogBase {
	private:
		// Dialog widgets
		BitmapButton *w_back;
		BitmapButton *w_stop;
		BitmapButton *w_next;
		Textview *texter;
		int current;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		MCBack(Mayclub *Engine,Textview *Texter);
		~MCBack();
};

#endif

