/*! \class Widget
 *	\brief A generic graphical widget (The key element in ViLE)
 *
 *  The Widget is the core element in ViLE. Any user interaction is picked up
 *  through widgets, and any graphics are presented to the user though widgets.
 *  Widgets are managed by the EngineBase class (Altough a Dialog element can
 *  work as a mediator for a group of widgets) by placing them in different
 *  layers on screen to build up a graphical interface.
 */
#ifndef _WIDGET_H_
#define _WIDGET_H_

#include "../common/log.h"
#include "../common/dstack.h"
#include "../common/dbuffer.h"
#include "../common/darray.h"
#include "../common/dvector.h"
#include "../common/stringlist.h"
#include "../common/inifile.h"
#include "../common/savegame.h"
#include "../common/cfg.h"
#include "../common/edl_fs.h"
#include "../common/edl_gfx.h"
#include "../common/edl_texture.h"
#include "../res/media.h"

// Define standard input keys
#define KEY_DIRECTION_H(K) 			(K==SDLK_LEFT||K==SDLK_RIGHT)
#define KEY_DIRECTION_V(K) 			(K==SDLK_UP||K==SDLK_DOWN)
#define KEY_DIRECTION(K)			(KEY_DIRECTION_H(K)||KEY_DIRECTION_V(K))
#define KEY_REFOCUS(K) 				(K==SDLK_TAB)
#define KEY_RETURN(K) 				(K==SDLK_RETURN||K==SDLK_KP_ENTER)
#define KEY_ACTION_OK(K) 			(KEY_RETURN(K)||K==SDLK_SPACE)
#define KEY_ACTION_CANCEL(K)		(K==SDLK_ESCAPE)
#define KEY_ACTION(K)				(KEY_ACTION_OK(K)||KEY_ACTION_CANCEL(K))


// Forward declare group class
class Group;

class Widget {
	private:
		// Helpers
		void setdefaults();
	protected:
		// data .. lotsa data
		static bool global_refresh;			//!< Wether we need to refresh
		bool filled;
		static SDL_Rect global_update;		//!< Tracks updated graphics
		bool local_refresh;					//!< Wether we need to refresh
		SDL_Rect local_update;				//!< Tracks updated graphics
		SDL_Rect pos;						//!< Screen position
		SDL_Surface *simage;				//!< Primary surface image
		SDL_Rect fill;
		SDL_Color color;
		bool fhittable;						//!< Accepts mouse input
		bool fvisible;						//!< Onscreen visibility
		Uint8 valpha;						//!< Alpha value
		Uint32 tag;							//!< Freeform data tag
		bool GlobalXY;
	public:
		// Constructors to force key settings
		Widget(int X,int Y,int Width,int Height);
		Widget(SDL_Rect Position);
		Widget();
		virtual ~Widget();

		// Surface options
		void SetAlpha(Uint8 Alpha,SDL_Rect *Destination=0);

		// Visibility options
		virtual void SetVisible(bool Show);
		virtual void SetHittable(bool Hittable);
		bool GetVisible();
		bool GetHittable();

		// Manipulate graphics directly
		virtual void Move(SDL_Rect Position);
		virtual void Move(int X,int Y);
		virtual void Resize(int Width,int Height);
		void Fill(Uint32 Color,SDL_Rect *DRect=0);
		void Fill(Uint8 R,Uint8 G,Uint8 B,Uint8 A,SDL_Rect *DRect=0);
		void Set(SDL_Surface *Src,SDL_Rect *SRect=0,SDL_Rect *DRect=0);
		void Blit(SDL_Surface *Src,SDL_Rect *SRect=0,SDL_Rect *DRect=0);
		void Blend(SDL_Surface *Src,SDL_Rect *SRect=0,SDL_Rect *DRect=0);
		void Dumpfile(uString Filename="");
		SDL_Surface *GetSurface();
		void Free();

		// Get positiondata
		int GetX();
		int GetY();
		int GetWidth();
		int GetHeight();
		SDL_Rect GetPosition();

		// Wether display needs refresh
		static bool GetGlobalRefresh(SDL_Rect *Cliprect=0);
		static bool PeekGlobalRefresh(SDL_Rect *Cliprect=0);
		virtual bool GetLocalRefresh(SDL_Rect *Cliprect=0);
		virtual bool PeekLocalRefresh(SDL_Rect *Cliprect=0);
		virtual void Refresh(SDL_Rect *Update=0);
		virtual void Redraw();
		void SetGlobalXY(bool glxy);

		// Data assignment
		void SetTag(Uint32 Tag);
		Uint32 GetTag();

		// Internal management and control interface

		virtual void Render();
		virtual bool TestKey(SDL_Keycode Key);
		virtual bool TestMouse(int X,int Y);
		virtual bool TestSolid();

		// Engine events
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

		// Stacking pointers for driver classes
		Widget *PrevPtr;
		Widget *NextPtr;
};

#endif

