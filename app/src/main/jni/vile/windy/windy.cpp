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

#include "windy.h"

EngineWindy::EngineWindy(int Width,int Height) : EngineVN(Width,Height){
	// Set defaults
	ssoffset=0x4000;
	ssbuffer=0;
	ssindex=0;
	sslength=0;
	lsoffset=0;
	lsbuffer=0;
	lsindex=0;
	lslength=0;
	tsindex=0;
	csbuffer=lsbuffer;
	csstack=&lsstack;
	csindex=&lsindex;
	cslength=&lslength;
	csoffset=&lsoffset;
	w_tv=0;
	whisper=0;
	sel_result=-1;
	sel_flag=-1;
	sel_address=0;
	processastext=true;
	tstype=WINDYSTUB_ERROR;
	state=WINDYSTATE_NORMAL;
	pausedstate=state;
	Pause();

	// Preallocate a good chunk of ram
	vars.SetUint16(0xffff,0);

	// Assign standard widgets
	w_bg=new Widget();
	w_bg->Move(0,0);
	w_bg->Resize(Width,Height);
	AddWidget(w_bg,VL_BACKGROUND);
}

EngineWindy::~EngineWindy(){
	if(ssbuffer){
		delete [] ssbuffer;
	}
	if(lsbuffer){
		delete [] lsbuffer;
	}
	if(whisper){
		delete whisper;
	}
}

/*! \brief Loads a script with common subroutines
 *	\param Script Name of the script resource to load
 *  \param Entrypoint Offset to start parsing from (Usually NULL)
 *	\return TRUE if script was found and loaded
 */
bool EngineWindy::LoadWindyRoutines(uString Script,Uint16 Entrypoint){
	bool retval=false;
	RWops *blob=0;
	if((blob=LoadScript(Script))){
		// Extract and verify data
		LogDebug("Loading routines: %s",Script.c_str());
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace blob
			blob->Seek(0,SEEK_SET);
			char *tbuffer=new char[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Find start
				int start=0;
				if(!strncmp(tbuffer,"WindySub",8)){
					// Skip the mumbojumbo part
					start=256;
				}

				// Decode input data
				bool toggle=true;
				for(int i=start;i<tlength;i++){
					toggle=!toggle;
					if(toggle){
						tbuffer[i]=tbuffer[i]+1;
					}
					else{
						tbuffer[i]=tbuffer[i]-1;
					}
				}

				// Check currentpointer
				if(csbuffer==ssbuffer && csbuffer!=lsbuffer){
					csbuffer=(Uint8*)tbuffer;
				}

				// Assign to system
				if(ssbuffer){
					delete [] ssbuffer;
				}
				ssbuffer=(Uint8*)tbuffer;
				sslength=tlength;
				ssindex=Entrypoint+start;
				retval=true;

				// Flush stacked positions
				while(ssstack.Pop());
			}
			else{
				delete [] tbuffer;
			}
		}

		// Free ops
		delete blob;
	}
	else{
		LogError("Cant find script: %s",Script.c_str());
	}
	return retval;
}


/*! \brief Loads a script and prepare it for parsing
 *	\param Script Name of the script resource to load
 *  \param Entrypoint Offset to start parsing from (Usually NULL)
 *	\return TRUE if script was found and loaded
 */
bool EngineWindy::LoadWindyScript(uString Script,Uint16 Entrypoint){
	bool retval=false;
	RWops *blob=0;
	uString sname=EDL_Searchname(Script);
	if(sname=="open" && Entrypoint==0){
		// Fall back to title screen
		EventGameDialog(VD_CREDITS);
		EventGameDialog(VD_TITLE);
		Pause();
		retval=true;
	}
	else if((blob=LoadScript(Script))){
		// Mark script as read if applicable
		UnlockScene(Script);

		// Syncronize voices
		if(whisper){
			whisper->Synchronize(EDL_Searchname(Script),stringlist);
		}

		// Extract and verify data
		LogDebug("Loading script: %s",Script.c_str());
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			// Replace blob
			blob->Seek(0,SEEK_SET);
			char *tbuffer=new char[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Find start
				int start=0;
				if(!strncmp(tbuffer,"WindySub",8)){
					LogError("Loading routines as script:%s",Script.c_str());
					start=256;
				}

				// Decode input data
				bool toggle=true;
				for(int i=start;i<tlength;i++){
					toggle=!toggle;
					if(toggle){
						tbuffer[i]=tbuffer[i]+1;
					}
					else{
						tbuffer[i]=tbuffer[i]-1;
					}
				}

				// Flush stacked positions
				while(ssstack.Pop());
				while(lsstack.Pop());

				// Assign to system
				if(lsbuffer){
					delete [] lsbuffer;
				}
				lsname=Script;
				lsbuffer=(Uint8*)tbuffer;
				lslength=tlength;
				lsindex=Entrypoint+start;
				saveindex=lsindex;
				ssindex=0;
				retval=true;

				// Set current
				csbuffer=lsbuffer;
				csstack=&lsstack;
				csindex=&lsindex;
				cslength=&lslength;
				csoffset=&lsoffset;

				// Flush variables ??????
				//for(int i=0;i<0x0100;i++){
				//	vars.SetUint16(i,0);
				//}
			}
			else{
				delete [] tbuffer;
			}
		}

		// Free ops and force state
		delete blob;
		state=WINDYSTATE_NORMAL;
	}
	else{
		LogError("Cant find script: %s",Script.c_str());
	}
	return retval;
}

bool EngineWindy::LoadGraphics(uString Resource,Uint16 Index,Uint16 Type){
	return false;
}

void EngineWindy::FillGraphics(Uint16 Index){
}

void EngineWindy::FillBackground(Uint32 RGBA){
	w_bg->Fill(RGBA);
}

