/*! \class Media
 *  \brief Provides media from the standard resource file
 *
 *  The internal resource archive (Default:vilevn.pck) contains standard
 *  graphic templates for standard widgets such as checkboxes, buttons and
 *  sliders.
 *
 *  Internal resources are globally available.
 */

#ifndef _MEDIA_H_
#define _MEDIA_H_

#include "resources.h"

class Media{
	private:
		static Resources *resman;
	public:
		// Resource and conversion interface
		static RWops *GetResource(uString Name);
		static SDL_Surface *GetImage(uString Name);
		static SDL_Surface **GetAnimation(uString Name);
		static void Close();
};

#endif


