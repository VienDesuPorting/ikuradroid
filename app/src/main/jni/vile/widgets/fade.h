/*! \class Fade
 *  \brief Fade animation widget
 */
#ifndef _FADE_H_
#define _FADE_H_

#include "animation.h"
#include <math.h>

class Fade : public Animation {
	private:
		// Common
		SDL_Surface *sfade;			//!< Surface to fade or whatever into
		Uint32 start;				//!< Time the effect started
		Uint32 duration;			//!< How long the effect should run
		SDL_Rect crect;				//!< Rect relative to widget
		SDL_Color color;
	public:
		Fade(SDL_Rect Dst);
		Fade(SDL_Rect Dst,SDL_Surface *Surface,
				SDL_Rect Rect,Uint32 Duration);
		~Fade();
		void SetFade(SDL_Surface *Surface,SDL_Rect Rect,Uint32 Duration);
		void SetFade(Uint8 R,Uint8 G,Uint8 B,SDL_Rect Rect,Uint32 Duration);

		void Render();
		bool Skip();
		bool Continue();
};

class FadeBlack : public Fade {
	public:
		FadeBlack(SDL_Rect Dst,Uint32 Duration);

};

class FadeWhite : public Fade {
	public:
		FadeWhite(SDL_Rect Dst,Uint32 Duration);

};

class FadeColor : public Fade {
	public:
		FadeColor(SDL_Rect Dst,SDL_Color Color,Uint32 Duration);
		FadeColor(SDL_Rect Dst,Uint8 R,Uint8 G,Uint8 B,Uint32 Duration);
};

#endif

