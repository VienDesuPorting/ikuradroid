/*! \class StdMenu
 *  \brief Standard (ie. simple!) drop down menu
 */
#ifndef _STDMENU_H_
#define _STDMENU_H_

#include "dlgbase.h"

enum STDMENU_ITEMS {
	SI_NEW,
	SI_LOAD,
	SI_SAVE,
	SI_OPTIONS,
	SI_TITLE,
	SI_EXIT,
	SI_ITEM_COUNT
};

class StdMenu : public DialogBase {
	protected:
		// Dialog widgets
		TextButton *w_items[SI_ITEM_COUNT];
		SDL_Surface *s_gfx;
		bool visible;

		// Helpers
		void ShowMenu(bool Visible);

		// Override events
		virtual bool InputOk(Widget *Object);
		virtual bool FocusEnter();
		virtual bool FocusLeave();
	public:
		StdMenu(EngineVN *Engine);
		~StdMenu();
};

#endif