Widget *EngineWindy::LoadSelection(Stringlist Strings,
		Uint16 R1,Uint16 R2,Uint16 A1,Uint16 A2){
	return 0;
}

Widget *EngineWindy::LoadSelection(Stringlist Strings){
	return 0;
}

bool EngineWindy::Call(Uint16 Address){
	if(Address-*csoffset<*cslength){
		// Pass request to clients
		bool process=false;
		if(*csoffset==0){
			if(!EventInternal(Address)){
				process=true;
			}
		}
		else{
			if(!EventExternal(Address)){
				process=true;
			}
		}

		// Process request ourselves
		if(process){
			Uint16 *oldindex=new Uint16;
			*oldindex=*csindex;
			csstack->Push(oldindex);
			*csindex=Address-*csoffset;
		}
	}
	else if(csbuffer==lsbuffer){
		if(!EventExternal(Address)){
			// Shift to external routines
			ssindex=Address-ssoffset;
			csbuffer=ssbuffer;
			csstack=&ssstack;
			csindex=&ssindex;
			cslength=&sslength;
			csoffset=&ssoffset;
		}
	}
	else{
		LogError("Invalid Call(0x%04x)",Address);
	}
	return false;
}

/*
 *  Return from routine (Either internal or external)
 */
bool EngineWindy::Return(){
	if(csstack->Peek()){
		Uint16 *oldindex=(Uint16*)csstack->Pop();
		*csindex=*oldindex;
		delete oldindex;
	}
	else if(csbuffer==ssbuffer){
		// Shift back to internal buffer
		csbuffer=lsbuffer;
		csstack=&lsstack;
		csindex=&lsindex;
		cslength=&lslength;
		csoffset=&lsoffset;
	}
	else{
		LogError("Invalid return (0x%04x)",*csindex);
	}
	return false;
}

bool EngineWindy::Jump(Uint16 Address){
	if(Address-*csoffset<*cslength){
		*csindex=Address-*csoffset;
	}
	else{
		LogError("Invalid jump: 0x%04x -> 0x%04x",Address,Address-*csoffset);
	}
	return false;
}

/*! \brief Temporarily halts execution
 *  \return True if execution is paused
 *
 *  This method should be used by derivated games to halt processing the
 *  native code until it has completed some specialized tasks (Such as the
 *  localization menu in nocturnal illusions.
 *
 *  This method will fail if the engine is in a non-processingstate upon
 *  calling it (Except when already paused).
 */
bool EngineWindy::Pause(){
	if(state!=WINDYSTATE_WAITCLIENT){
		pausedstate=state;
		state=WINDYSTATE_WAITCLIENT;
	}
	return (state==WINDYSTATE_WAITCLIENT);
}

/*! \brief Resumes native code processing
 *  \return True if execution is running as normal
 */
bool EngineWindy::Resume(){
	if(state==WINDYSTATE_WAITCLIENT){
		state=pausedstate;
	}
	return (state==WINDYSTATE_NORMAL);
}

Uint16 EngineWindy::GetValue(Uint16 Address){
	return vars.GetUint16(Address);
}

void EngineWindy::SetValue(Uint16 Address,Uint16 Value){
	vars.SetUint16(Address,Value);
}

void EngineWindy::SetSkipmode(bool Skip){
	EngineVN::SetSkipmode(Skip);
	w_tv->SetSkipmode(Skip);
}

void EngineWindy::SetMessageDelayEnabled(bool Enabled){
	EngineVN::SetMessageDelayEnabled(Enabled);
	if(Enabled){
		w_tv->SetTextInterval((GetMessageDelayInterval()/100.0)*200);
	}
	else{
		w_tv->SetTextInterval(0);
	}
}

void EngineWindy::SetMessageDelayInterval(int Interval){
	EngineVN::SetMessageDelayInterval(Interval);
	w_tv->SetTextInterval((GetMessageDelayInterval()/100.0)*200);
}


void EngineWindy::EventTextMode(Uint16 Mode,Uint16 X,Uint16 Y,
				Uint16 Width,Uint16 Height,Uint16 Flag){
}

void EngineWindy::EventBlit(Uint16 Opcode,Uint16 Index){
}

/*! \brief Allows derived games to override special functions
 *  \param Address Address of external routine
 *  \return True if call was handled internally
 *
 *  This can be used to override special functions such as setting date
 *  and time. These methods are often meaningless when emulating an updated
 *  version of the games.
 *
 *  Base method always does nothing and returns false to allow the real
 *  method execute normally.
 */
bool EngineWindy::EventExternal(Uint16 Address){
	return false;
}

bool EngineWindy::EventInternal(Uint16 Address){
	return false;
}

void EngineWindy::EventSelect(int Selection){
	// Store result and resume processing
	vars.SetUint16(sel_result,Selection+1);
	vars.SetUint16(sel_flag,0);
	if(sel_address){
		//Call(sel_address);
		Jump(sel_address);
	}

	// Free up widgets and return to processing
	DestroyLayer(VL_CHOICES);
	state=WINDYSTATE_NORMAL;
}

bool EngineWindy::EventGameTick(){
	// Check for queued up textstubs
	if(state==WINDYSTATE_WAITMORE){
		if(w_tv){
			bool skip=keyok || keyctrl() || GetSkipmode();
			if(skip && w_tv->GetRemainingText()){
				w_tv->CompleteText();
				keyok=false;
			}
			else if(skip){
				StopSound(VA_VOICES);
				ProcessText();
				keyok=false;
			}
		}
	}

	// Check for user input
	if(state==WINDYSTATE_WAITCLICK){
		if(w_tv){
			bool skip=keyok || keyctrl() || GetSkipmode();
			if(skip && w_tv->GetRemainingText()){
				w_tv->CompleteText();
				keyok=false;
			}
			else if(skip){
				StopSound(VA_VOICES);
				state=WINDYSTATE_NORMAL;
				keyok=false;
			}
		}
	}

	// Process only when all text is printed in normal state
	return !(state==WINDYSTATE_NORMAL && !w_tv->GetRemainingText());
}

