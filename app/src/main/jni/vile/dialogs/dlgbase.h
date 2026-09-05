/*! \class DialogBase
 *  \brief Organizes and handles widgets in a dialog sense
 *
 *  The Dialog class distributes events between the derived dialog and the
 *  widgets within it. A MouseLeftDown event from the engine will be handled
 *      if(!(retval=MouseLeftDown(wptr,X,Y))){
 *          if((retval=wptr->MouseLeftDown(X,Y))){
 *              retval=InputOk(wptr);
 *          }
 *      }
 *
 *  The first and last calls are sent to the derived dialog which might
 *  intercept and optionally block further processing of the event by
 *  returning true.
 *
 *  Normally you will want to override the InputOk() method.
 */
#ifndef _DLGBASE_H_
#define _DLGBASE_H_

#include "../widgets/group.h"
#include "../widgets/hotspot.h"
#include "../widgets/sprite.h"
#include "../widgets/printer.h"
#include "../widgets/tbutton.h"
#include "../widgets/bbutton.h"
#include "../widgets/vbutton.h"
#include "../widgets/checkbox.h"
#include "../widgets/slider.h"
#include "../widgets/saveslab.h"
#include "../widgets/fade.h"
#include "../widgets/zoom.h"
#include "../widgets/slide.h"
#include "../widgets/scroll.h"

// Forward declaration of the engine classes
class EngineVN;

class DialogBase : public Widget {
	private:
		bool block;
		Widget *focus;
		Group *layer;
		int defindex;
	protected:
		// Protected constructors
		DialogBase(EngineVN *Engine,bool Blockinput);
		~DialogBase();

		// Configure widgets
		Widget *AddWidget(Widget *WidgetPtr);
		void DestroyWidget(Widget *WidgetPtr);
		void DestroyWidgets();

		// Access to parent engine
		EngineVN *engine;
	public:
		// Widget overrides
		virtual void Render();
		virtual bool TestKey(SDL_Keycode Key);
		virtual bool TestMouse(int X,int Y);
		virtual bool GetLocalRefresh(SDL_Rect *Cliprect);
		virtual bool PeekLocalRefresh(SDL_Rect *Cliprect);

		bool SetFocus(Widget *WidgetPtr);
		Widget *GetFocus();

		// Handle events
		virtual void Tick();
		virtual bool FocusEnter();
		virtual bool FocusLeave();
		virtual bool MouseMove(int X,int Y);
		virtual bool MouseLeftUp(int X,int Y);
		virtual bool MouseRightUp(int X,int Y);
		virtual bool MouseLeftDown(int X,int Y);
		virtual bool MouseRightDown(int X,int Y);
		virtual bool KeyUp(SDL_Keycode Key);
		virtual bool KeyDown(SDL_Keycode Key);

		// Declare dialog events
		virtual bool MouseMove(Widget *Object,int X,int Y);
		virtual bool MouseLeftUp(Widget *Object,int X,int Y);
		virtual bool MouseRightUp(Widget *Object,int X,int Y);
		virtual bool MouseLeftDown(Widget *Object,int X,int Y);
		virtual bool MouseRightDown(Widget *Object,int X,int Y);
		virtual bool KeyUp(Widget *Object,SDL_Keycode Key);
		virtual bool KeyDown(Widget *Object,SDL_Keycode Key);
		virtual bool FocusEnter(Widget *Object);
		virtual bool FocusLeave(Widget *Object);
		virtual bool InputOk(Widget *Object);
		virtual bool InputCancel(Widget *Object);
		virtual bool InputDirection(SDL_Keycode Key);

		// Manage focus
		virtual Widget *Select();

		// Retrieve widgets in various ways
		void MoveDialog(int X,int Y);
		bool CheckWidget(Widget *WidgetPtr);
		int GetWidgetCount();
		Widget *GetWidget(int X,int Y);
		Widget *GetWidget(int Index);
};

#endif

