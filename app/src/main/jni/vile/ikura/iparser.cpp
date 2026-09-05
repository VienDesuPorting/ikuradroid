/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "iparser.h"

IParser::IParser() {
	// Preset values
	scriptstack=0;
}

IParser::~IParser(){
	while(scriptstack){
		IkuraScript *tmpscript=scriptstack->NextPtr;
		delete scriptstack;
		scriptstack=tmpscript;
	}
}

/*! \brief Saves parser state and all script positions
 *  \param File Savegame file to write to
 *  \return True if the data was successfully stored
 */
bool IParser::Save(Savegame *File){
	// Get save positions
	Uint32 count=0;
	IkuraScript *tmpptr=scriptstack;
	while(tmpptr){
		uString name=tmpptr->Save(File);
		File->SaveString(EDL_Format("script-%d",count++),name);
		tmpptr=tmpptr->NextPtr;
	}

	// Save metadata
	File->SaveUint32("stack-size",count);
	File->SaveVector("flags",&Flags);
	File->SaveVector("vars",&Variables);
	return true;
}

bool IParser::Load(IkuraDecoder *Engine,Savegame *File){
	Uint32 size;
	bool retval=false;
	if(File->LoadUint32("stack-size",&size)){
		// Flush existing stack
		while(scriptstack){
			IkuraScript *script=scriptstack->NextPtr;
			delete scriptstack;
			scriptstack=script;
		}

		// Rebuild old stack
		uString name;
		IkuraScript *stackptr=0;
		for(unsigned int i=0;i<size;i++){
			if(File->LoadString(EDL_Format("script-%d",i),&name)){
				IkuraScript *script=new IkuraScript();
				if(script && script->Load(Engine,File,name)){
					if(stackptr){
						stackptr->NextPtr=script;
						stackptr=stackptr->NextPtr;
					}
					else{
						stackptr=script;
						scriptstack=script;
					}
				}
				else if(script){
					delete script;
				}
			}
		}

		// Load metadata
		retval=true;
		retval&=File->LoadVector("flags",&Flags);
		retval&=File->LoadVector("vars",&Variables);
	}
	return retval;
}

bool IParser::SaveSystem(Savegame *File){
	File->SaveVector("GlobalFlags",&GlobalFlags);
	//File->SaveVector("vars",&Variables);
	return true;
}

bool IParser::LoadSystem(IkuraDecoder *Engine,Savegame *File){
    File->LoadVector("GlobalFlags",&GlobalFlags);
    //File->LoadVector("vars",&Variables);
	return true;
}

/*! \brief Returns register value if 32nd bit is set
 *  \param 31 bit signed value or register address
 *  \returns 32bit signed value
 */
int IParser::DecodeValue(Uint32 Value){
	// Check 32b signed (Means register value)
	if(Value&0x80000000){
		Value=GetValue(Value&~(0x80000000));
	}
	if(Value&0x40000000){
		Value|=0x80000000;
	}
	return Value;
}

/*! \brief Retrieves a 31 bit signed as a 32bit value
 *  \param Position Value index
 *  \returns 32bit signed value
 */
int IParser::GetValue(int Position){
	// Check 32b signed (Means register value)
	Uint32 value=Variables.GetUint32(Position);
	if(value&0x40000000){
		value|=0x80000000;
	}
	return value;
}

/*! \brief Stores a "Uint31" value :)
 *  \param Position Value index
 *  \param Value The 31 bit signed value
 */
void IParser::SetValue(int Position,Uint32 Value){
	Variables.SetUint32(Position,Value&~(0x80000000));
}

/*! \brief Sets a system flag
 *  \param Position Which flag to set
 *  \param Value Value of target flag
 *
 *  Please note that several system flags correspond to "auto" settings
 *  etc and will be overridden in the decoder stage with dynamic values.
 */
void IParser::SetSystem(int Position,bool Value){
    LogError("SetSystem: %d",Position);
	System.SetBit(Position,Value);
}

/*! \brief Reads a system flag
 *  \param Position Which flag to read
 *  \returns Value of read flag
 */
bool IParser::GetSystem(int Position){
//LogError("GetSystem: %d",Position);
	return System.GetBit(Position);
}

/*! \brief Sets a flag
 *  \param Position Which flag to set
 *  \param Value Value of target flag
 */
void IParser::SetFlag(int Position,bool Value){
    LogError("SetFlag: %d",Position);

   // Flags.SetBit(500,true);//меню в крещендо

	Flags.SetBit(Position,Value);

	if (Value){
        if (!GlobalFlags.GetBit(Position))
            GlobalFlags.SetBit(Position,Value);
    }
}

/*! \brief Reads a flag
 *  \param Position Which flag to read
 *  \returns Value of read flag
 */