void EngineWindy::EventNew(){
	vars.Clear();
	StopMusic();
	StopSound();
	w_tv->CompleteText();
	w_tv->ClearText();
	DestroyLayer(VL_CHOICES);
	sel_strings.Clear();
}

bool EngineWindy::EventLoad(int Index){
	bool retval=false;
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		// Clear existing data
		EngineWindy::EventNew();

		// Load script data
		load->LoadVector("variables",&vars);
		load->LoadString("scriptname",&lsname);
		load->LoadUint32("scriptpos",&lsindex);
		LoadWindyScript(lsname,lsindex);
		state=WINDYSTATE_NORMAL;

		// Load text data
		Uint8 tmpval;
		uString tmpstring;
		load->LoadUint32("textpos",&tsindex);
		load->LoadUint8("texttype",&tmpval);
		tstype=(WINDY_STUBTYPE)tmpval;

		// Load dynamic strings
		stringlist.Clear();
		load->LoadStringList("stringlist",&stringlist);

		// Load graphics
		SDL_Surface *tmps;
		if(load->LoadSurface("screen-display",&tmps)){
			w_bg->Blit(tmps);
			SDL_FreeSurface(tmps);
		}

		// Close dialog
		retval=true;
	}
	delete load;
	return retval;
}

bool EngineWindy::EventSave(int Index){
	// Gather date string
	uString date=EDL_DateString(EDL_UnixTime());
	uString time=EDL_TimeString(EDL_UnixTime());
	uString datetime=date+uString(" ")+time;

	// Store script data
	Savegame *save=new Savegame(NativeID(),Index);
	save->SaveString("scriptname",lsname);
	save->SaveUint32("scriptpos",saveindex);
	save->SaveVector("variables",&vars);
	save->SaveString("savedate",datetime.c_str());

	// Store text data
	save->SaveUint32("textpos",tsindex);
	save->SaveUint8("texttype",tstype);

	// Store dynamic strings
	save->SaveStringList("stringlist",&stringlist);

	// Store graphics
	SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
	Paint(screen,VL_CHOICES);
	save->SaveSurface("screen-thumb",screen,96,72);
	Paint(screen,VL_CHARACTERS);
	save->SaveSurface("screen-display",screen);
	SDL_FreeSurface(screen);

	// Close dialogs
	save->Write();
	delete save;
	return true;
}

bool EngineWindy::EventGameProcess(){
	bool retval=true;
	if(state==WINDYSTATE_NORMAL && csindex && *csindex<*cslength){
		// Parse word
		Uint16 opcode=GETWORD(csbuffer+*csindex);
		LogDebug("Processing opcode %04x:%04x",*csindex,opcode);
		*csindex+=2;

		// Believed-to-be-identified opcodes
	   	if(opcode==0x0002)				retval=OP0002();
		else if(opcode==0x0003)			retval=OP0003();
		else if(opcode==0x000A)			retval=OP000A();
		else if(opcode==0x000B)			retval=OP000B();
		else if(opcode==0x000C)			retval=OP000C();
		else if(opcode==0x0013)			retval=OP0013();
		else if(opcode==0x0014)			retval=OP0014();
		else if(opcode==0x0015)			retval=OP0015();
		else if(opcode==0x0018)			retval=OP0018();
		else if(opcode==0x0019)			retval=OP0019();
		else if(opcode==0x001A)			retval=OP001A();
		else if(opcode==0x001C)			retval=OP001C();
		else if(opcode==0x001F)			retval=OP001F();
		else if(opcode==0x0021)			retval=OP0021();
		else if(opcode==0x0022)			retval=OP0022();
		else if(opcode==0x0024)			retval=OP0024();
		else if(opcode==0x0025)			retval=OP0025();
		else if(opcode==0x0027)			retval=OP0027();
		else if(opcode==0x002D)			retval=OP002D();
		else if(opcode==0x002F)			retval=OP002F();
		else if(opcode==0x0038)			retval=OP0038();
		else if(opcode==0x003A)			retval=OP003A();
		else if(opcode==0x003E)			retval=OP003E();
		else if(opcode==0x003F)			retval=OP003F();
		else if(opcode==0x0040)			retval=OP0040();
		else if(opcode==0x0041)			retval=OP0041();
		else if(opcode==0x0042)			retval=OP0042();
		else if(opcode==0x0043)			retval=OP0043();
		else if(opcode==0x004A)			retval=OP004A();
		else if(opcode==0x004B)			retval=OP004B();
		else if(opcode==0x004F)			retval=OP004F();
		else if(opcode==0x0051)			retval=OP0051();
		else if(opcode==0x0052)			retval=OP0052();
		else if(opcode==0x0053)			retval=OP0053();
		else if(opcode==0x0054)			retval=OP0054();
		else if(opcode==0x0055)			retval=OP0055();
		else if(opcode==0x0059)			retval=OP0059();
		else if(opcode==0x005A)			retval=OP005A();
		else if(opcode==0x005E)			retval=OP005E();
		else if(opcode==0x0060)			retval=OP0060();
		else if(opcode==0x0062)			retval=OP0062();
		else if(opcode==0x0063)			retval=OP0063();
		else if(opcode==0x0064)			retval=OP0064();
		else if(opcode==0x006E)			retval=OP006E();
		else if(opcode==0x007B)			retval=OP007B();
		else if(opcode==0x0080)			retval=OP0080();
		else if(opcode==0x008F)			retval=OP008F();
		else if(opcode==0x0095)			retval=OP0095();
		//else if(opcode==0x0096)			retval=OP0096();

		// Unknown opcodes (Needed for correct parsing)
		else if(opcode==0x0004){
			*csindex+=2;
		}
		else if(opcode==0x0007){
			*csindex+=4;
		}
		else if(opcode==0x0008){
			*csindex+=4;
		}
		else if(opcode==0x0012){
			*csindex+=2;
		}
		else if(opcode==0x001E){
			*csindex+=2;
		}
		else if(opcode==0x0072){
			*csindex+=2;
		}
		else if(opcode==0x0076){
			*csindex+=16;
		}
		else if(opcode==0x0077){
			*csindex+=16;
		}
		else if(opcode==0x0093){
			*csindex+=2;
		}
		else if(opcode==0x0096){
			*csindex+=4;
		}

		// Report unknown
		else{
			LogDebug("%04x:UNKNOWN: 0x%04x",*csindex-2,opcode);
		}
	}

	// Check wether state was retained
	if(state!=WINDYSTATE_NORMAL){
		retval=true;
	}
	return retval;
}

