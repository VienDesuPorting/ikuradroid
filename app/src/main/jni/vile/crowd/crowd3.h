/*! \class EngineCrowd3
 *	\brief Crowd game engine v3
 */
#ifndef _CROWD3_H_
#define _CROWD3_H_

#include "../engine/evn.h"

enum CROWD3_STATE {
	C3_NORMAL,					//!< Normal processing state
	C3_SELECTION,				//!< Wait for user selection
	C3_WAITCLICK				//!< Wait for user input
};

class EngineCrowd3 : public EngineVN {
	private:
		// Script handling
		uString ParseWord(char Parser);
		void SkipWord(char Parser);
		bool CleanText(char *input);
		CROWD3_STATE state;
		uString csname;
		const char *csbuffer;
		int cslength;
		int csindex;
		int saveindex;
		int selcount;
		
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
		EngineCrowd3(int Width,int Height);
		~EngineCrowd3();

		// Engine API
		virtual bool LoadCrowdScript(uString Script);
		virtual bool SetBackground(uString Resource);
		virtual bool SetEventCG(uString Resource);
		virtual bool SetForeground(uString Resource,int Index=0);
		virtual bool ShowSelection(Stringlist List);
};

#endif

