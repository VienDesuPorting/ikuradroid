/*! \class Textview
 *  \brief Widget for managing a textview with buttons
 *
 *  This should only be used as a basis for complex textview object which
 *  has embedded buttons etc. You should just use a Printer object if all
 *  you need is the animated text.
 *
 *  This class must be revised from bottom up.
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
#ifndef _TEXTVIEW_H_
#define _TEXTVIEW_H_

#include "dlgbase.h"

#define TEXLOG_LINES 100			//!< Number of lines to store in log

class Textview : public DialogBase {
	private:
		// Standard widgets
		Printer *w_text;
		ValueButton *w_skipbutton;
		BitmapButton *w_savebutton;
		BitmapButton *w_loadbutton;
		BitmapButton *w_optionbutton;
		BitmapButton *w_backbutton;
		BitmapButton *w_autobutton;

		// Textlogging
		bool textlog_flag;
		DStack textlog_stack;

		// Other data
		int xdisplacement;
		int ydisplacement;

		// OP42 SAY id word (indexes the original's name-color table)
		int nameid;
	public:
		Textview(EngineVN *Engine);
		~Textview();

		// Name color id carried from the script op to the textview
		void SetNameId(int Id){nameid=Id;}
		int GetNameId(){return nameid;}

		// API
		virtual void PrintText(uString Title,uString Text);
		virtual void PrintText(uString Text);
		virtual void PrintNewline();
		virtual void CompleteText();
		virtual void ClearText();
		void SetLeading(int Leading);
		bool GetTextSize(uString Text,int *Width,int *Height);
		bool GetTextPosition(int *X,int *Y,int *W,int *H);
		void SetTextPosition(int X,int Y,int W,int H);
		void SetTextInterval(Uint32 Milliseconds);
		void SetFontFace(uString Filename);
		void SetFontSize(int Size);
		void SetFontColor(Uint8 Red,Uint8 Green,Uint8 Blue);
		void SetFontColor(SDL_Color Color);
		void SetFontStyle(int Style);
		void SetFontShadow(int X,int Y,SDL_Color Color);
		void SetFontGlow(int Glow,SDL_Color Color);
		int GetRemainingText();

		// Log feature
		uString GetTextlog(int Index);
		void EnableTextlog(bool Enable);

		// Set skipmode
		void SetSkipmode(bool Skip);
		bool GetSkipmode();

		// Register standard buttons
		void SetSaveButton(BitmapButton *Object);
		void SetLoadButton(BitmapButton *Object);
		void SetSkipButton(ValueButton *Object);
		void SetAutoButton(BitmapButton *Object);
		void SetOptionButton(BitmapButton *Object);
		void SetBackButton(BitmapButton *Object);

		// Hook into user input
		virtual bool InputOk(Widget *Object);
};

#endif

