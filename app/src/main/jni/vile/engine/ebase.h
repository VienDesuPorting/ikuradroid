/*! \class EngineBase
 *  \brief Defines generic core API and events
 *  \todo Dirty rects system is disabled
 *  \todo Clipping is disabled
 *
 *  EngineBase defines the core event interface for the host, any kind of mouse
 *  movement, keyclicking or whatever is passed into the engine through
 *  EngineBase which redistributes it to the dynamic widgets while keeping
 *  them up to date on the screen. Many events are also decoded and
 *  redistributed as internal events such as EventGameProcess() which tells
 *  the derived objects that it is time to process game scripts.
 *
 *  EngineBase also defines logical layers (VN_LAYERS) for the games to place
 *  the loaded objects to make them appear logically ontop of each other
 *  visually on the screen.
 *
 *  The rendering interface was moved into Renderer, a dedicated class which
 *  is easy to subclass for experimantal rendering techniques.
 */
#ifndef _EBASE_H_
#define _EBASE_H_

// Include foundation classes
#include "../res/resources.h"
#include "../res/cache.h"
#include "renderer.h"
#include "crenderer.h"

// Include dialog elements
#include "../dialogs/selection.h"
#include "../dialogs/textview.h"
#include "../dialogs/popimage.h"
#include "../dialogs/stdtitle.h"
#include "../dialogs/stdsave.h"
#include "../dialogs/stdload.h"
#include "../dialogs/stdmenu.h"
#include "../dialogs/options.h"
#include "../dialogs/stdhalt.h"
#include "../dialogs/stdmsg.h"
#include "../dialogs/fatal.h"

// Configuration
#define ENGINE_PROCESSTICKS		20		//!< Milliseconds for processing
#define ENGINE_HIDEMOUSE		2		//!< Hide cursor when using keyboard

//!< Enumerates the graphical layers
enum VN_LAYERS {						//!< Enumerates viable layers
	VL_OVERLAY=0,						//!< Top layer
	VL_DIALOG,							//!< Load,save,options etc
	VL_EXTRAS,							//!< Overlay to ingame graphics
	VL_CHOICES,							//!< Command options
	VL_TEXTVIEW,						//!< Textview box or simular
	VL_HOTSPOTS,						//!< Game hotspots
	VL_CHARACTERS,						//!< Character graphics
	VL_BACKGROUND,						//!< Background graphics
	VL_SIZE								//!< Defines number of layers in enum
};

enum VN_DIALOGS {						//!< Enumerates standard dialogs
	VD_TITLE,							//!< Title screen
	VD_OPTIONS,							//!< Option / Configuration screen
	VD_SAVE,							//!< Dialog for saving games
	VD_LOAD,							//!< Dialog for loading games
	VD_LOG,								//!< Dialog for displaying text log
	VD_EXTRAS,							//!< Common CG room dialog
	VD_EXTRAS_CG,						//!< Specialized dialog for cg
	VD_EXTRAS_BGM,						//!< Specialized dialog for music
	VD_EXTRAS_SCENE,					//!< Specialized dialog for scenes
	VD_CREDITS,							//!< Credit roll
	VD_SHUTDOWN							//!< Shutdown verification
};

enum VN_TRANSITIONS {					//!< Enumerates screen transitions
	VT_NONE=0,							//!< No transition
	VT_FADE,							//!< Fade in new screen
	VT_RANDOM,							//!< Randomize a transition
	VT_SIZE								//!< Defines size of enum
};

class EngineBase {
	private:
		// Widget management
		Group *animations;				//!< Holds animation objects
		Group *widget_head;				//!< Holds widgets
		Widget *widget_focus;			//!< Pointer to focused widget

		// Engine data
		int hidemouse;					//!< Hides and shows mouse cursor
		bool fullscreen;				//!< True if fullscreen is enabled
		int width;						//!< Native game width
		int height;						//!< Native game height
		bool shutdown;					//!< Shutdown confirmation flag
		Renderer *renderer;
       // EDLTexture *rendererTex;
		// Next transition
		VN_TRANSITIONS transition_type;	//!< Next screen transition
		int mx;
		int my;
		SDL_Rect transition_rect;		//!< Limit transition to a rect
		int transition_time;			//!< Time to spend on transiton
        SDL_Window* window;
		// Helper
		void ScaledPaint(SDL_Surface *In,SDL_Surface *Out,SDL_Rect *Clip);
	public:
		// Contructorset
		EngineBase(int Width,int Height);
		virtual ~EngineBase();
        virtual void render();
		// Redistributed events
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventGameShutdown();
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventBackgroundMouseMove(int X,int Y);
		virtual bool EventBackgroundMouseLeftDown(int X,int Y);
		virtual bool EventBackgroundMouseRightDown(int X,int Y);
		virtual bool EventBackgroundMouseLeftUp(int X,int Y);
		virtual bool EventBackgroundMouseRightUp(int X,int Y);
		virtual bool EventBackgroundKeyDown(SDL_Keycode Key);
		virtual bool EventBackgroundKeyUp(SDL_Keycode Key);

		// Handlers for incoming system events
		virtual void EventHostTick();
		virtual void EventHostMouseMove(SDL_Surface *Screen,int X,int Y);
		virtual void EventHostMouseLeftDown(SDL_Surface *Screen,int X,int Y);
		virtual void EventHostMouseRightDown(SDL_Surface *Screen,int X,int Y);
		virtual void EventHostMouseLeftUp(SDL_Surface *Screen,int X,int Y);
		virtual void EventHostMouseRightUp(SDL_Surface *Screen,int X,int Y);
		virtual void EventHostKeyDown(SDL_Keycode Key);
		virtual void EventHostKeyUp(SDL_Keycode Key);

		// Engine attributes
		virtual const uString NativeID();
		virtual const uString NativeName();
		const int NativeWidth();
		const int NativeHeight();

		// Enforces a maximum framerate by waiting the remainder
		void LimitFramerate(int Framerate);
		void updateRenderWindow(SDL_Window* win);
		// GUI Management
		bool GetFullscreen();
		void SetFullscreen();
		void SetFullscreen(bool Fullscreen);
		void SetTransition(SDL_Rect *Rect=0);
		void SetTransition(VN_TRANSITIONS Transition,
				Uint32 Duration,SDL_Rect *Rect=0);
		void AddAnimation(Animation *WidgetPtr);
		bool DestroyAnimation();
		bool SkipAnimation();
		void AddWidget(Widget *WidgetPtr,int Layer,int Position=0);
		void DestroyWidget(Widget *WidgetPtr,int Layer);
		void DestroyWidget(Widget *WidgetPtr);
		void DestroyLayer(int Layer);
		void ShowLayer(int Layer,bool Visible);
		bool CheckWidget(Widget *WidgetPtr);
		Group *GetLayer(int Layer);
		Widget *GetWidget(int Layer,int Index);
		Widget *GetWidgetAt(int X,int Y);
		bool SetFocusWidget(Widget *WidgetPtr);
		bool SetFocusFirst();
		bool SetFocusNext();
		bool GetShutdown();

		// Access to internal graphics
		void Paint(SDL_Surface *Surface,VN_LAYERS Toplayer=VL_OVERLAY);

		// Recalculates coordinates for the screen display
		int GetRelativeX(SDL_Surface *Screen,int X);
		int GetRelativeY(SDL_Surface *Screen,int Y);

		// Warps mouse cursor
		void CursorWarp(int X,int Y);
};

#endif


