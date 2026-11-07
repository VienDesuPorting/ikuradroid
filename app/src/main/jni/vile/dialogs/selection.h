/*! \class Selection
 *  \brief Widget for displaying a load- or savegame
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
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
	bool bfill; //!< Backing strips behind text items
	bool swallow; //!< Ignore presses that miss every item
	bool hinvert; //!< Invert the hovered row instead of a fill (CP)
	bool fshadow; //!< Drop shadow under item captions
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
	void SetBackgroundFill(bool Enable);
	void SetSwallowMisses(bool Enable);
	void SetHoverInvert(bool Enable);
	void SetShadow(bool Enable);

		// Hook into user input
		bool MouseLeftDown(int X,int Y);
		bool MouseLeftDown(Widget *Object,int X,int Y);
		bool MouseLeftUp(Widget *Object,int X,int Y);
		bool InputOk(Widget* Object);
};

#endif

