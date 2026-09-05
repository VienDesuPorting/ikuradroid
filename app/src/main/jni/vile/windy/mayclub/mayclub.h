/*! \class Mayclub
 *	\brief Uses the crowd engine to stage a XChange 3 game
 */
#ifndef _MAYCLUB_H_
#define _MAYCLUB_H_

#include "../windy.h"
#include "mcwhisper.h"
#include "mcload.h"
#include "mcsave.h"
#include "mcmain.h"
#include "mcalbum.h"
#include "mctv.h"
#include "mcback.h"

class Mayclub : public EngineWindy {
	private:
		// Graphic buffers
		SDL_Surface *sbuffer0;
		SDL_Surface *sbuffer1;
		SDL_Surface *sbuffer2;

		// Cached selection graphics
		SDL_Surface *sselection[4];

		// Cached main menu
		MCMain *main;

		// Helper functions
		uString GetImagename(uString Imagename);
	public:
		Mayclub(uString Path);
		~Mayclub();

		// Special windy events
		virtual void EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag);
		virtual bool EventExternal(Uint16 Address);

		// Windy methods
		virtual bool LoadGraphics(uString Name,Uint16 Index,Uint16 Type);
		virtual void FillGraphics(Uint16 Index);
		virtual Widget *LoadSelection(Stringlist Strings);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
		virtual bool PlayMusic(uString Name);
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
		virtual void EventNew();
		virtual bool EventBackgroundMouseRightUp(int X,int Y);
};

#endif