bool EngineWindy::OP0002(){
	// Extract parameters
	//Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	Uint16 p4=GETWORD(csbuffer+*csindex+6);		// Target surface?
	Uint16 p5=GETWORD(csbuffer+*csindex+8);		// Image filename
	Uint16 p6=GETWORD(csbuffer+*csindex+10);	// Transparancy 0/1?
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);	// Always 1?
	*csindex+=14;
	
	// Set resource name
	uString name;
	if(p5>ssoffset){
		name=(char*)ssbuffer+1+(p5-ssoffset);
	}
	else{
		name=(char*)lsbuffer+1+p5;
	}

	// Load resource
	if(name.length()){
		if(LoadGraphics(name,p4,p6)){
			UnlockCG(name);
		}
	}
	else{
		LogError("Invalid bitmap reference: %04x",p5);
	}
	return false;
}

/*
 *  Configures text output (Includes option for old versions)
 *
 *	Before options:
 *		000C 0000 000A 0009 0008 0001
 *
 *	Before text:
 *		0003 0003 0013 0022 0006 0000
 *
 *  Before quote:
 *		0003 0007(X) 000A(Y) 001E(W) 0004(H) 0000
 */
bool EngineWindy::OP0003(){
	// Process data
	Uint16 p1=GETWORD(csbuffer+*csindex+0);		// Type or subcommand
	Uint16 p2=GETWORD(csbuffer+*csindex+2);		// X Coordinate
	Uint16 p3=GETWORD(csbuffer+*csindex+4);		// Y Coordinate
	Uint16 p4=GETWORD(csbuffer+*csindex+6);		// Width in characters
	Uint16 p5=GETWORD(csbuffer+*csindex+8);		// Height in characters
	Uint16 p6=GETWORD(csbuffer+*csindex+10);	// Clickable flag?
	*csindex+=12;

	// Pass gui change
	EventTextMode(p1,p2,p3,p4,p5,p6);
	return false;
}

/*! \brief Executes an unconditional jump instruction to its argument
 *  \return False
 */
bool EngineWindy::OP000A(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	Jump(p1);
	return false;
}

/* If-then blocks for comparing two registers
 */
