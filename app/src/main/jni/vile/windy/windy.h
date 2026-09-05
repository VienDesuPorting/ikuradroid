/*! \class EngineWindy
 *	\brief Windy game engine
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
 *  The Windy engine is not UNICODE compliant as it only supports two games.
 */
#ifndef _WINDY_H_
#define _WINDY_H_

#include "../engine/evn.h"
#include "whisper.h"

#define GETWORD(B)				((B)[0]|((B)[1]<<8))

enum WINDY_STATE {					//!< State of engine
	WINDYSTATE_NORMAL,				//!< Normal processing state
	WINDYSTATE_SELECT,				//!< Wait for user selection
	WINDYSTATE_WAITMORE,			//!< Wait for user then process more text
	WINDYSTATE_WAITCLICK,			//!< Wait for user then return to normal
	WINDYSTATE_WAITCLIENT			//!< Wait for game derivate to resume code
};

enum WINDY_STUBTYPE {
	WINDYSTUB_ERROR=		-1,		//!< Could not read stub
	WINDYSTUB_DEFAULT=		0x00,	//!< Null-terminated string
	WINDYSTUB_CLEARSCREEN=	0x01,	//!< Clear screen after text completes
	WINDYSTUB_WAITFORUSER=	0x02,	//!< Print next after text completes
	WINDYSTUB_SELECTION=	0x03,	//!< CR Terminated lines for selection
	WINDYSTUB_CONSTANT=		0x04	//!< Constant (for names etc)
};

class EngineWindy : public EngineVN {
	private:
		// Common subroutines
		Uint8 *ssbuffer;			//!< Holds current script data
		Uint32 ssindex;				//!< Current script code position
		Uint32 sslength;			//!< Length of current script data
		Uint32 ssoffset;			//!< Offset from given index
		DStack ssstack;				//!< Tracks code position through calls

		// Loaded script resource
		uString lsname;			//!< Name of loaded script
		Uint8 *lsbuffer;			//!< Holds loaded script data
		Uint32 lsindex;				//!< Code position for loaded script
		Uint32 lslength;			//!< Length of loaded script data
		Uint32 lsoffset;			//!< Offset from given index
		DStack lsstack;				//!< Stack mechanism for loaded script

		// Current script resource
		Uint8 *csbuffer;			//!< Holds current script data
		Uint32 *csindex;			//!< Current script code position
		Uint32 *cslength;			//!< Length of current script data
		Uint32 *csoffset;			//!< Offset from given index
		DStack *csstack;			//!< Tracks code position through calls

		// Selection data
		Stringlist sel_strings;		//!< Hold selection menu strings
		Uint16 sel_result;			//!< Holds register for selection result
		Uint16 sel_flag;			//!< Holds register for selection flag 
		Uint16 sel_address;			//!< Address to call(?) upon completion

		// Other Engine data
		WINDY_STUBTYPE tstype;		//!< Current text type (Cls,etc)
		Uint32 tsindex;				//!< Current text position (In script)
		WINDY_STATE state;			//!< Current state
		WINDY_STATE pausedstate;	//!< State to resume to
		Stringlist stringlist;		//!< Hold allocated strings
		Uint32 saveindex;			//!< Holds index for saving

		// Text processing
		WINDY_STUBTYPE ParseText(uString *String);
		WINDY_STUBTYPE ProcessText();
		bool ProcessOptions();
		bool processastext;
	protected:
		// Opcode handlers
		bool OP0002();				// Set background(?) graphics
		bool OP0003();
		bool OP000A();				// Unconditional jump
		bool OP000B();				// If(var==var)
		bool OP000C();				// If(var==int)
		bool OP0013();				// Switch/case
		bool OP0014();
		bool OP0015();				// Process text stub?
		bool OP0018();				// Stores intermediate value
		bool OP0019();				// Stores register value
		bool OP001A();				// Load named bgm resource
		bool OP001C();				// Stop running bgm
		bool OP001F();				// Load named script
		bool OP0021();
		bool OP0022();				// Changes state????
		bool OP0024();				// Add intermediate to variable
		bool OP0025();				// Substract intermediate from variable
		bool OP0027();				// Substract variable from variable
		bool OP002D();
		bool OP002F();				// Blitter?
		bool OP0038();				// And registers? 
		bool OP003A();
		bool OP003E();				// Divides registers
		bool OP003F();
		bool OP0040();				// Store system variables?
		bool OP0041();				// Store system variables?
		bool OP0042();
		bool OP0043();
		bool OP004A();
		bool OP004B();				// Load binary data
		bool OP004F();				// Play sound effect
		bool OP0051();				// AND register with intermediate
		bool OP0052();				// OR register with intermediate
		bool OP0053();
		bool OP0054();				// Call subroutines
		bool OP0055();				// Return from subroutines
		bool OP0059();				// Load string resources
		bool OP005A();				// Clear string resources
		bool OP005E();
		bool OP0060();				// Open external files (Legacy savegames)
		bool OP0062();				// Move background
		bool OP0063();				// Blitter?
		bool OP0064();				// Blitter?
		bool OP006E();				// Calculates string length
		bool OP007B();
		bool OP0080();				// Load values from inifile
		bool OP008F();				// Load subroutines
		bool OP0095();				// Blitter
		bool OP0096();				// Blitter?
		DVector vars;				// Holds values

		// Widgets
		Widget *w_bg;				//!< Holds background graphics
		Textview *w_tv;

		// Voice synchronization
		Whisper *whisper;
	public:
		EngineWindy(int Width,int Height);
		~EngineWindy();

		// Special windy events
		virtual void EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag);
		virtual void EventBlit(Uint16 Opcode,Uint16 Index);
		virtual bool EventExternal(Uint16 Address);
		virtual bool EventInternal(Uint16 Address);

		// Windy support methods
		virtual bool LoadWindyScript(uString Script,Uint16 Entrypoint=0);
		virtual bool LoadWindyRoutines(uString Script,Uint16 Entrypoint=0);
		virtual bool LoadGraphics(uString Name,Uint16 Index,Uint16 Type);
		virtual void FillGraphics(Uint16 Index);
		virtual void FillBackground(Uint32 RGBA);
		virtual Widget *LoadSelection(Stringlist Strings);
		virtual Widget *LoadSelection(Stringlist Strings,
				Uint16 R1,Uint16 R2,Uint16 A1,Uint16 A2);

		// Code manipulation
		bool Jump(Uint16 Address);
		bool Call(Uint16 Address);
		bool Return();

		// Variable access
		Uint16 GetValue(Uint16 Address);
		void SetValue(Uint16 Address,Uint16 Value);

		// Start and stop processing
		bool Pause();
		bool Resume();

		// Eventhandlers
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual void EventSelect(int Selection);
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
		virtual void EventNew();
		virtual void SetSkipmode(bool Skip);
		virtual void SetMessageDelayEnabled(bool Enabled);
		virtual void SetMessageDelayInterval(int Interval);
};

#endif

