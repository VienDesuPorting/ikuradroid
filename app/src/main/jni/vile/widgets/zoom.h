/*! \class Zoom
 *  \brief Zoom animation widget
 */
#ifndef _ZOOM_H_
#define _ZOOM_H_

#include "animation.h"
#include <math.h>

class Zoom : public Animation {
	private:
		// Common
		SDL_Surface *source;			//!< Surface to zoom or whatever into
		SDL_Rect start;
		SDL_Rect end;
		Uint32 t_start;				//!< Time the effect started
		Uint32 t_duration;			//!< How long the effect should run
	public:
		Zoom(SDL_Rect Dst);
		Zoom(SDL_Surface *Surface,SDL_Rect Source,SDL_Rect Start,
				SDL_Rect End,Uint32 Duration);
		~Zoom();
		void SetZoom(SDL_Surface *Surface,SDL_Rect Source,
				SDL_Rect Start,SDL_Rect End,Uint32 Duration);
		bool ZoomSurface(double Progress,SDL_Surface **Surface,SDL_Rect *Rect);
		virtual void Render();
		bool Skip();
		bool Continue();
};

#endif

