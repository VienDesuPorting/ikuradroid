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

#include "ikuradecoder.h"
#include "iscript.h"

IkuraScript::IkuraScript(){
	ssoffset=0;
	revision=0;
	xorkey=0;
	reserved=0;
	buffer=0;
	length=0;
	jumptable=0;
	positions=0;
	NextPtr=0;
	save_pos=0;
}

IkuraScript::~IkuraScript(){
	if(buffer){
		delete [] buffer;
	}
	if(jumptable){
		delete [] jumptable;
	}
	while(positions){
		IkuraScriptPosition *tmppos=positions->nextptr;
		delete positions;
		positions=tmppos;
	}
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
bool IkuraScript::GetOpcode(Uint8 *Opcode,const Uint8 **Buffer,Uint32 *Length){
	bool retval=false;
	if(buffer && positions && positions->position<length){
		// Extract oplength
		retval=true;
		int pos=positions->position;
		if(buffer[pos+1]&0x80){
			// Length is two bytes (Between 0x80 and 0x7FFF)
			*Opcode=buffer[pos+0];
			*Length=(((buffer[pos+1]&0x7F)<<8)|buffer[pos+2])-3;
			*Buffer=buffer+pos+3;
			positions->position+=(*Length)+3;
		}
		else{
			// Length is one byte (Between 0x00 and 0x7F)
			*Opcode=buffer[pos+0];
			*Length=buffer[pos+1]-2;
			*Buffer=buffer+pos+2;
			positions->position+=(*Length)+2;
		}

#ifdef IKURADROID_AUTODRIVE
		// TEMPORARY full opcode trace for the automated choice test
		{
			static const struct{Uint8 op;const char*name;} traceops[]={
				{0x01,"LS"},{0x02,"LSBS"},{0x10,"CW"},{0x11,"CP"},{0x12,"CIR"},
				{0x13,"CPS"},{0x15,"CSET"},{0x16,"CWO"},{0x17,"CWC"},{0x18,"CC"},
				{0x19,"CCLR"},{0x1B,"CRND"},{0x04,"JP"},{0x05,"JS"},{0x06,"RT"},
				{0x07,"ONJP"},{0x08,"ONJS"},{0x31,"SK"},{0x33,"HF"},{0x3B,"HN"},
				{0x41,"HS"},{0x44,"CALC"},{0x47,"IF"},{0x84,"IH"},{0x85,"IG"},
				{0x86,"IGINIT"},{0x88,"IHK"},{0xE5,"OPSL"},{0xE6,"OPPROP"},
				{0xF1,"ATIMES"},{0xF2,"AWAIT"},{0xFA,"TIMERSET"},{0xE9,"TITLE"},
				{0x00,"ED"},{0x22,"WL"},{0x23,"WW"},{0x8D,"CLK"},{0x4A,"EXC"},{0x49,"EXS"},
			};
			for(unsigned ti=0;ti<sizeof(traceops)/sizeof(traceops[0]);ti++){
				if(traceops[ti].op==*Opcode){
					LogError("TRACEOP %s @%u len=%d",traceops[ti].name,pos,*Length);
					break;
				}
			}
		}
#endif
		// Store save information
		if(*Opcode==IOP_PM){
			rsave_name=save_name;
			rsave_pos=save_pos;

			save_name=sname;
			save_pos=positions->position;
			//LogError("rsave_pos %s-%d",rsave_name.c_str(),rsave_pos);
			//LogError("save_pos %s-%d",save_name.c_str(),save_pos);
		}
	}
	return retval;
}

/*! \brief Jumps to an indexed position inside the current script
 *  \param Table Position index to jump to
 *  \returns True if jump was within bounds
 */
bool IkuraScript::Jump(Uint16 Table){
	bool retval=false;
	if(positions && jumptable){
		positions->position=ssoffset+jumptable[Table];
		retval=true;
	}
	return retval;
}

/*! \brief Calls a subroutine at an indexed position inside the current script
 *  \param Table Position index to call
 *  \returns True if call was within bounds
 */
bool IkuraScript::Call(Uint16 Table){
	bool retval=false;
	if(positions){
		// Copy old position as a safety precausion and stack it
		IkuraScriptPosition *newpos=new IkuraScriptPosition;
		newpos->position=positions->position;
		newpos->nextptr=positions;
		positions=newpos;
		retval=Jump(Table);
	}
	return retval;
}

/*! \brief Returns from an earlier subroutine call
 *  \return True if a position was pushed
 */
bool IkuraScript::Return(){
	bool retval=false;
	if(positions){
		IkuraScriptPosition *newpos=positions->nextptr;
		delete positions;
		positions=newpos;
		retval=true;
	}
	return retval;
}

/*! \brief Saves script positions
 *  \param File Savegame file
 *  \return Name of saved script
 */
uString IkuraScript::Save(Savegame *File){
	// Store script positions
	int count=0;
	IkuraScriptPosition *tmpptr=positions;
	while(tmpptr){
		uString n=EDL_Format("%s-%d",save_name.c_str(),count++);
		int p=tmpptr->position;
		File->SaveUint32(n,p);
		tmpptr=tmpptr->nextptr;
	}
	File->SaveUint32(EDL_Format("%s-save",save_name.c_str()),rsave_pos);
	//LogError("SaveUint32 %s-%d",save_name.c_str(),save_pos);
	File->SaveUint32(EDL_Format("%s-size",save_name.c_str()),count);
	return save_name;
}

/*! \brief Reloads old state from savegame
 *  \param Engine Engine object to load scripts from
 *  \param File Savegame file to read old states from
 *  \param Script Name of the script to load
 */
bool IkuraScript::Load(IkuraDecoder *Engine,Savegame *File,uString Script){
	RWops *blob=0;
	if((blob=Engine->LoadScript(Script))){
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Load script
				Engine->DecodeScript(tbuffer,tlength);
				Load(Script,tbuffer,tlength);

				// Flush existing stack
				while(positions){
					IkuraScriptPosition *tmpptr=positions->nextptr;
					delete positions;
					positions=tmpptr;
				}

				// Recreate position stack
				Uint32 p=0;
				Uint32 count=0;
				IkuraScriptPosition *stackptr=0;
				File->LoadUint32(EDL_Format("%s-size",Script.c_str()),&count);
				for(unsigned int i=0;i<count;i++){
					uString n=EDL_Format("%s-%d",Script.c_str(),i);
					if(File->LoadUint32(n,&p)){
						if(stackptr){
							stackptr->nextptr=new IkuraScriptPosition;
							stackptr=stackptr->nextptr;
						}
						else{
							stackptr=new IkuraScriptPosition;
							positions=stackptr;
						}
						stackptr->position=p;
						stackptr->nextptr=0;
					}
				}

				// Reset safe position
				if(File->LoadUint32(EDL_Format("%s-save",Script.c_str()),&p)){
					rsave_pos=p;
					if(positions){
						positions->position=p;
					}
				}
			}
			delete [] tbuffer;
		}
		delete blob;
	}
	return true;
}

