/*! \class Truelove
 *	\brief Truelove game engine
 */
#ifndef _TLOVE_H_
#define _TLOVE_H_

#include "../engine/evn.h"

enum TLSTATES {
	TLSTATE_NORMAL,							//!< Process input script
	TLSTATE_CHOICE,							//!< Wait for user selection
	TLSTATE_WAITCLICK,						//!< Wait for user to click
	TLSTATE_EXTRA							//!< Sow Extras/CG
};

enum TLBUFFERS {							//!< Defines surface buffers
	TLBUFFER0=0,							//!< Display buffer
	TLBUFFER1,								//!< Graphical cache
	TLBUFFER2,								//!< Graphical cache
	TLBUFFER3,								//!< Graphical cache
	TLBUFFER_SIZE							//!< Size of graphical cache
};

enum TLFLAGS {								//!< Defines flag banks
	TLFLAG_0=0,
	TLFLAG_1,
	TLFLAG_2,
	TLFLAG_3,
	TLFLAG_SIZE								//!< Number of flag banks
};

class Truelove : public EngineVN {
	private:
		// Stackable script data
		struct SCRIPTDATA {
			uString name;				//!< Name of script
			Uint8 *buffer;					//!< Script data
			Uint32 length;					//!< Length of data
			DVector table;					//!< Jumptable
			SCRIPTDATA *nextptr;			//!< Pointer next object
		}*scriptdata;						//!< Stack of loaded scripts

		// Script position stack
		struct SCRIPTPOS {
			Uint32 index;					//!< Current position in script
			SCRIPTDATA *data;				//!< Pointer to script data
			SCRIPTPOS *nextptr;				//!< Pointer to next position 
		}*scriptpos;						//!< Stacked positions

		// Engine data
		DVector vars[TLFLAG_SIZE];			//!< Holds values
		SDL_Surface *cache[TLBUFFER_SIZE];	//!< Holds graphics
		TLSTATES state;						//!< Current state
		int mousex;
		int mousey;

		// Opcode handlers
		bool OP00(Uint8 *Data,Uint32 Size);
		bool OP04(Uint8 *Data,Uint32 Size);
		bool OP16(Uint8 *Data,Uint32 Size);
		bool OP17(Uint8 *Data,Uint32 Size);
		bool OP19(Uint8 *Data,Uint32 Size);
		bool OP1B(Uint8 *Data,Uint32 Size);
		bool OP20(Uint8 *Data,Uint32 Size);
		bool OP23(Uint8 *Data,Uint32 Size);
		bool OP24(Uint8 *Data,Uint32 Size);
		bool OP28(Uint8 *Data,Uint32 Size);
		bool OP2B(Uint8 *Data,Uint32 Size);
		bool OP30(Uint8 *Data,Uint32 Size);
		bool OP31(Uint8 *Data,Uint32 Size);
		bool OP32(Uint8 *Data,Uint32 Size);
		bool OP33(Uint8 *Data,Uint32 Size);
		bool OP34(Uint8 *Data,Uint32 Size);
		bool OP35(Uint8 *Data,Uint32 Size);
		bool OP36(Uint8 *Data,Uint32 Size);
		bool OP38(Uint8 *Data,Uint32 Size);
		bool OP39(Uint8 *Data,Uint32 Size);
		bool OP3A(Uint8 *Data,Uint32 Size);
		bool OP3C(Uint8 *Data,Uint32 Size);
		bool OP40(Uint8 *Data,Uint32 Size);
		bool OP41(Uint8 *Data,Uint32 Size);
		bool OP42(Uint8 *Data,Uint32 Size);
		bool OP43(Uint8 *Data,Uint32 Size);
		bool OP44(Uint8 *Data,Uint32 Size);
		bool OP48(Uint8 *Data,Uint32 Size);
		bool OP49(Uint8 *Data,Uint32 Size);
		bool OP4A(Uint8 *Data,Uint32 Size);
		bool OP4B(Uint8 *Data,Uint32 Size);
		bool OP4C(Uint8 *Data,Uint32 Size);
		bool OP52(Uint8 *Data,Uint32 Size);
		bool OP53(Uint8 *Data,Uint32 Size);
		bool OP54(Uint8 *Data,Uint32 Size);
		bool OP61(Uint8 *Data,Uint32 Size);
		bool OP62(Uint8 *Data,Uint32 Size);
		bool OP63(Uint8 *Data,Uint32 Size);
		bool OP66(Uint8 *Data,Uint32 Size);
		bool OP67(Uint8 *Data,Uint32 Size);
		bool OP70(Uint8 *Data,Uint32 Size);
		bool OP71(Uint8 *Data,Uint32 Size);
		bool OP72(Uint8 *Data,Uint32 Size);
		bool OP73(Uint8 *Data,Uint32 Size);
		bool OP75(Uint8 *Data,Uint32 Size);
		bool OP82(Uint8 *Data,Uint32 Size);
		bool OP83(Uint8 *Data,Uint32 Size);
		bool OP84(Uint8 *Data,Uint32 Size);
		bool OP85(Uint8 *Data,Uint32 Size);
		bool OP86(Uint8 *Data,Uint32 Size);
		bool OP87(Uint8 *Data,Uint32 Size);
		bool OP89(Uint8 *Data,Uint32 Size);
		bool OP8A(Uint8 *Data,Uint32 Size);
		bool OP91(Uint8 *Data,Uint32 Size);
		bool OP92(Uint8 *Data,Uint32 Size);
		bool OP95(Uint8 *Data,Uint32 Size);
		bool OP98(Uint8 *Data,Uint32 Size);
		bool OP99(Uint8 *Data,Uint32 Size);
		bool OP9D(Uint8 *Data,Uint32 Size);
		bool OPA6(Uint8 *Data,Uint32 Size);
		bool OPA7(Uint8 *Data,Uint32 Size);
		bool OPAA(Uint8 *Data,Uint32 Size);
		bool OPAD(Uint8 *Data,Uint32 Size);
		bool OPAE(Uint8 *Data,Uint32 Size);
		bool OPFF(Uint8 *Data,Uint32 Size);
	protected:
		Widget *display;			//!< Widget for showing base graphics
		Textview *textview;			//!< Textview display
		Selection *selection;		//!< Simple text choices
	public:
		Truelove(uString Path);
		~Truelove();

		// Game interface
		bool LoadTrueloveScript(uString Script);
		bool Jump(unsigned int Position);
		bool Call(unsigned int Position);
		bool Return();
		bool Running();
		void Purge();
		void Stop();
		Sint32 Decode(Uint32 Value);

		// Eventhandlers
		virtual void EventHostMouseMove(SDL_Surface *Screen,int X,int Y);
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual bool EventLoad(int Index);
		virtual bool EventSave(int Index);
		virtual void EventSelect(int Selection);
		virtual const uString NativeID();
		virtual const uString NativeName();

};

#endif

