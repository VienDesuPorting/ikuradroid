/*! \class Sprite
 *  \brief Animated sprite
 *
 *  The sprite can only animate at low framerate as it uses the bulky
 *  widget-tick event to drive itself. Consider an highspeed option on
 *  demand.
 */
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "widget.h"

class Sprite : public Widget {
	private:
		struct SPRITEFRAME {
			SDL_Surface *Surface;
			Uint32 Duration;
			SPRITEFRAME *NextPtr;
		}*spritestack,*spritecur;
		Uint32 start;
	public:
		Sprite(SDL_Rect Dst);
		Sprite();
		~Sprite();
		void AddFrame(SDL_Surface *Surface,SDL_Rect *Src,Uint32 Duration);
		void ClearFrames();
		virtual void Tick();
		//virtual void Copy(SDL_Surface *Dst);
		void Render();
};

#endif
