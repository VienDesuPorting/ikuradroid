/*! \class Hotspot
 *  \brief Defines an on-screen field that takes input
 */
#ifndef _HOTSPOT_H_
#define _HOTSPOT_H_

#include "widget.h"

class Hotspot : public Widget {
	public:
		Hotspot(int X,int Y,int Width,int Height);
		Hotspot(SDL_Rect Dst);
		Hotspot();
};

#endif
