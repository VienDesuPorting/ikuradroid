/*! \class CIkura
 *  \brief Conversion routines for Ikura GDL
 */
#ifndef _CIKURA_H_
#define _CIKURA_H_

#include "../rwops.h"
#include "../../common/edl_gfx.h"

class CIkura {
	public:
		// Converters
		static SDL_Surface *ggp_32b(RWops *Object);
		static SDL_Surface *gga_32b(RWops *Object);
		static SDL_Surface *ggd_32b(RWops *Object);
		static SDL_Surface *ggd_24b(RWops *Object);
		static SDL_Surface *ggd_16b(RWops *Object);
		static SDL_Surface *ggd_8b(RWops *Object);
		static SDL_Surface **gan(RWops *Object);

		// Game-specific
		static Uint32 ggd_24b_alpha;
};

#endif

