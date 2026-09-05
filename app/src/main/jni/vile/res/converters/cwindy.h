/*! \class CWindy
 *  \brief Conversion routines for Windy Engine
 *
 *  Actually converts bitmaps for the s21 engine used by the Mayclub remake
 */
#ifndef _CWINDY_H_
#define _CWINDY_H_

#include "../rwops.h"
#include "../../common/edl_gfx.h"

class CWindy {
	public:
		static SDL_Surface *bet_32b(RWops *Object);
};

#endif