bool IParser::GetFlag(int Position){
LogError("GetFlag: %d",Position);
/*
500 - экстра в крещендо
501 - вход в меню картинок
*/
if (Flags.GetBit(501)||Position==500)
    return GlobalFlags.GetBit(Position);
else
	return Flags.GetBit(Position);
}

/*! \brief Sets a hitpattern
 */
void IParser::Setpattern(int Index,Uint8 *Buffer,int Size){
LogError("Setpattern: %d %d",Index);

	Patterns.SetBuffer(Index,Buffer,Size);
}

bool IParser::Hitpattern(Uint8 Index,Uint8 Command){
	LogError("Hitpattern: %d %d ",Index,Command);

	// Scour trough hitpattern
	bool retval=true;
	Uint8 *buffer;
	int size;
	Patterns.GetBuffer(Index,&buffer,&size);
	for(int i=0;retval && i<size && buffer[i]!=0xFF;){
		Uint16 pos=GETWORD(buffer+i);
		Uint16 cnt=GETWORD(buffer+i+2);
		i+=4;
		while(cnt--){
			Uint8 bit=buffer[i++];
			retval&=(bool)(Flags.GetUint8(pos++)&bit);
		}
	}
	return retval;
}

/*! \brief Extracts an opcode from the current script
 *  \param Opcode One byte buffer for the extracted opcode
 *  \param Buffer Pointer to the script data for extracted opcode
 *  \param Length Length of the opcode data
 *
 *  The opcode header has variying size, so the pointer and associated length
 *  only refers to the data part of the extracted data. The opcode and length
 *  fields are parsed directly to the relevant parameters
 */
bool IParser::GetOpcode(Uint8 *Opcode,const Uint8 **Buffer,Uint32 *Length){
	if(scriptstack){
		return scriptstack->GetOpcode(Opcode,Buffer,Length);
	}
	else{
		return false;
	}
}

/*! \brief Loads a new script while dropping all current data
 *  \param Script The script resource to load
 *  \returns True if script was successfully loaded
 */
bool IParser::LoadScript(uString Name,Uint8 *Buffer,int Length,int Pos){
	bool retval=false;
	IkuraScript *script=new IkuraScript();
	if(script->Load(Name,Pos,Buffer,Length)){
		while(scriptstack){
			IkuraScript *tmpptr=scriptstack->NextPtr;
			delete scriptstack;
			scriptstack=tmpptr;
		}
		scriptstack=script;
		retval=true;
	}
	else{
		delete script;
	}
	return retval;
}

/*! \brief Loads a new script while dropping the current one
 *  \param Script The script resource to load
 *  \returns True if script was successfully loaded
 */
bool IParser::JumpScript(uString Name,Uint8 *Buffer,int Length){
	bool retval=false;
	IkuraScript *script=new IkuraScript();
	if(script->Load(Name,Buffer,Length)){
		if(scriptstack){
			script->NextPtr=scriptstack->NextPtr;
			delete scriptstack;
		}
		scriptstack=script;
		retval=true;
	}
	else{
		delete script;
	}
	return retval;
}

/*! \brief Loads a new script while pushing the old one onto the stack
 *  \param Script The script resource to load
 *  \returns True if script was successfully loaded
 */

bool IParser::CallScript(uString Name,Uint8 *Buffer,int Length){
	bool retval=false;
	IkuraScript *script=new IkuraScript();
	if(script->Load(Name,Buffer,Length)){
		script->NextPtr=scriptstack;
		scriptstack=script;
		retval=true;
	}
	else{
		delete script;
	}

	return retval;
}

/*! \brief Returns from current subscript
 *  \returns True if a script is running
 */
bool IParser::ReturnScript(){
	if(scriptstack){
		// Remove the head script
		IkuraScript *tmpptr=scriptstack->NextPtr;
		delete scriptstack;
		scriptstack=tmpptr;
	}
	return scriptstack;
}

/*! \brief Jumps to an indexed position inside the current script
 *  \param Table Position index to jump to
 *  \returns True if jump was within bounds
 */
bool IParser::JumpFunction(Uint16 Table){
	if(scriptstack){
		return scriptstack->Jump(Table);
	}
	else{
		return false;
	}
}

/*! \brief Calls a subroutine at an indexed position inside the current script
 *  \param Table Position index to call
 *  \returns True if call was within bounds
 */
bool IParser::CallFunction(Uint16 Table){
	if(scriptstack){
		return scriptstack->Call(Table);
	}
	else{
		return false;
	}
}

/*! \brief Returns from a subroutine call
 *  \returns True if return could be executed
 */
bool IParser::ReturnFunction(){
	if(scriptstack){
		return scriptstack->Return();
	}
	else{
		return false;
	}
}


