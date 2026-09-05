/*! \class CCrowd
 *  \brief Conversion routines for Crowd Engine
 */
#ifndef _CCROWD_H_
#define _CCROWD_H_

#include "../rwops.h"
#include "../../common/edl_gfx.h"

class CCrowd {
	public:
		static SDL_Surface *cwp(RWops *Object);
		static SDL_Surface *zbm(RWops *Object);
};

#endif
