/*! \class EngineCrowd1
 *	\brief Crowd game engine v1
 */
#ifndef _CROWD1_H_
#define _CROWD1_H_

#include "../engine/evn.h"

enum CROWD1_STATE {
	C1_GUI,						//!< Wait for GUI
	C1_NORMAL,					//!< Normal processing state
	C1_SELECTION,				//!< Wait for user selection
	C1_WAITCLICK				//!< Wait for user input
};

class EngineCrowd1 : public EngineVN {
	private:
		// Script handling
		uString ParseWord(uString Line,char Parser,int Index);
		bool CleanText(char *input);
		CROWD1_STATE state;
		uString csname;
		const char *csbuffer;
		int cslength;
		int csindex;
		int saveindex;
		DArray *seldata;		//!< Tracks unread menu items
		int selcount;			//!< Number of selections
		int selindex;			//!< Buffer index for selection
		
		// Game data
		DVector rflags;			//!< Register lines
		DVector vars;			//!< Engine variables
		Cache *bgcache;			//!< Caches background
		Cache *fgcache;			//!< Caches background
	protected:
		// Override events
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventLoad(int Index);
		virtual bool EventSave(int Index);
		virtual void EventSelect(int Selection);

		// Widgets
		Widget *display_bg;
		Widget *display_event;
		Widget *display_cg[3];
		Textview *textview;
		Selection *selection;
		StdTitle *title;
	public:
		EngineCrowd1(int Width,int Height);
		~EngineCrowd1();

		// Engine API
		virtual bool LoadCrowdScript(uString Script);
		virtual bool SetBackground(uString Resource);
		virtual bool SetEventCG(uString Resource);
		virtual bool SetForeground(uString Resource,int Index,int X,int Y);
		virtual bool ShowSelection(Stringlist List);
};

#endif