bool EngineWindy::OP000B(){
	// Extract parameters
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	Uint16 p3=GETWORD(csbuffer+*csindex+4);
	Uint16 p4=GETWORD(csbuffer+*csindex+6);
	*csindex+=8;
	Sint16 v1=vars.GetSint16(p1);
	Sint16 v2=vars.GetSint16(p2);

	// Check operator
	if(p3==0x000D){
		// Equal?
		if(v1==v2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x000E){
		if(v1<v2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x000F){
		if(v1>v2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0010){
		if(v1<=v2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0011){
		if(v1>=v2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0012){
		// Not equal?
		if(v1!=v2){
			*csindex=p4-*csoffset;
		}
	}
	else{
		LogError("Invalid IF operator: 0x%04x",p3);
	}
	return false;
}

/* If-then blocks for comparing a register to an intermediate
 */
bool EngineWindy::OP000C(){
	// Extract parameters
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Sint16 p2=GETWORD(csbuffer+*csindex+2);	// Must be signed!
	Uint16 p3=GETWORD(csbuffer+*csindex+4);
	Sint16 p4=GETWORD(csbuffer+*csindex+6);
	*csindex+=8;
	Sint16 v=vars.GetSint16(p1);

	// Check operator
	if(p3==0x000D){
		// Equal?
		if(v==p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x000E){
		if(v<p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x000F){
		if(v>p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0010){
		if(v<=p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0011){
		if(v>=p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0012){
		// Not equal?
		if(v!=p2){
			*csindex=p4-*csoffset;
		}
	}
	else if(p3==0x0018){
		// This is probably a dirty hack
		// Necessary to ge through nocturnal illusion script 8_30.S
		if(v!=p2){
			*csindex-=4;
		}
		else{
			*csindex+=2;
		}
	}
	else{
		LogError("Invalid IF operator: 0x%04x (%x  %x)",p3,v,p2);
	}
	return false;
}

/* Executes switch-case jumps in tables of absolute byte positions
 *
 * The first parameter probably reffers to some sort of register or
 * variable which in turn refers a system parameter. Had to implement
 * this half-assed version to act upon selections.
 */
bool EngineWindy::OP0013(){
	// Extract jump offset
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;

	// Extract variable and jump according to the result
	Uint16 v=vars.GetUint16(p1);
	if((*csindex+(v*2))<*cslength){
		Uint16 p=GETWORD(csbuffer+*csindex+(v*2));
		if(p-*csoffset<*cslength){
			*csindex=p-*csoffset;
		}
		else{
			LogError("%04x:Invalid switch jump: var[0x%04x]=0x%04x=>0x%04x",
					*csindex-4,p1,v,p);
		}
	}
	else{
		LogError("%04x:Invalid switch value: var[0x%x]=0x%x",*csindex-4,p1,v);
	}
	return false;
}

bool EngineWindy::OP0014(){
	//Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);
	*csindex+=8;
	return false;
}

/* Loads addressed text stubs (Both text and selection options)
 */
bool EngineWindy::OP0015(){
	// Parse input
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	if(p2-*csoffset<*cslength){
		// Process stub as either text or option/menu
		tsindex=p2;
		if(processastext){
			LogDebug("Processing stubs as text ...");
			ProcessText();
		}
		else{
			LogDebug("Processing stubs as options ...");
			ProcessOptions();
		}
	}
	else{
		LogError("%04x:Invalid text stub: %04x %04x",*csindex-6,p1,p2);
	}
	return false;
}

/* Assigns an intermediate to a register
 */
bool EngineWindy::OP0018(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	vars.SetUint16(p1,p2);
	return false;
}

/* Assigns a register to another register
 */
bool EngineWindy::OP0019(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	Uint16 v2=vars.GetUint16(p2);
	*csindex+=4;
	vars.SetUint16(p1,v2);
	return false;
}

/*! \brief Loads a named bgm resource
 */
bool EngineWindy::OP001A(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	char b[12];
	strncpy(b,(char*)csbuffer+(p1-*csoffset),12);
	if(p1-*csoffset<*cslength){
		if(!PlayMusic(b+1)){
			LogError("Could not resolve BGM: %s",b+1);
		}
	}
	else{
		LogError("%04x:Invalid bgm resource: 0x%04x",*csindex-4,p1);
	}
	return false;
}

/*
 * Stops running track?
 */
bool EngineWindy::OP001C(){
	StopMusic();
	return false;
}

/* Loads a named script resource
 */
bool EngineWindy::OP001F(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	char b[12];
	strncpy(b,(char*)csbuffer+p1-*csoffset,12);
	LogDebug("%04x:Loading script: %s (0x%02x)",*csindex-4,b+1,b[0]);
	if(p1-*csoffset<*cslength && LoadWindyScript(b+1)){
	}
	else{
		LogError("%04x:Invalid script resource: 0x%04x (%s)",*csindex-4,p1,b+1);
	}
	return false;
}

/* Might be a be a decrement..might very well be not
 */
bool EngineWindy::OP0021(){
	//Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	return false;
}

/* Might be a be an increment..might very well be not
 */
bool EngineWindy::OP0022(){
	// Parse input
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	if(p1==0x03E8){
		// Enter option mode
		LogDebug("%04x:Preparing state:0022 %04x",*csindex-4,p1);
		processastext=false;

		// Savepoint for selections
		if(!lsstack.Count() && !ssstack.Count()){
			saveindex=lsindex-4;
		}
	}
	else if(p1==0x0120){
		LogDebug("%04x:Preparing state:0022 %04x",*csindex-4,p1);
		processastext=true;
	}
	else{
		LogError("Unknown state: 0x%x",p1);
	}
	return false;
}

/* Adds an intermediate to a register
 */
bool EngineWindy::OP0024(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Sint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint16 v=vars.GetSint16(p1);
	vars.SetSint16(p1,v+p2);
	return false;
}

/* Substracts an intermediate from a register
 */
bool EngineWindy::OP0025(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Sint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint16 v=vars.GetSint16(p1);
	vars.SetSint16(p1,v-p2);
	return false;
}

/* This is a rather dubious one ...
 */
bool EngineWindy::OP0027(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint16 v1=vars.GetSint16(p1);
	Sint16 v2=vars.GetSint16(p2);
	vars.SetSint16(p1,v1+v2);
	return false;
}

/*
 * Clears a range of variables by setting them to null
 */
bool EngineWindy::OP002D(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	for(int i=p1;i<p2;i++){
		vars.SetSint16(i,0);
	}
	return false;
}

/*
 *	Seem to be some kind of a blitter. The last two parameters might be
 *  bitmask, the other looks more like offsets and indexes.
 *
 *  We dont really care as we dont support native blitters.
 */
bool EngineWindy::OP002F(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);
	*csindex+=16;
	EventBlit(0x002F,p1);
	return false;
}

/*
 *  Combines two register using the AND operator:
 *    var[x]=var[x]&var[y]
 */
bool EngineWindy::OP0038(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Uint16 v1=vars.GetUint16(p1);
	Uint16 v2=vars.GetUint16(p2);
	vars.SetUint16(p1,v1&v2);
	return false;
}

/*
 *	Seem to be some kind of a blitter. The last two parameters might be
 *  bitmask, the other looks more like offsets and indexes.
 *
 *  This blitter takes registers as parameters instead of intermeditaries.
 *
 *  We dont support native blitters.
 */
bool EngineWindy::OP003A(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);		// Reg med integerverdi (0+)
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);	// Reg med integerverdi (0+)
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);	// Reg med koordinat (y akse)
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);	// Reg. Muligens flagg
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);	// Reg med integerverdi (0+)
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);	// Reg med integerverdi (0+)
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);	// Reg med integerverdi (10+)
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);	// Reg med koordinat (y akse)
	Uint16 v1=vars.GetUint16(p1);
	*csindex+=16;
	EventBlit(0x003A,v1);
	return false;
}

/*
 *  Divides two registers. First register gets divisor, seconds gets remainder.
 */
bool EngineWindy::OP003E(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint16 v1=vars.GetSint16(p1);
	Sint16 v2=vars.GetSint16(p2);
	if(v2){
		vars.SetSint16(p1,v1/v2);
		vars.SetSint16(p2,v1%v2);
	}
	return false;
}

bool EngineWindy::OP003F(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint32 v1=vars.GetSint16(p1);
	Sint32 v2=vars.GetSint16(p2);
	Sint32 v3=v1*v2;
	vars.SetSint16(p1,(v3>>16)&0xFFFF);
	vars.SetSint16(p2,v3&0xFFFF);
	return false;
}

/*
 *  Loads a selection dialog. The last two parameter can either be jumps or
 *  calls, not sure, both seemingly works.
 */
bool EngineWindy::OP0040(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0); // Register selection result?
	Uint16 p2=GETWORD(csbuffer+*csindex+2);	// Register mouse click?
#ifdef VILE_LOGGING_DEBUG
	Uint16 p3=GETWORD(csbuffer+*csindex+4);	// Routine for rightclick?
#endif
	Uint16 p4=GETWORD(csbuffer+*csindex+6);	// Routine for leftclick?
	*csindex+=8;
	LogDebug("%04x:Loading options: 0x%04x 0x%04x 0x%04x 0x%04x",
			*csindex-10,p1,p2,p3,p4);

	// Change state and load selection menu
	sel_result=p1;
	sel_flag=p2;
	sel_address=p4;
	state=WINDYSTATE_SELECT;
	AddWidget(LoadSelection(sel_strings),VL_CHOICES);
	sel_strings.Clear();
	return false;
}

