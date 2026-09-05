/*! \class IParser
 *  \brief Parses a Ikura GDL script and handles execution, flags, etc
 */
#ifndef _IPARSER_H_
#define _IPARSER_H_

#include "iscript.h"

class IParser {
	private:
		// Script maintainers
		IkuraScript *scriptstack;
	public:
		IParser();
		~IParser();

		// Script API
		bool LoadScript(uString Name,Uint8 *Buffer,int Length,int Position);
		bool JumpScript(uString Name,Uint8 *Buffer,int Length);
		bool CallScript(uString Name,Uint8 *Buffer,int Length);
		bool ReturnScript();
		bool JumpFunction(Uint16 Table);
		bool CallFunction(Uint16 Table);
		bool ReturnFunction();
		bool GetOpcode(Uint8 *Opcode,const Uint8 **Buffer,Uint32 *Length);

		// Variable and flags API
		int DecodeValue(Uint32 Value);
		int GetValue(int Position);
		void SetValue(int Position,Uint32 Value);
		void SetSystem(int Position,bool Value);
		bool GetSystem(int Position);
		void SetFlag(int Position,bool Value);
		bool GetFlag(int Position);
		bool Hitpattern(Uint8 Index,Uint8 Command);
		void Setpattern(int Index,Uint8 *Buffer,int Size);

		// Variables and flags
		DVector System;					//!< System flags
		DVector Variables;				//!< Variables
		DVector Flags;					//!< Flags
		DVector GlobalFlags;			//!< Flags
		DBuffer Patterns;				//!< Hitpatterns

		// Export savegame data
		bool Save(Savegame *File);
		bool Load(IkuraDecoder* Engine,Savegame *File);
        bool SaveSystem(Savegame *File);
		bool LoadSystem(IkuraDecoder* Engine,Savegame *File);
};

#endif

