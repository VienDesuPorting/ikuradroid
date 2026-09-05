#ifndef _MCLOAD_H_
#define _MCLOAD_H_

#include "../../dialogs/dlgbase.h"

class Mayclub;

class MCLoad : public DialogBase {
	private:
		// Dialog widgets
		Sprite *w_page;
		Sprite *w_hover;
		SaveSlab *w_slabs[8];
		Hotspot *w_pages[5];
		Hotspot *w_exit;
		int page;

		// Override events
		virtual bool FocusEnter(Widget *Object);
		virtual bool FocusLeave(Widget *Object);
		virtual bool InputOk(Widget *Object);
	public:
		MCLoad(Mayclub *Engine);
		~MCLoad();
		void LoadPage(int Page);
};

#endif

