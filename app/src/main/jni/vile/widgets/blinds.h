/*! \class Blinds
 *  \brief Venetian blinds wipe animation
 */
#ifndef _BLINDS_H_
#define _BLINDS_H_

#include "animation.h"

class Blinds : public Animation {
	private:
		// Common
		SDL_Surface *simage;			//!< Final graphics
		SDL_Surface *sframe;			//!< Frame being assembled
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect crect;				//!< Rect relative to widget
		SDL_Color color;			//!< Slats color
		bool vertical;				//!< Vertical slats
		int pitch;				//!< Distance between slats
	public:
		Blinds(SDL_Rect Dst,SDL_Surface *Surface,
				SDL_Rect Rect,Uint32 Duration,
				bool Vertical,int Pitch,
				Uint8 R,Uint8 G,Uint8 B);
		~Blinds();

		void Render();
		bool Skip();
		bool Continue();
};

#endif

