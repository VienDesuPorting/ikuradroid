/*! \class TextButton
 *  \brief A input sensitive text control with optional background
 */
#ifndef _TBUTTON_H
#define _TBUTTON_H

#include "swidget.h"

#define TEXT_LEFT		0
#define TEXT_CENTER	1
#define	TEXT_RIGHT	2
#define TA

enum HALIGN {
	HA_LEFT=0,
	HA_CENTER,
	HA_RIGHT
};

enum VALIGN {
	VA_TOP=0,
	VA_CENTER,
	VA_BOTTOM
};

class TextButton : public StateWidget {
	private:
		Uint32 colorfg[WIDGET_STATE_SIZE];
		Uint32 colorbg[WIDGET_STATE_SIZE];
		HALIGN horizontal;
		VALIGN vertical;
		uString caption;
		int fontsize;
		void autogenerate();
	public:
		TextButton(int X,int Y,uString Caption);
		TextButton(int X,int Y,int Width,int Height,uString Caption);
		TextButton(SDL_Rect Dst,uString Caption);
		TextButton(SDL_Rect Dst);
		TextButton();

		// Configuration
		virtual void SetAlignment(HALIGN Horizontal,VALIGN Vertical);
		virtual void SetFontSize(int Size);

		// Color schema
		virtual Uint32 GetColorBackground(WIDGET_STATE State);
		virtual Uint32 GetColorForeground(WIDGET_STATE State);
		virtual void SetColorBackground(WIDGET_STATE State,Uint32 Color);
		virtual void SetColorForeground(WIDGET_STATE State,Uint32 Color);
		virtual void SetColorDefault();

		// Overrides
		virtual void ChangeState(WIDGET_STATE State);
		virtual void Resize(int Width,int Height);
};

#endif
