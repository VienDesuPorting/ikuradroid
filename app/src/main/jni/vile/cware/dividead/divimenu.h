#ifndef _DIVIMENU_H_
#define _DIVIMENU_H_

#include "../../dialogs/selection.h"

enum DIVIMENU {
	DM_NEW=0x100,
	DM_LOAD,
	DM_SAVE,
	DM_CONFIG,
	DM_EXIT,
	DM_EXTRAS,
	DM_SCREENSHOT
};

class DiviDead;

class DiviMenu : public Selection {
	private:
		SDL_Surface *graphics;
	public:
		DiviMenu(DiviDead *Engine);
		~DiviMenu();
		void Generate();
		virtual bool InputOk(Widget *Object);
};

#endif

