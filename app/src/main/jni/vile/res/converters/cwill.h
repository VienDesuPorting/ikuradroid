/*! \class CWill
 *  \brief Conversion routines for Windy Engine
 *
 *  Actually converts bitmaps for the s21 engine used by the Mayclub remake
 */
#ifndef _CWILL_H_
#define _CWILL_H_

#include "../rwops.h"
#include "../../common/edl_gfx.h"

class CWill {
	public:
		static SDL_Surface *wip(RWops *Object,int Index);
		static SDL_Surface *wip(RWops *Object);
		static SDL_Surface **wipa(RWops *Object);
};

#endif
