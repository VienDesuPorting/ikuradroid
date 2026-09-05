/*! \class Textview
 *  \brief Widget for managing a textview with buttons
 *
 *  This should only be used as a basis for complex textview object which
 *  has embedded buttons etc. You should just use a Printer object if all
 *  you need is the animated text.
 *
 *  This class must be revised from bottom up.
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
	public:
		Textview(EngineVN *Engine);
		~Textview();

		// API
		virtual void PrintText(uString Title,uString Text);
		virtual void PrintText(uString Text);
		virtual void PrintNewline();
		virtual void CompleteText();
		virtual void ClearText();
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

