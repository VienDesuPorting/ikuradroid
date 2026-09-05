/*! \class IkuraCore
 *  \brief Encapsulates data for Ikura Script Files (ISF)
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

		// Savegame interface
		bool Load(IkuraDecoder *Engine,Savegame *File,uString Script);
		uString Save(Savegame *File);

		// Stacker
		IkuraScript *NextPtr;
};

#endif

