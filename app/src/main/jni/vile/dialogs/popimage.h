/* \class PopImage
 * \brief Popup image that destroys itself upon user input
 */
#ifndef _POPIMAGE_H_
#define _POPIMAGE_H_

#include "dlgbase.h"

class PopImage : public DialogBase{
	private:
		struct SLIDES_TYPE {
			SDL_Surface *surface;
			SDL_Rect rect;
			SLIDES_TYPE *next;
		}*slides;
		Hotspot *widget;
	public:
		PopImage(EngineVN *Engine,SDL_Surface *Surface);
		PopImage(EngineVN *Engine);
		~PopImage();
		void AddImage(SDL_Surface *Surface);
		void AddImage(SDL_Surface *Surface,int X,int Y);
		bool InputOk(Widget *Object);
};

#endif
