#ifndef _NILOC_H_
#define _NILOC_H_

#include "../../dialogs/dlgbase.h"

class Nocturnal;

class NILocation : public DialogBase {
	private:
		BitmapButton *loadbutton(int Tag,SDL_Surface *Base,
				SDL_Surface *Icon,SDL_Rect *Source,SDL_Rect *Dest);
		virtual bool InputOk(Widget *Object);
	public:
		NILocation(Nocturnal *Engine);
		void LoadFirstFloor();
		void LoadSecondFloor();
		void LoadOutDoors();
};

#endif

