/*! \class CachedRenderer
 *  \brief Renders widgets onto a surface using an internal cache
 *
 *  The idea of the CachedRenderer is that most games layers a background,
 *  some character graphics and a textview widget, whereas most of the time,
 *  the only screen update is a character changing on the textview widget so
 *  you do not really have to keep repainting the background or alpha-blending
 *  the character images.
 *
 *  The CacheRenderer remembers which widgets that requires updates, and 
 *  caches a copy of the static widgets instead of drawing them. The effiency 
 *  of this method depends on the engine implementation, complex systems
 *  like Ikura usually caches its own graphics on a single surface and will 
 *  not benefit from this at all, but the overhead should be minimal so it
 *  might be a fair comprimise.
 */

#ifndef _CRENDERER_H_
#define _CRENDERER_H_

#include "renderer.h"

class CachedRenderer : public Renderer {
	private:
		SDL_Surface *clean;
		Uint32 cleanhash;
	public:
		CachedRenderer(Group *Widgets,Group *Animations,int Width,int Height);
		~CachedRenderer();

		// Optimized functions
		virtual void UpdateWidgets(SDL_Surface *Surface);
};

#endif