/*! \brief Loads an Ikura/DRS script and sets a starting position
 *  \param Filename Resource name
 *  \param Uint32 Pos Starting position
 *  \returns True if script was valid
 *
 *  This variation is used to load savegames!
 */
bool IkuraScript::Load(uString Name,Uint32 Pos,Uint8 *Buffer,Uint32 Length){
	bool retval=Load(Name,Buffer,Length);
	if(retval && Pos>0){
		positions->position=Pos;
	}
	return retval;
}

/*! \brief Loads an Ikura/DRS script and sets a starting position
 *  \param Filename Resource name
 *  \returns True if script was valid
 */
bool IkuraScript::Load(uString Name,Uint8 *Buffer,Uint32 Length){
	bool retval=false;
	if(!strncmp((char*)Buffer,"DigitalRomanceSystem.",21)){
		retval=LoadDRS(Name,Buffer,Length);
	}
	else{
		retval=LoadISF(Name,Buffer,Length);
	}
	return retval;
}

/*! \brief Loads an Ikura script
 *  \param Filename Resource name
 *  \returns True if script was valid
 */
bool IkuraScript::LoadISF(uString Name,Uint8 *Buffer,Uint32 Length){
	// Verify jumptable before conversion
	bool retval=false;
	ssoffset=Buffer[0]|(Buffer[1]<<8)|(Buffer[2]<<16)|(Buffer[3]<<24);
	int tablecount=(ssoffset-8)/4;
	double tabletest=(ssoffset-8)/(double)4;
	if(tablecount==tabletest){
		// Decrypt buffer
		if(buffer){
			delete [] buffer;
		}
		length=Length;
		buffer=new Uint8[length];
		for(unsigned int i=0;i<8;i++){
			buffer[i]=Buffer[i];
		}
		for(unsigned int i=8;i<length;i++){
			buffer[i]=(((Buffer[i]<<6)|(Buffer[i]>>2))&0xFF);
		}

		/*
		uString dumpname=Name+".isf.dump";
		FILE *f=fopen(dumpname.c_str(),"wb");
		if(f){
			fwrite(buffer,1,length,f);
			fclose(f);
			exit(99999);
		}
		*/

		// Create jumptable (Might be zero so pad it)
		if(jumptable){
			delete [] jumptable;
		}
		jumptable=new Uint32[tablecount+1];
		for(int i=0;i<tablecount;i++){
			jumptable[i]=buffer[(i*4)+8]|(buffer[(i*4)+9]<<8)|
				(buffer[(i*4)+10]<<16)|(buffer[(i*4)+11]<<24);
		}

		// Prepare a sane position
		while(positions){
			IkuraScriptPosition *tmppos=positions->nextptr;
			delete positions;
			positions=tmppos;
		}
		positions=new IkuraScriptPosition;
		positions->position=ssoffset;
		positions->nextptr=0;

		// Complete object data
		revision=buffer[5]|(buffer[4]<<8);
		xorkey=buffer[6];
		reserved=buffer[7];
		sname=Name;
		retval=true;

		// Verify metadata
		LogDebug("revision: 0x%04x",revision);
		LogDebug("xorkey:   0x%02x",xorkey);
		LogDebug("reserved:   0x%02x",reserved);
	}
	else{
		LogError("Illegal jumptable in script file %s (%d/%d)",
				Name.c_str(),tablecount,tabletest);
	}
	return retval;
}

