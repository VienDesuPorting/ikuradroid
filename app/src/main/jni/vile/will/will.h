/*! \class EngineWill
 *	\brief Will game engine
 *
 *  This implementation was ported from VNMV (http://code.google.com/p/vnvm/)
 *  with the concent of its author, soywiz. Thank you!
 */
#ifndef _WILL_H_
#define _WILL_H_

#include "../engine/evn.h"

#define GETBYTE(B)				((B)[0])
#define GETWORD(B)				((B)[0]|((B)[1]<<8))
#define GETDWORD(B)				((B)[0]|((B)[1]<<8)|((B)[2]<<16)|((B)[3]<<24))

struct WILLTABLE {
	SDL_Surface *surface;			//!< Decoded graphics
	Uint32 flags[0xFF];				//!< Unknown binary blobs
	Uint8 keymap[0x12][0x10];		//!< Maps keyboard input
	Uint32 count;					//!< Have no idea
};

struct WILLANIMATION {
	uString name;				//!< Name of WIP graphic file
	SDL_Surface **frames;			//!< Animation frames
	bool visible[100];				//!< Visibility for each frame
	Uint16 entries[100][402];		//!< Unknown binary blobs
};

enum WILLSTATES {
	WILLSTATE_NORMAL,				//!< Process input script
	WILLSTATE_GUI,					//!< Will gui subsystem
	WILLSTATE_CHOICE,				//!< Wait for user to click a selection
	WILLSTATE_WAITCLICK				//!< Wait for user to click
};

class EngineWill : public EngineVN {
	private:
		// Script data
        bool clearOverlays;
        bool first;
		DVector vars;				//!< Holds values
		WILLSTATES state;			//!< Current state
		unsigned int ticks_stamp;	//!< Time of last tick update
		unsigned int ticks_value;	//!< Ticks left in timer

		// Will stuff
		Stringlist jumptable;		//!< Jumptable for selections
		static const int WILL_MAX_CHOICES=16;//!< Route table capacity
		Uint32 choiceroutes[WILL_MAX_CHOICES];//!< In-script route offset per choice
		int choicecount;			//!< Valid entries in choiceroutes
		SDL_Surface *prev_backgroudSurface;		//!<
		SDL_Surface *backgroudSurface;
		SDL_Surface *overlaySurfaces[3];
		SDL_Surface *prev_overlaySurfaces[3];

		SDL_Surface *prev_aniSurface;		//!<
		SDL_Surface *aniSurface;

		SDL_Surface *effect;		//!< Effectsurface

		WILLANIMATION *anim_data;	//!< Holds animation data
		WILLTABLE *table_data;		//!< Holds table data
		int table_mouseindex;		//!< Index for mouse hovering
		int table_varclick;			//!< Variable to register index in
		int table_varkind;			//!< Variable to register kind in
		uString bgname;			//!< Name of current bg resource

	protected:
		//! CP/LMM-era scripts use shorter argument blocks
		//! (OP4A=3B, OP21=2B, OP25=5B, OP8A=3B); set by the game subclass
		bool script_v2;

	private:

		// Opcode handlers
		bool OP01();				//!< Conditional jump
		bool OP02();				//!< Decision box
		bool OP03();				//!< Calculus
		bool OP04();				//!< Halt execution
		bool OP05();				//!< Wait for timer
		bool OP06();				//!< Absolute jump
		bool OP07();				//!< Load script
		bool OP08();				//!< Set text size
		bool OP09();				//!< Call script
		bool OP0A();				//!< Return from called script
		bool OP0B();				//!< Set timer
		bool OP0C();				//!< Decrement timer

		bool OP22();				//!< Stop music
		bool OP23();				//!< Play voice
		bool OP25();				//!< Play sound
		bool OP26();				//!< Stop sound
		bool OP41();				//!< Text ..
		bool OP42();				//!< Text ..
		bool OP43();				//!< Load table file
		bool OP45();				//!< Set table item
		bool OP4A();				//!< Set transition
		bool OP4B();				//!< Add animation
		bool OP4C();				//!< Play animation
		bool OP4D();				//!< Add effect
		bool OP4F();				//!< Unset table item
		bool OP50();				//!< Load table item
		bool OP51();				//!< Pick table item
		bool OP52();				//!< Delay sound
		bool OP54();				//!< Load transparent image
		bool OP61();				//!< Play video
		bool OPB6();				//!< Text ..
		bool OPFF();				//!< EOF
		bool OP46();				//!< Set background image
		bool OP47();				//!< Set background color
		bool OP48();				//!< Display character image
		bool OP49();				//!< Clear overlay
		bool OP64();				//!< Show character info
		bool OP68();				//!< Background position

		//!princess
		bool OP70();
		//!waltz

		bool OP73();				//!< Add overlay
		bool OP74();				//!< Clear layer
		bool OP82();				//!< Fixed delay
		bool OP83();				//!< New game load
		bool OP84();				//!< Name game save
		bool OP8B();				//!< Show config
		bool OPB8();				//!< Clear layer
		bool OPE2();				//!< Quickload
		bool OPXX(int Size);		//!< Skip unknown/irrelevant opcode
		bool OPXXName(int Size);//!< Skip opcode with name (LMM 0x0D family)
	protected:
		// Widgets
		Widget *display;			//!< Widget for showing base graphics
		Widget *overlay[3];			//!< Widget for showing characters
		Textview *textview;			//!< Textview display
		Selection *selection;		//!< Simple text choices

		// Choice presentation hooks
		//! Populates the text fallback menu (centered strips by default)
		virtual void LayoutTextSelection(Stringlist *items);
		//! Called whenever the choice UI is dismissed
		virtual void OnSelectClosed();
	public:
		EngineWill(int Width,int Height);
		~EngineWill();

		struct SCRIPT {
			uString name;		//!< Name of current script
			Uint8 *buffer;			//!< Script data
			unsigned int length;	//!< Length of script data
			unsigned int index;		//!< Index of binary script buffer
			unsigned int save;		//!< Position for savegame
			SCRIPT *nextptr;		//!< Pointer to stacked parent
		}*script;

		// Game interface
		WILLTABLE *LoadTable(uString Name);
		WILLANIMATION *LoadAnimation(uString Name);
		SDL_Surface *LoadMaskedImage(uString Name);
		SDL_Surface **LoadMaskedAnimation(uString Name);
		bool GetImagePosition(uString Name,Uint8 Index,SDL_Rect *Rect);
		bool LoadWillScript(uString Script);
		bool Jump(unsigned int Position);
		bool Running();
		void Stop();
		void SetTransit(Uint32 Duration);
        //
        virtual bool OP21();				//!< Play music
		// Eventhandlers
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventLoad(int Index);
		virtual bool EventSave(int Index);
		virtual void EventSelect(int Selection);
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventBackgroundMouseMove(int X,int Y);
		virtual bool EventBackgroundMouseLeftDown(int X,int Y);
		virtual bool EventBackgroundMouseRightDown(int X,int Y);

};

#endif

