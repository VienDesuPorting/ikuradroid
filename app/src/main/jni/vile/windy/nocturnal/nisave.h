#ifndef _NISAVE_H_
#define _NISAVE_H_

#include "../../dialogs/dlgbase.h"

class Nocturnal;

class NISave : public DialogBase {
	private:
		// Dialog widgets
		SaveSlab *w_slabs[8];
		TextButton *w_exit;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		NISave(Nocturnal *Engine);
		~NISave();
};

#endif

