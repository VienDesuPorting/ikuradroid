/*! \class EngineJAST
 *	\brief JAST game engine
 *
 *  The windy engine uses a wordcode interpreter to drive the game. Tbe kink
 *  with this system is that it allocates common routines in a different
 *  script. It also calls routines from both internal and external routines,
 *  making a stacking mechanism necesary for both handlers.
 *
 *  This implementation covers this by accessing the "current script resource"
 *  from a set of pointers; These pointers can point to either the resources
 *  for common routines (ssxxxxx) or the currently loaded resource (lsxxxxx).
 *
 *  The engine is currently rough on systemrelated commands, but works quite
 *  well using macros instead of the external methods (which holds most of the
 *  systemcalls).
 *
 *  The JAST engine is not UNICODE compliant as it only supports three games.
 */
#ifndef _JAST_H_
#define _JAST_H_

#include "../engine/evn.h"

#define GETWORD(B)				((B)[0]|((B)[1]<<8))

class EngineJAST : public EngineVN {
	private:
		// Script data
		uString scriptname;
		uString scriptlabel;
		Uint8 *scriptbuffer;
		int scriptlength;
		int scriptindex;

		// File resource database
		struct FILENAME_TYPE {
			uString type;		//!< Filetype (Ex. P,N,numeric)
			uString name;		//!< Filename (Ex. MB02_1,)
			FILENAME_TYPE *next;
		}*filenames;
		void AddFilename(uString Name,uString Type);
		void ClearFilenames();


		// Label database
		struct LABEL_TYPE {
			uString caption;	//!< Label (Typically M1, M2, etc)
			int index;				//!< Script index
			LABEL_TYPE *next;
		}*labels;
		void AddLabel(uString Caption,int Index);
		void ClearLabel();

		// Choice management
		struct CHOICE_TYPE {
			uString id;			//!< Textual ID (Typicall VVx and NNx)
			uString caption;	//!< Caption to display
			int index;				//!< Index (Main and sub separately)
			bool visible;			//!< Wether item should show
			CHOICE_TYPE *sub;		//!< Submenu (Null for none and subs)
			CHOICE_TYPE *next;		//!< Pointer for stacking
		}*choices;					//!< Head menu item
		void AddChoice(uString ID);
		void AddChoice(uString ID,uString SubID);
		void ClearChoice();

		// Widgets
		Widget *w_display;
		Textview *w_tv;
		Selection *w_sel;
		SDL_Rect r_display;

		SDL_Surface *s_background;	//!< Caches background image
		DVector vars;				//!< Holds values
		int choicemain,				//!< Last chosen mainmenu index
			choicesub;				//!< Last chosen summenu index

		// Parsers
		bool GetLine(uString Label);
		uString GetCurrentLine();
		uString GetNextLine();
		int GetParamCount(uString Line);
		uString GetParam(uString Line,int Index);

		// Santa's little helpers
		uString PopString(DStack *Stack);
		uString Unquote(uString Text);
		bool Interpret(DStack *Stack);

		// State control
		enum JAST_STATE {			//!< State of engine
			JASTSTATE_NORMAL,		//!< Normal processing state
			JASTSTATE_PRESELECT,	//!< Show last message and then selection
			JASTSTATE_SELECT,		//!< Wait for user selection
			JASTSTATE_WAITCLICK		//!< Wait for user then return to normal
		}state;
	public:
		EngineJAST(int Width,int Height);
		~EngineJAST();

		// JAST Support methods
		bool LoadJASTScript(uString Script);
		bool SetFrame(SDL_Surface *Resource);
		bool SetBackground(SDL_Surface *Resource);
		bool SetCharacter(SDL_Surface *Resource,int X);
		bool ShowSelection(int Index);
		bool ShowMain(int Index,bool Visible);
		bool ShowSub(int Index,int SubIndex,bool Visible);

		// Eventhandlers
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventBackgroundMouseRightUp(int X,int Y);
		virtual void EventSelect(int Selection);
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
};

#endif

