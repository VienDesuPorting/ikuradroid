#ifndef _MCALBUM_H_
#define _MCALBUM_H_

#include "../../dialogs/dlgbase.h"

class Mayclub;

enum ALBUM_MODE {
	AM_CG,
	AM_STORY,
	AM_INVALID
};

class MCAlbum : public DialogBase {
	private:
		// Dialog widgets
		Sprite *w_hover;
		Widget *w_modedsp;
		BitmapButton *w_modebtn;
		BitmapButton *w_exit;
		ValueButton *w_bgm[11];
		ValueButton *w_pages[5];
		Hotspot *w_items[16];
		Widget *w_itembg;
		ALBUM_MODE mode;
		int page;

		// Override events
		virtual bool FocusEnter(Widget *Object);
		virtual bool FocusLeave(Widget *Object);
		virtual bool InputOk(Widget *Object);
	public:
		MCAlbum(Mayclub *Engine);
		~MCAlbum();


		void SetMode(ALBUM_MODE Mode);
		void SetPage(int Page);
};

#endif