/*! \brief Loads a Digital Romance System script
 *  \param Filename Resource name
 *  \returns True if script was valid
 */
bool IkuraScript::LoadDRS(uString Name,Uint8 *Buffer,Uint32 Length){
	// Decrypt buffer
	bool retval=false;
	int pos=0x19;
	if(buffer){
		delete [] buffer;
	}
	length=Length;
	buffer=new Uint8[length];
	for(int i=0;i<pos;i++){
		buffer[i]=Buffer[i];
	}
	for(int i=pos;i<length;i++){
		buffer[i]=(((Buffer[i]<<6)|(Buffer[i]>>2))&0xFF);
	}

	/*
	uString dumpname=Name+".drs.dump";
	FILE *f=fopen(dumpname.c_str(),"wb");
	if(f){
		fwrite(buffer,1,length,f);
		fclose(f);
		exit(99999);
	}
	*/

	// Create jump table (Might be zero so pad it)
	int tsize=buffer[pos+0]|(buffer[pos+1]<<8)|
		(buffer[pos+2]<<16)|(buffer[pos+3]<<24);
	int num=buffer[pos+4]|(buffer[pos+5]<<8)|
		(buffer[pos+6]<<16)|(buffer[pos+7]<<24);
	int tablecount=(tsize)/4;
	double tabletest=(tsize)/(double)4;
	pos+=8;
	if(jumptable){
		delete [] jumptable;
	}
	jumptable=new Uint32[tablecount+1];
	for(int i=0;i<tablecount;i++){
		jumptable[i]=buffer[(i*4)+pos+0]|(buffer[(i*4)+pos+1]<<8)|
			(buffer[(i*4)+pos+2]<<16)|(buffer[(i*4)+pos+3]<<24);
	}
	pos+=tsize;

	// Ignore unknown table
	tsize=buffer[pos+0]|(buffer[pos+1]<<8)|
		(buffer[pos+2]<<16)|(buffer[pos+3]<<24);
	num=buffer[pos+4]|(buffer[pos+5]<<8)|
		(buffer[pos+6]<<16)|(buffer[pos+7]<<24);
	pos+=tsize+8;

	// Ignore calltable
	tsize=buffer[pos+0]|(buffer[pos+1]<<8)|
		(buffer[pos+2]<<16)|(buffer[pos+3]<<24);
	num=buffer[pos+4]|(buffer[pos+5]<<8)|
		(buffer[pos+6]<<16)|(buffer[pos+7]<<24);
	pos+=tsize+8;


	// Extract script data
	tsize=buffer[pos+0]|(buffer[pos+1]<<8)|
		(buffer[pos+2]<<16)|(buffer[pos+3]<<24);
	pos+=4;
	//pos=Length-tsize;
LogError("SCRIPTTABLE:%d (%d-%d=%d)",tsize,Length,pos,Length-pos);

	// Prepare a sane position
	ssoffset=pos;
	while(positions){
		IkuraScriptPosition *tmppos=positions->nextptr;
		delete positions;
		positions=tmppos;
	}
	positions=new IkuraScriptPosition;
	positions->position=ssoffset;
	positions->nextptr=0;
	sname=Name;
	retval=true;
	return retval;
}