/*
 *  Same as 0040, but for graphical menues
 *
 *  This is only used for the location menu under nocturnal illusions
 */
bool EngineWindy::OP0041(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0); // Register selection result?
	Uint16 p2=GETWORD(csbuffer+*csindex+2);	// Register mouse click?
	Uint16 p3=GETWORD(csbuffer+*csindex+4);
	Uint16 p4=GETWORD(csbuffer+*csindex+6);	// Routine for mouseover?
	*csindex+=8;
	LogDebug("%04x:Loading location: 0x%04x 0x%04x 0x%04x 0x%04x",
			*csindex-10,p1,p2,p3,p4);

	// Change state and load selection menu
	sel_result=p1;
	sel_flag=p2;
	sel_address=p4;
	state=WINDYSTATE_SELECT;
	AddWidget(LoadSelection(sel_strings,p1,p2,p3,p4),VL_CHOICES);
	sel_strings.Clear();
	return false;
}

bool EngineWindy::OP0042(){
	*csindex+=2;
	return false;
}

/*
 *  This might pull results off "windows" such as location control in ni.
 */
bool EngineWindy::OP0043(){
	*csindex+=4;
	return false;
}

bool EngineWindy::OP004A(){
	*csindex+=4;
	return false;
}

/*
 * Reads binary data (variables) from a file
 */
bool EngineWindy::OP004B(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	if(p2+1-*csoffset<*cslength){
		RWops *blob=LoadOther((char*)csbuffer+1+p2-*csoffset);
		if(blob){
			int l=blob->Seek(0,SEEK_END);
			blob->Seek(0,SEEK_SET);
			Uint8 b[l];
			if(blob->Read(b,l)>(int)0){
				for(int i=0;i<l;i++){
					vars.SetUint16(i+p1,b[i]);
				}
			}
			delete blob;
		}
		else{
			LogError("%04x:Invalid filename:%s",*csindex-6,
					csbuffer+1+p2-*csoffset);
		}
	}
	else{
		LogError("%04x:Invalid file reference: %04x %04x",
				*csindex-6,0x004B,p1,p2);
	}
	return false;
}

/* 
 *  Plays sound effect. Not compatible with new filenames
 */
bool EngineWindy::OP004F(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
#ifdef VILE_LOGGING_DEBUG
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
#endif
	*csindex+=4;

	// Determine effect name
	char fn[32]={0};
	sprintf(fn,"SE%05d.WAV",p1);

	// TODO: Repeat effect p2 times

	// Play effect
	LogDebug("%04x:PlaySound:%04x %04x:%s",*csindex-2,p1,p2,fn);
	PlaySound(fn);
	return false;
}

bool EngineWindy::OP0051(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Uint16 v=vars.GetUint16(p1);
	vars.SetUint16(p1,p2&v);
	return false;
}

/*
 *  Or register with intermediate
 *
 *  This is confirmed used to set flags via registers.
 */
bool EngineWindy::OP0052(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Uint16 v=vars.GetUint16(p1);
	vars.SetUint16(p1,p2|v);
	return false;
}

/*  Copies a string resource to another position
 */
bool EngineWindy::OP0053(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	uString s1=stringlist.GetString(p1);
	uString s2=stringlist.GetString(p2);
	stringlist.SetString(p1,s2);
	LogDebug("STRINGLIST:COPY:%d=[%d]%s",p1,p2,s2.c_str());
	//vars.SetUint16(p1,v2);
	return false;
}

/*
 *  Call routine. Either internally in the loaded script, or externally
 *  from the common routines. The latter might be overridden in order to
 *  accommodate updated uis.
 */
bool EngineWindy::OP0054(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	*csindex+=2;
	Call(p1);
	return false;
}

/*
 *  Return from routine (Either internal or external)
 */
bool EngineWindy::OP0055(){
	Return();
	return false;
}

/*
 *  Load string resources such as names
 */
bool EngineWindy::OP0059(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	if(p2-*csoffset<*cslength){
		uString s1=stringlist.GetString(p1);
		LogDebug("%04x:stringlist[%d]=%04x=%s+%s",
			*csindex-6,p1,p2,s1.c_str(),csbuffer+p2-*csoffset);
		stringlist.SetString(p1,s1+(char*)(csbuffer+p2-*csoffset));
		//vars.SetUint16(p1,stringlist.GetString(p1).length());
	}
	else{
		LogError("%04x:Invalid string load:%04x %04x",*csindex-6,p1,p2);
	}
	return false;
}

/*
 *  Clear text stubs loaded with 0x0059
 */
bool EngineWindy::OP005A(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	stringlist.SetString(p1,"");
	//vars.SetUint16(p1,0);
	*csindex+=2;
	return false;
}

