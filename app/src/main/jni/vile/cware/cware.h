/*! \class EngineCWare
 *	\brief CWare game engine
 *
 *  CWare uses a simple word parser
 */
#ifndef _CWARE_H_
#define _CWARE_H_

#include "../engine/evn.h"
#include "cwaremap.h"

#define GETBYTE(B)				((B)[0])
#define GETWORD(B)				((B)[0]|((B)[1]<<8))
#define GETDWORD(B)				((B)[0]|((B)[1]<<8)|((B)[2]<<16)|((B)[3]<<24))

enum CWARESTATES {
	CWARESTATE_NORMAL,				//!< Process input script
	CWARESTATE_CHOICE,				//!< Wait for user to click a selection
	CWARESTATE_WAITCLICK,			//!< Wait for user to click
	CWARESTATE_PAUSED				//!< Show Extras/CG
};

class EngineCWare : public EngineVN {
	private:
		// Script data
		uString scriptname;		//!< Name of current script
		Uint8 *scriptbuffer;		//!< Script data
		unsigned int scriptlength;	//!< Length of script data
		unsigned int scriptindex;	//!< Index of binary script buffer
		unsigned int saveindex;		//!< Position for savegame
		DVector vars;				//!< Holds values
		CWARESTATES state;			//!< Current state

		// Opcode handlers
		bool OP0000();				//!< Print text
		bool OP0001();				//!< Add option
		bool OP0002();				//!< Jump to position
		bool OP0003();				//!< Copies a value to a bulk of variables
		bool OP0004();				//!< Copies a value to a variable
		bool OP0006();				//!< Flushes choices
		bool OP0007();				//!< Show options
		bool OP000A();				//!< Show again
		bool OP0010();				//!< Jump if NOT
		bool OP0011();				//!< Wait (milliseconds)
		bool OP0014();				//!< Repaint display
		bool OP0016();				//!< Load overlay image
		bool OP0018();				//!< Load script
		bool OP0019();				//!< Game end
		bool OP001E();				//!< Fade to black
		bool OP001F();				//!< Fade to white
		bool OP0026();				//!< Start music
		bool OP0028();				//!< Stop music
		bool OP002B();				//!< Play voice
		bool OP0030();				//!< Set clipping
		bool OP0035();				//!< Play sound
		bool OP0036();				//!< Stop sound
		bool OP0037();				//!< Map overlay background
		bool OP0038();				//!< Clear map overlay
		bool OP0040();				//!< Add map overlay
		bool OP0041();				//!< Show map overlay
		bool OP0046();				//!< Load foreground image
		bool OP0047();				//!< Load background image
		bool OP004A();				//!< Repaint clipped area
		bool OP004B();				//!< Load character image
		bool OP004C();				//!< Load two character images
		bool OP004D();
		bool OP004E();				//!< Scroll down
		bool OP004F();				//!< Scroll up
		bool OP0050();				//!< Set title for savegame
	protected:
		SDL_Rect background;		//!< Defines display rect
		Widget *display;			//!< Widget for showing base graphics
		Textview *textview;			//!< Textview display
		Selection *selection;		//!< Simple text choices
		CWareMap *map;				//!< Map object
	public:
		EngineCWare(int Width,int Height);
		~EngineCWare();

		// Game interface
		bool LoadCWareScript(uString Script);
		bool Jump(unsigned int Position);
		bool Running();
		void Stop();
		void Pause();
		void Resume();

		// Eventhandlers
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventLoad(int Index);
		virtual bool EventSave(int Index);
		virtual void EventSelect(int Selection);
		virtual void EventGameDialog(VN_DIALOGS Dialog);
};

#endif

