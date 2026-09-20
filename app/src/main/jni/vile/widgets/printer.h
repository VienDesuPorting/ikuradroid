/*! \class Printer
 *  \brief Widget for printing text
 */
#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "widget.h"
using namespace std;

class Printer : public Widget {
	private:
		TTF_Font *font_ttf;
		SDL_Color font_color;
		SDL_Color back_color;
		unsigned char back_alpha;
		string font_name;
		int font_size;
		int font_style;
		int shadow_x;			//!< Shadow offset X (0 disables)
		int shadow_y;			//!< Shadow offset Y
		SDL_Color shadow_color;		//!< Shadow ink
		SDL_Surface *stext;		//!< Surface for current textview
		EDLTexture *stexturetext;
		string text;		//!< Text to print
		string curtext;	//!< Text that has been printed
		unsigned int index;		//!< Current text index
		Uint32 interval;		//!< Milliseconds per character
		Uint32 start;			//!< Start of sequence
		SDL_Rect crect;			//!< Client rect relative to widget
		SDL_Rect trect;			//!< Current print position
		int charcount;			//!< Number of printed characters
		bool wordwrap;			//!< Wether or not to wrap text lines
		bool clearscreen;		//!< Clear screen at next pass (wordwrap only)
		bool printnext();
	public:
		Printer(int X,int Y,int Width,int Height);
		Printer(SDL_Rect Position);
		Printer();
		~Printer();
		void Autosize(string Text);
		int GetLength();
		int GetRemaining();
		int GetInterval();
		void SetInterval(Uint32 Interval);
		void Reprint(int Interval=-1);
		void Print(string Text,int Interval=-1);
		void Newline();
		bool Skip();
		bool SkipScreen();
		bool Continue();
		void Clear();
		void ClearScreen();
		void SetWordwrap(bool Enable);
		void SetBackgroundColor(Uint32 Color);
		void SetFontFace(string Filename);
		void SetFontSize(int Size);
		void SetFontColor(Uint8 Red,Uint8 Green,Uint8 Blue);
		void SetFontColor(SDL_Color Color);
		void SetFontColor(Uint32 Color);
		void SetFontStyle(int Style);
		void SetFontShadow(int X,int Y,SDL_Color Color);
		void SetFontGlow(int Glow,SDL_Color Color);
		bool GetTextSize(string Text,int *Width,int *Height);

		void Render();
};

#endif

