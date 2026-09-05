/*! \class Scroll
 *  \brief Scrolling widget
 *  \todo Implement scrolling in all four directions!
 *
 *  Scroll currently only works if you are scrolling downwards a vertical
 *  surface. This should be relatively easy to fix when needed.
 */
#ifndef _SCROLL_H_
#define _SCROLL_H_

#include "animation.h"

class Scroll : public Animation {
	private:
		// Common
		SDL_Surface *surface;		//!< Scrolling surface
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect rstart;			//!< Start source rectange
		SDL_Rect rdest;				//!< End source rectange
	public:
		Scroll();
		Scroll(SDL_Rect Dst);
		Scroll(SDL_Rect Dst,SDL_Surface *Source,SDL_Rect Start,
				SDL_Rect End,Uint32 Time);
		~Scroll();
		void Animate(SDL_Surface *Src,SDL_Rect Start,SDL_Rect End,
				Uint32 Time);
		virtual bool Skip();
		virtual bool Continue();
		virtual void Render();
};

#endif
