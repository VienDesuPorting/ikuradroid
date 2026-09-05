#ifndef _NIREMENISCE_H_
#define _NIREMENISCE_H_

#include "../../dialogs/dlgbase.h"

class Nocturnal;

enum ALBUM_MODE {
	AM_CG,
	AM_MUSIC,
	AM_SELECT
};

class NIRemenisce : public DialogBase {
	private:
		// Dialog widgets
		TextButton *w_back;
		TextButton *w_menu[2];
		ValueButton *w_bgm[18];
		ValueButton *w_items[11];
		ALBUM_MODE mode;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		NIRemenisce(Nocturnal *Engine);
		~NIRemenisce();
		void SetModeSelect();
		void SetModeCG();
		void SetModeMusic();
};

#endif

