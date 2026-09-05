/*! \class Slide
 *  \brief Animation that slides across the screen
 */
#ifndef _SLIDE_H_
#define _SLIDE_H_

#include "animation.h"

class Slide : public Animation {
	private:
		// Common
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect rstart;
		SDL_Rect rdest;
	public:
		Slide();
		Slide(SDL_Rect Dst);
		Slide(SDL_Surface *Source,SDL_Rect Start,SDL_Rect Dest,int Duration);
		~Slide();
		void Animate(SDL_Rect Start,SDL_Rect Dest,Uint32 Duration);
        virtual void Render();
		virtual bool Skip();
		virtual bool Continue();
};

#endif
