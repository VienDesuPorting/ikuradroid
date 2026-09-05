/*! \class Cache
 *  \brief Basic class for caching processed graphics
 *
 *  This is a highly experimental realtime cacher designed to optimize ViLE
 *  per-engine. We have tested a couple of conventional oft-used-resource
 *  caches directly in the Resource manager, but these has failed to produce
 *  significant effects as so many of the processing steps are performed
 *  after decoding the base graphic format (Color keying, masking, etc).
 *
 *  The Cache class cannot be used transparantly so it is not recomended to
 *  implement it until an engine is matured and/or fully understood. Check
 *  the Crowd engine for sample implementation.
 */
#ifndef _CACHE_H_
#define _CACHE_H_

#include "../common/edl_gfx.h"
#include "rwops.h"

// Cache item
struct CacheItem {
	SDL_Surface *Surface;
	uString Name;
	CacheItem *NextPtr;
};

class Cache {
	private:
		CacheItem root;
		int maxsize;
		int size;
	public:
		Cache(int Size);
		~Cache();
		void SetSurface(uString Name,SDL_Surface *Surface);
		SDL_Surface *GetSurface(uString Name);
};

#endif

