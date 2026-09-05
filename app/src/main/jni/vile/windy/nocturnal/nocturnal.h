/*! \class Nocturnal
 *	\brief Uses the windy engine to stage a Nocturnal Illusion game
 */
#ifndef _NOCTURNAL_H_
#define _NOCTURNAL_H_

#include "../windy.h"
#include "nimain.h"
#include "nisave.h"
#include "niload.h"
#include "niloc.h"

class Nocturnal : public EngineWindy {
	private:
		int iselection;				//!< Currently used selection graphic
		bool menu_main_flag;		//!< True if current menu is a root menu
		int menu_main_cache;		//!< Holds last used main menu choice
		int menu_sub_cache;			//!< Holds last used main menu choice
		SDL_Surface *background;	//!< Cached background
		SDL_Surface *sselection[3];	//!< Cached selection backgrounds
		NIMain *main;				//!< Cached main menu
	public:
		Nocturnal(uString Path);
		~Nocturnal();

		// Windy overrides
		virtual bool EventExternal(Uint16 Address);
		virtual void EventBlit(Uint16 Opcode,Uint16 Index);
		virtual void EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag);

		// General overrides
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
		virtual void EventNew();
		virtual void EventSelect(int Selection);
		virtual const uString NativeID();
		virtual const uString NativeName();
		virtual bool PlayMusic(uString Name);
		virtual bool LoadGraphics(uString Name,Uint16 Index,Uint16 Type);
		virtual void FillGraphics(Uint16 Index);
		virtual Widget *LoadSelection(Stringlist Strings);
		virtual Widget *LoadSelection(Stringlist Strings,
				Uint16 R1,Uint16 R2,Uint16 A1,Uint16 A2);

		// Helpers
		uString GetImagename(uString Imagename);
};

#endif

