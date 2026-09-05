/*! \class SaveSlab
 *  \brief Widget for displaying a load- or savegame
 */
#ifndef _SAVESLAB_H_
#define _SAVESLAB_H_

#include "widget.h"
#include "printer.h"
#include "../common/savegame.h"

class SaveSlab : public Widget {
	private:
		SDL_Surface *sbackground;	//!< Background surface
		SDL_Surface *soverlay;		//!< Overlay surface
		SDL_Surface *sthumb;		//!< Thumbnail surface
		SDL_Surface *sdate;			//!< Date surface
		SDL_Surface *stext;			//!< Text surface
		SDL_Rect rthumb;			//!< Thumbnail position
		SDL_Rect rdate;				//!< Date position
		SDL_Rect rtext;				//!< Text position
		bool selected;
		void update();
	public:
		SaveSlab(int X,int Y,int Width,int Height);
		SaveSlab(SDL_Rect Position);
		~SaveSlab();
		void SetSelected(bool Selected);
		void SetBackground(SDL_Surface *Src,SDL_Rect *SRect);
		void SetBackgroundAlpha(Uint8 Alpha);
		void SetOverlay(SDL_Surface *Src,SDL_Rect *SRect);
		void SetThumb(SDL_Surface *Src,SDL_Rect *SRect,SDL_Rect *DRect);
		void SetDate(Uint64 Unixtime,SDL_Rect *DRect);
		void SetText(uString Message,SDL_Rect *DRect);
		void Flush();
};

#endif