bool EngineWindy::OP005E(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);
	*csindex+=10;
	FillGraphics(p1);
	return false;
}

/* Probably used for loading savegames in legacy game
 */
bool EngineWindy::OP0060(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	Uint16 p3=GETWORD(csbuffer+*csindex+4);
	*csindex+=6;
	vars.SetUint16(p1,0);
	vars.SetUint16(p2,0);
	LogMessage("%04x:Check savegame:i %x %x %s",
			*csindex-8,p1,p2,csbuffer+1+p3-*csoffset);
	return false;
}

/*
 * Oh yeah, the beloved and highly necessary 'shake' effect
 */
bool EngineWindy::OP0062(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	Sint16 v1=vars.GetSint16(p1);
	Sint16 v2=vars.GetSint16(p2);
	w_bg->Move(v1*10,v2*10);
	return true;
}

/*
 *	Seem to be some kind of a blitter. The last two parameters might be
 *  bitmask, the other looks more like offsets and indexes.
 */
bool EngineWindy::OP0063(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);		// Index for kilde?
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);	// Flagg eller subkommando??
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);	// Flagg??
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);	// Lavt integer?
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);	// Hoyde?
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);	// Koordinat?
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);	// Koordinat?
	*csindex+=16;
	EventBlit(0x0063,p1);
	return false;
}

bool EngineWindy::OP0064(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);
	Uint16 v1=vars.GetUint16(p1);
	*csindex+=16;
	EventBlit(0x0064,v1);
	return false;
}

bool EngineWindy::OP006E(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	uString s1=stringlist.GetString(p1);
	uString s2=stringlist.GetString(p2);
	LogDebug("%04x:STRING:LENGTH: vars[%x]={%s}=%d",
			*csindex-6,p1,s2.c_str(),s2.length());
	stringlist.SetString(p1,s2);
	vars.SetUint16(p1,s2.length());
	return false;
}

bool EngineWindy::OP007B(){
	*csindex+=4;
	return false;
}

/*
 *  Load string resources such as names and uses them as keys for the inifile
 */
bool EngineWindy::OP0080(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	if(p2-*csoffset<*cslength){
		LogDebug("%04x:STRING:INI:%04x=%04x=%s",
			*csindex-6,p1,p2,csbuffer+1+p2-*csoffset);
		stringlist.SetString(p1,(char*)csbuffer+1+p2-*csoffset);
		stringlist.SetString(p1+1,"Shinichi");
		//vars.SetUint16(p1,stringlist.GetString(p1).length());
		//vars.SetUint16(p1+1,stringlist.GetString(p1+1).length());
	}
	else{
		LogError("%04x:Invalid string ini:%04x %04x",*csindex-6,p1,p2);
	}
	return false;
}

/* Loads a named script resource
 */
bool EngineWindy::OP008F(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	Uint16 p2=GETWORD(csbuffer+*csindex+2);
	*csindex+=4;
	char b[12];
	strncpy(b,(char*)csbuffer+p2-*csoffset,12);
	LogDebug("%04x:Loading routines: %s (0x%02x)",*csindex-4,b+1,b[0]);
	if(p2-*csoffset<*cslength && LoadWindyRoutines(b+1)){
	}
	else{
		LogError("%04x:Invalid routines: 0x%04x",*csindex-4,p1);
	}
	return false;
}

/*
 * Blitter that is mostly used in nocturnal illusion. Seems to blit a image
 * (either opaque or transparent) from the first buffer to the display.
 *
 * Strangely, it seems like the source is deleted upon using this blitter.
 */
bool EngineWindy::OP0095(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);		// Always 0?
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);	// Always 0?
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);	// X Coordinate
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);	// Width
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);
	*csindex+=16;
	EventBlit(0x0095,p1);
	return false;
}

bool EngineWindy::OP0096(){
	Uint16 p1=GETWORD(csbuffer+*csindex+0);
	//Uint16 p2=GETWORD(csbuffer+*csindex+2);
	//Uint16 p3=GETWORD(csbuffer+*csindex+4);
	//Uint16 p4=GETWORD(csbuffer+*csindex+6);
	//Uint16 p5=GETWORD(csbuffer+*csindex+8);
	//Uint16 p6=GETWORD(csbuffer+*csindex+10);
	//Uint16 p7=GETWORD(csbuffer+*csindex+12);
	//Uint16 p8=GETWORD(csbuffer+*csindex+14);
	//Uint16 p9=GETWORD(csbuffer+*csindex+16);
	//Uint16 p10=GETWORD(csbuffer+*csindex+18);
	//Uint16 p11=GETWORD(csbuffer+*csindex+20);
	*csindex+=22;
	EventBlit(0x0096,p1);
	return false;
}

/*! \brief Processes text from csbuffer[tsindex]
 *  \return True if options was successfully parsed
 */
bool EngineWindy::ProcessOptions(){
	bool retval=true;
	int tsstart=tsindex;
	uString ptext;
	while(tsindex<*cslength && csbuffer[tsindex]){
		if(csbuffer[tsindex]==0x0F){
			// Inline ascii is not supported
			tsindex++;
			tsindex++;
		}
		else if(csbuffer[tsindex]==0x1B){
			// Exchange constants
			Uint16 ref=csbuffer[tsindex++];
			ref=csbuffer[tsindex++];
			uString ttext=stringlist.GetString(ref);
			LogDebug("Appending string: %d=[%s]",ref,ttext.c_str());
			ptext+=ttext;
		}
		else if(csbuffer[tsindex]<0x0D){
			// Drop control data
			LogDebug("Dropping control character:%x",csbuffer[tsindex]);
			tsindex++;
		}
		else{
			// Accumelate data
			ptext+=(char)csbuffer[tsindex++];
		}
	}


	// Build selection dialog
	if(ptext.length()){
		// Parse selection captions
		int selcount=sel_strings.GetCount();
		unsigned int start=0;
		for(unsigned int i=0;i<ptext.length();i++){
			if(ptext[i]=='\r'){
				if(i-start>0){
					uString tmpstring=ptext.substr(start,i-start);
					sel_strings.SetString(selcount++,tmpstring);
				}
				start=i+1;
			}
		}
		if(start<ptext.length()){
			uString tmpstring=ptext.substr(start);
			sel_strings.SetString(selcount++,tmpstring);
		}
	}
	else{
		LogError("Failed to parse options at: 0x%04x",tsstart);
		retval=false;
	}
	return retval;
}

