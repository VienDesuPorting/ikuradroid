/*! \class Animation
 *  \brief Defines an animated widget
 */
#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "widget.h"

class Animation : public Widget {
	public:
		Animation(int X,int Y,int Width,int Height);
		Animation(SDL_Rect Dst);
		Animation();

		// Interact with dynamic objects
		virtual bool Continue();
		virtual bool Skip();
};

#endif
