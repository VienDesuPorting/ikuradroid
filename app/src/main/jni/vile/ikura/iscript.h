/*! \class IkuraCore
 *  \brief Encapsulates data for Ikura Script Files (ISF)
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
#ifndef _ISCRIPT_H_
#define _ISCRIPT_H_

// Include base system
#include "iopcodes.h"
#include "../common/log.h"
#include "../common/dvector.h"
#include "../common/dbuffer.h"
#include "../common/savegame.h"
#include "../res/rwops.h"

#define GETBYTE(B)				((B)[0])
#define GETWORD(B)				((B)[0]|((B)[1]<<8))
#define GETDWORD(B)				((B)[0]|((B)[1]<<8)|((B)[2]<<16)|((B)[3]<<24))

// Forward declare engine interface
class IkuraDecoder;

// Script maintainers
struct IkuraScriptPosition {				//!< Struct for stacking positions
	Uint32 position;						//!< Stored jump position
	IkuraScriptPosition *nextptr;			//!< Next stacked position
};

// Declare main script interface
class IkuraScript {
	private:
		Uint32 ssoffset;					//!< Offset to script section
		Uint16 revision;					//!< Script revision code
		Uint8 xorkey;						//!< Key for xor
		Uint8 reserved;						//!< Unused
		Uint8 *buffer;						//!< Script data
		unsigned int length;				//!< Size of script buffer
		Uint32 *jumptable;              	//!< Table of script positions
		Uint32 *calltable;              	//!< Table of script positions
		unsigned int jtabcount;			//!< Valid entries in jumptable
		uString sname;						//!< Name of script resource
		IkuraScriptPosition *positions;		//!< Stacks positions
		uString save_name;					//!< Remembers script name
		Uint32 save_pos;					//!< Stores safe save position
        uString rsave_name;					//!< Remembers script name
		Uint32 rsave_pos;					//!< Stores safe save position
	public:
		IkuraScript();
		~IkuraScript();

		// Script API
		bool GetOpcode(Uint8 *Opcode,const Uint8 **Buffer,Uint32 *Length);
		bool Load(uString Name,Uint32 Pos,Uint8 *Buffer,Uint32 Length);
		bool Load(uString Name,Uint8 *Buffer,Uint32 Length);
		bool LoadISF(uString Name,Uint8 *Buffer,Uint32 Length);
		bool LoadDRS(uString Name,Uint8 *Buffer,Uint32 Length);
		bool Jump(Uint16 Table);
		bool Call(Uint16 Table);
		bool Return();

		// Diagnostics
		uString DebugPos(int *Position);

		// Savegame interface
		bool Load(IkuraDecoder *Engine,Savegame *File,uString Script);
		uString Save(Savegame *File);

		// Stacker
		IkuraScript *NextPtr;
};

#endif

