/*! \class IkuraDisplay
 *  \brief Encapsulates internal graphics handling for ikura games
 */
#ifndef _IDISPLAY_H_
#define _IDISPLAY_H_

#include "../widgets/fade.h"
#include "../widgets/hotspot.h"
#include "../widgets/group.h"

#define GBSIZE			255				//!< Max index of graphical buffer
#define MBSIZE			32				//!< Size of mouse command buffer

class IkuraDisplay : public Hotspot {
	private:
		// Graphic buffers
		SDL_Surface *isurface[GBSIZE];	//!< Internally managed graphics

		// Hitmap and hotspot data
		SDL_Surface *hitmap;
		Group *spots;
		DArray keymap;
		int defkey;
		int selected;
		int result;
		int target;
	public:
		IkuraDisplay(int Width,int Height);
		IkuraDisplay(SDL_Rect Pos);
		~IkuraDisplay();

		// Graphics
		void SetSurface(int Index,SDL_Surface *Surface);
		void SetSurface(int Index,int Width,int Height);
		void SetTarget(int Index);
		void FillSurface(int SIndex,Uint8 R,Uint8 G,Uint8 B);
		SDL_Surface *GetSurface(int Index);
		void BlitSurface(int SIndex,SDL_Rect *SRect,
				int DIndex,SDL_Rect *DRect);
		void BlendSurface(int SIndex,SDL_Rect *SRect,
				int DIndex,SDL_Rect *DRect);

		// Hitmap and hotspot data
		void SetMap(SDL_Surface *Hitmap);
		void SetSpot(int Index,SDL_Rect Area);
		void SetSpot(int Index,int X,int Y,int W,int H);
		void DropSpot(int Index);
		void DropMap();
		void Clear();
		void SetKeyDefault(int Default);
		void SetKeyMap(int Index,int Key,int Next);
		int GetSelected(bool Unset);
		int GetResult(bool Unset);

		// Hook into user input
		virtual bool FocusEnter();
		virtual bool FocusLeave();
		virtual bool TestMouse(int X,int Y);
		virtual bool MouseMove(int X,int Y);
		virtual bool MouseLeftDown(int X,int Y);
		virtual bool KeyDown(SDL_Keycode Key);

		// Export savegame data
		bool Save(Savegame *File);
		bool Load(Savegame *File);
};

#endif

