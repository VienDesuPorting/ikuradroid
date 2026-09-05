/*! \class CCWare
 *  \brief Conversion routines for Windy Engine
 *
 *  Actually converts bitmaps for the s21 engine used by the Mayclub remake
 */
#ifndef _CCWARE_H_
#define _CCWARE_H_

#include "../rwops.h"
#include "../../common/edl_gfx.h"

class CCWare {
	public:
		static SDL_Surface *lz_32b(RWops *Object);
};

#endif
