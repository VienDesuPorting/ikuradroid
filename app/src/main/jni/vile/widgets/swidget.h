/*! \class StateWidget
 *  \brief Widget with state graphics
 */
#ifndef _SWIDGET_H_
#define _SWIDGET_H_

#include "widget.h"

// States
enum WIDGET_STATE {
	WS_NORMAL=0,
	WS_HOVER,
	WS_SELECT,
	WS_DISABLE,
	WS_INVALID,
	WIDGET_STATE_SIZE
};

class StateWidget : public Widget {
	protected:
		WIDGET_STATE state;
	public:
		StateWidget(SDL_Rect Position);
		StateWidget(int X,int Y,int Width,int Height);
		StateWidget();

		// Overrides
		virtual bool FocusEnter();
		virtual bool FocusLeave();
		virtual bool KeyDown(SDL_Keycode Key);
		virtual bool KeyUp(SDL_Keycode Key);
		virtual bool MouseLeftUp(int X,int Y);
		virtual bool MouseLeftDown(int X,int Y);

		// New events
		virtual void ChangeState(WIDGET_STATE State);
};

#endif

