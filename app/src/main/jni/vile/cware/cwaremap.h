/*! \class CWareMap
 *  \brief Manages graphical map selections for CWare games
 */

#ifndef _CWAREMAP_H_
#define _CWAREMAP_H_

#include "../dialogs/dlgbase.h"

class EngineCWare;

class CWareMap : public DialogBase {
	private:	
		uString bgstring;						//!< Stores name of background
		uString fgstring;						//!< Stores name of foreground
		uString istring;						//!< Stores items
		SDL_Surface *bg;						//!< Background surface
		SDL_Surface *fg;						//!< Foreground surface
		virtual bool InputOk(Widget *Object);
	public:
		CWareMap(EngineCWare *Engine);
		~CWareMap();

		// Map API
		void Flush();
		bool SetBackground(uString Name);
		bool SetForeground(uString Name);
		bool AddLocation(int X1,int Y1,int X2,int Y2,unsigned int Position);

		// Support for loading and saving
		bool LoadMap(uString MapSettings);
		uString SaveMap();
};

#endif

