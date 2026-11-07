/*! \class IParser
 *  \brief Parses a Ikura GDL script and handles execution, flags, etc
 */
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
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

		// Diagnostics
		void GetDebugPos(uString *Name,int *Position);

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