/*! \brief Processes text from csbuffer[tsindex]
 *  \return True if still processing text
 */
WINDY_STUBTYPE EngineWindy::ProcessText(){
	// Fall back to processing mode per default
	state=WINDYSTATE_NORMAL;

	// Process stub
	uString ptext;
	int tsstart=tsindex;
	int res=ParseText(&ptext);
	if(res==WINDYSTUB_DEFAULT && !ptext.length()){
	}
	else if(res==WINDYSTUB_SELECTION){
		LogError("Processing text as selection: %04x",tsstart);
	}
	else if(res==WINDYSTUB_ERROR){
		if(ptext.length()){
			// Report as error
			LogError("Error when processing text: %02x %02x %02x",
					csbuffer[tsstart+0],
					csbuffer[tsstart+1],
					csbuffer[tsstart+2]);
		}
	}
	else if(!w_tv){
		LogError("Invalid textview control");
	}
	else{
		// Act upon previous text action
		if(tstype==WINDYSTUB_CLEARSCREEN){
			w_tv->ClearText();
		}

		// Register current text type
		tstype=(WINDY_STUBTYPE)res;
		if(tstype!=WINDYSTUB_DEFAULT){
			state=WINDYSTATE_WAITMORE;
		}

		// Load voice if any
		if(whisper){
			uString voice=whisper->GetVoice(tsstart);
			if(voice.length()){
				PlayVoice(voice);
			}
		}

		// Detect quoted names
		if(ptext[0]==(char)0x5B){
			// Assert clear screen when printing conversations
			w_tv->ClearText();

			// Find endquote and print name immidiatly
			for(unsigned int i=1;i<ptext.length();i++){
				if(ptext[i]==(char)0x5D){
					w_tv->PrintText(ptext.substr(0,i+1));
					w_tv->CompleteText();
					ptext=ptext.substr(i+1);
					break;
				}
			}
		}

		// Print remaining text normally
		w_tv->PrintText(ptext);

		// Store position
		if(!lsstack.Count() && !ssstack.Count()){
			saveindex=lsindex-6;
		}
	}
	return tstype;
}

/*! \brief Parses text from a decoded script
 *  \param String Where to place extracted data
 *  \return See WINDY_STUBTYPE
 */
WINDY_STUBTYPE EngineWindy::ParseText(uString *String){
	WINDY_STUBTYPE retval=WINDYSTUB_ERROR;
	if(String && tsindex<*cslength){
		// Parse string and datatype
		(*String)="";
		retval=WINDYSTUB_DEFAULT;
		while(1){
			if(csbuffer[tsindex]<=WINDYSTUB_SELECTION){
				// Handle inline command
				retval=(WINDY_STUBTYPE)csbuffer[tsindex++];
				break;
			}
			else if(csbuffer[tsindex]==WINDYSTUB_CONSTANT){
				// Change constant with name
				uString str=stringlist.GetString(csbuffer[tsindex+1]);
				if(!str.length()){
					str="Shinichi";
				}
				(*String)+=str;
				tsindex++;
				tsindex++;
			}
			else if(csbuffer[tsindex]==0x0F){
				// Think this is some kind of inline ascii codes
				(*String)+=' ';
				tsindex++;
				tsindex++;
			}
			else if(csbuffer[tsindex]>'~'){
				Uint16 gaiji=GETWORD(csbuffer+tsindex);
				tsindex+=2;
				if(gaiji==0xB4EB){
					(*String)+="!!";
				}
				else if(gaiji==0xB5EB){
					(*String)+="!";
				}
				else if(gaiji==0xB6EB){
					(*String)+="?";
				}
				else if(gaiji==0xB8EB){
					(*String)+="!!";	// Actually with droplets  ...
				}
				else if(gaiji==0xC1EB){
					(*String)+="!?";
				}
				else if(gaiji==0xC2EB){
					(*String)+="!!";
				}
				else if(gaiji==0xC3EB){
					(*String)+="??";
				}
				else if(gaiji==0xC3EB){
					(*String)+="..";	// Actually with droplets  ...
				}

				else{
					LogDebug("\tUnknown gaiji:%04x",gaiji);
				}
			}
			else if(csbuffer[tsindex]==0x1B){
				// Exchange constants
				Uint16 ref=csbuffer[tsindex++];
				ref=csbuffer[tsindex++];
				uString ttext=stringlist.GetString(ref);
				LogDebug("Appending string: %d=%s",ref,ttext.c_str());
				(*String)+=ttext;
			}
			else if(csbuffer[tsindex]>='\t' && csbuffer[tsindex]<='~'){
				// Accumelate buffer
				(*String)+=(char)csbuffer[tsindex++];
			}
			else{
				// Drop unrecognized code
				LogDebug("Unknown text code: 0x%02x",csbuffer[tsindex]);
				tsindex++;
			}
		}

		// Assert returnvalue
		if(retval>WINDYSTUB_WAITFORUSER){
			retval=WINDYSTUB_ERROR;
		}
		if(retval==WINDYSTUB_ERROR && String->length()){
			LogError("Dropping errenous text:%s",String->c_str());
			(*String)="";
		}
	}
	return retval;
}


