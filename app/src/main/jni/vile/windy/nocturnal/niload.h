#ifndef _NILOAD_H_
#define _NILOAD_H_

#include "../../dialogs/dlgbase.h"

class Nocturnal;

class NILoad : public DialogBase {
	private:
		// Dialog widgets
		SaveSlab *w_slabs[8];
		TextButton *w_exit;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		NILoad(Nocturnal *Engine);
		~NILoad();
};

#endif

