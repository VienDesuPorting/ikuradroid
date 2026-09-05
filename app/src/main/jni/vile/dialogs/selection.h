/*! \class Selection
 *  \brief Widget for displaying a load- or savegame
 */
#ifndef _SELECTION_H_
#define _SELECTION_H_

#include "dlgbase.h"

class Selection : public DialogBase {
	private:
		Uint32 bgscolor;
		Uint32 fgscolor;
		Uint32 bgucolor;
		Uint32 fgucolor;
		HALIGN halign;
		VALIGN valign;
		int fontsize;
	public:
		Selection(EngineVN *Engine);
		~Selection();

		// API
		void Clear();
		void SetFocusItem(int ID);
		int GetFocusItem();
		void SetText(uString Caption,SDL_Rect Area,int ID=-1);
		void SetText(Stringlist *Captions);
		void SetSurface(SDL_Surface *Selected,
				SDL_Surface *Unselected,
				SDL_Rect Area,int ID=-1);

		// Configuration options for textbased menues
		void SetFontSize(int Fontsize);
		void SetAlignment(HALIGN Horizontal,VALIGN Vertical);
		void SetColors(Uint32 BGSColor,Uint32 FGSColor,
				Uint32 BGUColor,Uint32 FGUColor);

		// Hook into user input
		bool InputOk(Widget* Object);
};

#endif

