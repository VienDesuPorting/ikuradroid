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

#include "tlove.h"

#define GETBYTE(B)				((B)[0])
#define GETWORD(B)				((B)[1]|((B)[0]<<8))
#define GETDWORD(B)				((B)[3]|((B)[2]<<8)|((B)[1]<<16)|((B)[0]<<24))


Truelove::Truelove(uString Path) : EngineVN(640,480){
	// Add resources
	AddBGM(new ArchiveTLove(Path+"MIDI"));
	AddSE(new ArchiveTLove(Path+"EFF"));
	AddScripts(new ArchiveTLove(Path+"DATE"));
	AddImages(new ArchiveTLove(Path+"MRS"));

	// Set defaults
	scriptdata=0;
	scriptpos=0;
	state=TLSTATE_NORMAL;
	for(int i=0;i<TLBUFFER_SIZE;i++){
		cache[i]=EDL_CreateSurface(640,480);
	}

	// Load gui interface
	display=new Widget(0,40,640,400);
	textview=new Textview(this);
	selection=new Selection(this);
	AddWidget(display,VL_BACKGROUND);
	AddWidget(textview,VL_TEXTVIEW);
	AddWidget(selection,VL_CHOICES);

	// Configure GUI directly (No other titles for this engine)
	display->Fill(0x000000FF);
	textview->SetTextPosition(0,0,440,70);
	textview->MoveDialog(100,396);
	textview->Resize(440,70);
	textview->Fill(0x000000FF);

	LoadTrueloveScript("MAIN");
}

Truelove::~Truelove(){
	Stop();
	for(int i=0;i<TLBUFFER_SIZE;i++){
		SDL_FreeSurface(cache[i]);
	}
}

const uString Truelove::NativeID(){
	return "True";
}

const uString Truelove::NativeName(){
	return "True Love";
}

void Truelove::EventHostMouseMove(SDL_Surface *Screen,int X,int Y){
	mousex=GetRelativeX(Screen,X);
	mousey=GetRelativeX(Screen,Y);
	EngineBase::EventHostMouseMove(Screen,X,Y);
}

/*! \brief Tells wether a script is loaded
 *  \returns True if a script is properly loaded
 */
bool Truelove::Running(){
	return scriptdata;
}

/*! \brief Stops the engine and flushes any script data
 */
void Truelove::Stop(){
	while(scriptdata){
		SCRIPTDATA *tmp=scriptdata;
		scriptdata=scriptdata->nextptr;
		if(tmp->buffer){
			delete [] tmp->buffer;
		}
		delete tmp;
	}
	while(scriptpos){
		SCRIPTPOS *tmp=scriptpos;
		scriptpos=scriptpos->nextptr;
		delete tmp;
	}
}

/*! \brief Jumps to an position in the jumptable
 *  \param Position Jumptable position
 *  \return True if Jump was successfull
 */
bool Truelove::Jump(unsigned int Position){
	bool retval=false;
	if(scriptpos){
		scriptpos->index=scriptdata->table.GetUint16(Position);
		retval=true;
	}
	return retval;
}

/*! \brief Allocates new position and performs jump
 *  \param Position Jumptable position
 *  \return True if Call was successfull
 */
bool Truelove::Call(unsigned int Position){
	bool retval=false;
	if(scriptpos){
		SCRIPTPOS *pos=new SCRIPTPOS;
		pos->index=scriptpos->index;
		pos->data=scriptpos->data;
		pos->nextptr=scriptpos;
		scriptpos=pos;
		retval=Jump(Position);
	}
	return retval;
}

/*! \brief Returns from call by deallocating top position
 *  \return True if Return was successfull
 */
bool Truelove::Return(){
	bool retval=false;
	if(scriptpos){
		SCRIPTPOS *pos=scriptpos;
		scriptpos=scriptpos->nextptr;
		if(scriptpos && scriptpos->data!=pos->data){
			// Change back to old script
			while(scriptdata && scriptpos->data!=scriptdata){
				SCRIPTDATA *oldptr=scriptdata;
				scriptdata=scriptdata->nextptr;
				SCRIPTDATA *tmpptr=oldptr;
				while(tmpptr->nextptr){
					tmpptr=tmpptr->nextptr;
				}
				scriptdata->nextptr=oldptr;
				oldptr->nextptr=0;
			}

			// Purge unnused scripts
			Purge();
		}
		delete pos;
		retval=true;
	}
	return retval;
}

/*! \brief Purges any scriptdata that is not reffered by any positions
 */
void Truelove::Purge(){
	SCRIPTDATA *tmpdata=scriptdata;
	while(tmpdata){
		// Check if script is reffered by any active positions
		bool hit=false;
		SCRIPTPOS *tmppos=scriptpos;
		while(tmppos){
			if(tmppos->data==tmpdata){
				hit=true;
				break;
			}
			tmppos=tmppos->nextptr;
		}

		if(hit){
			// Skip to next script
			tmpdata=tmpdata->nextptr;
		}
		else if(tmpdata==scriptdata){
			// Remove head
			LogVerbose("Purging scriptdata: %s",tmpdata->name.c_str());
			scriptdata=scriptdata->nextptr;
			if(tmpdata->buffer){
				delete [] tmpdata->buffer;
			}
			delete tmpdata;
			tmpdata=scriptdata;
		}
		else{
			// Find and remove from stack
			LogVerbose("Purging scriptdata: %s",tmpdata->name.c_str());
			SCRIPTDATA *tmpptr=scriptdata;
			while(tmpptr->nextptr!=tmpdata){
				tmpptr=tmpptr->nextptr;
			}
			tmpptr->nextptr=tmpdata->nextptr;

			if(tmpdata->buffer){
				delete [] tmpdata->buffer;
			}
			delete tmpdata;
			tmpdata=tmpptr;
		}
	}
}


/*! \brief Loads a Truelove script and pushes it on top of the stack
 *  \param Script Name of the script file
 *  \return True if script loaded
 */
bool Truelove::LoadTrueloveScript(uString Script){
	bool retval=false;
	RWops *blob=0;
	if((blob=LoadScript(Script,"DAT"))){
		// Extract and verify data
		LogVerbose("Loading Truelove script: %s",Script.c_str());
		int fulllength=blob->Seek(0,SEEK_END);
		blob->Seek(0,SEEK_SET);
		if(fulllength>(int)0){
			// Create a scriptdata structure
			SCRIPTDATA *script=new SCRIPTDATA;
			script->name=Script;

			// Get table length
			unsigned char wbuffer[2];
			int scriptstart=0;
			if(blob->Read(wbuffer,2)==2){
				scriptstart=GETWORD(wbuffer);
			}

			// Read jumptable
			int tablelength=scriptstart-12;
			script->length=fulllength-(tablelength+2);
			unsigned char tbuffer[tablelength+1];
			if(blob->Read(tbuffer,tablelength)==tablelength){
				for(int i=0;i<tablelength;i+=2){
					Uint16 value=GETWORD(tbuffer+i);
					if(value<script->length){
						script->table.SetUint16(i/2,value);
					}
					else{
						LogError("Invalid table: %d/%d",value,script->length);
					}

				}
			}

			// Read version information
			if(blob->Read(wbuffer,2)==2){
				char vbuffer[32];
				int vlength=GETWORD(wbuffer)-2;
				if(blob->Read(vbuffer,vlength)>(int)0){
					vbuffer[vlength]=0;
					script->length=script->length-(vlength+2);
					LogVerbose("Reading Truelove version: %s",vbuffer+4);
				}
			}

			// Read script data
			if(script->length>0){
				script->buffer=new unsigned char[script->length];
				unsigned int read=blob->Read(script->buffer,script->length);
				if(read==script->length){
					retval=true;
				}
				else{
					delete [] script->buffer;
				}
			}

			// Stack the script and allocate a position
			if(retval){
				SCRIPTPOS *pos=new SCRIPTPOS;
				pos->index=0;
				pos->data=script;
				pos->nextptr=scriptpos;
				scriptpos=pos;
				script->nextptr=scriptdata;
				scriptdata=script;
			}
			else{
				LogError("Failed to load script: %s",Script.c_str());
				delete script;
			}
		}
		delete blob;
	}
	return retval;
}

bool Truelove::EventLoad(int Index){
	bool retval=false;
	return retval;
}

bool Truelove::EventSave(int Index){
	return false;
}

void Truelove::EventSelect(int Selection){
	if(scriptdata && (unsigned int)Selection<scriptdata->length){
		Jump(Selection);
		state=TLSTATE_NORMAL;
		selection->SetVisible(false);
	}
}

bool Truelove::EventGameTick(){
	if(state==TLSTATE_WAITCLICK){
		bool skip=keyok || keyctrl() || GetSkipmode();
		if(skip && textview->GetRemainingText()){
			textview->CompleteText();
			keyok=false;
		}
		else if(skip){
			StopSound(VA_VOICES);
			state=TLSTATE_NORMAL;
			textview->ClearText();
			keyok=false;
		}
	}
	return !(state==TLSTATE_NORMAL && !textview->GetRemainingText());
}

bool Truelove::EventGameProcess(){
	bool retval=true;
	if(scriptpos && scriptdata && scriptpos->index<scriptdata->length){
		unsigned int off=0;
		unsigned int index=scriptpos->index;
		Uint8 opcode=GETBYTE(scriptdata->buffer+index+off++);
		Uint8 oplen=GETBYTE(scriptdata->buffer+index+off++);
		if(oplen&0x80){
			oplen=GETBYTE(scriptdata->buffer+index+off++)|((oplen&0x7F)<<8);
		}
		if(index+off+oplen<=scriptdata->length){
			// Handle opcodes
			//LogTest("OPCODE:%04d:%x (%d)",index,opcode,oplen);
			scriptpos->index+=off;
			Uint8 *opdata=scriptdata->buffer+scriptpos->index;
			scriptpos->index+=oplen;
			if(opcode==0x00)				retval=OP00(opdata,oplen);
			else if(opcode==0x04)			retval=OP04(opdata,oplen);
			else if(opcode==0x16)			retval=OP16(opdata,oplen);
			else if(opcode==0x17)			retval=OP17(opdata,oplen);
			else if(opcode==0x19)			retval=OP19(opdata,oplen);
			else if(opcode==0x1B)			retval=OP1B(opdata,oplen);
			else if(opcode==0x20)			retval=OP20(opdata,oplen);
			else if(opcode==0x23)			retval=OP23(opdata,oplen);
			else if(opcode==0x24)			retval=OP24(opdata,oplen);
			else if(opcode==0x28)			retval=OP28(opdata,oplen);
			else if(opcode==0x2B)			retval=OP2B(opdata,oplen);
			else if(opcode==0x30)			retval=OP30(opdata,oplen);
			else if(opcode==0x31)			retval=OP31(opdata,oplen);
			else if(opcode==0x32)			retval=OP32(opdata,oplen);
			else if(opcode==0x33)			retval=OP33(opdata,oplen);
			else if(opcode==0x34)			retval=OP34(opdata,oplen);
			else if(opcode==0x35)			retval=OP35(opdata,oplen);
			else if(opcode==0x36)			retval=OP36(opdata,oplen);
			else if(opcode==0x38)			retval=OP38(opdata,oplen);
			else if(opcode==0x39)			retval=OP39(opdata,oplen);
			else if(opcode==0x3A)			retval=OP3A(opdata,oplen);
			else if(opcode==0x3C)			retval=OP3C(opdata,oplen);
			else if(opcode==0x40)			retval=OP40(opdata,oplen);
			else if(opcode==0x41)			retval=OP41(opdata,oplen);
			else if(opcode==0x42)			retval=OP42(opdata,oplen);
			else if(opcode==0x43)			retval=OP43(opdata,oplen);
			else if(opcode==0x44)			retval=OP44(opdata,oplen);
			else if(opcode==0x48)			retval=OP48(opdata,oplen);
			else if(opcode==0x49)			retval=OP49(opdata,oplen);
			else if(opcode==0x4A)			retval=OP4A(opdata,oplen);
			else if(opcode==0x4B)			retval=OP4B(opdata,oplen);
			else if(opcode==0x4C)			retval=OP4C(opdata,oplen);
			else if(opcode==0x52)			retval=OP52(opdata,oplen);
			else if(opcode==0x53)			retval=OP53(opdata,oplen);
			else if(opcode==0x54)			retval=OP54(opdata,oplen);
			else if(opcode==0x61)			retval=OP61(opdata,oplen);
			else if(opcode==0x62)			retval=OP62(opdata,oplen);
			else if(opcode==0x63)			retval=OP63(opdata,oplen);
			else if(opcode==0x66)			retval=OP66(opdata,oplen);
			else if(opcode==0x67)			retval=OP67(opdata,oplen);
			else if(opcode==0x70)			retval=OP70(opdata,oplen);
			else if(opcode==0x71)			retval=OP71(opdata,oplen);
			else if(opcode==0x72)			retval=OP72(opdata,oplen);
			else if(opcode==0x73)			retval=OP73(opdata,oplen);
			else if(opcode==0x75)			retval=OP75(opdata,oplen);
			else if(opcode==0x82)			retval=OP82(opdata,oplen);
			else if(opcode==0x83)			retval=OP83(opdata,oplen);
			else if(opcode==0x84)			retval=OP84(opdata,oplen);
			else if(opcode==0x85)			retval=OP85(opdata,oplen);
			else if(opcode==0x86)			retval=OP86(opdata,oplen);
			else if(opcode==0x87)			retval=OP87(opdata,oplen);
			else if(opcode==0x89)			retval=OP89(opdata,oplen);
			else if(opcode==0x8A)			retval=OP8A(opdata,oplen);
			else if(opcode==0x91)			retval=OP91(opdata,oplen);
			else if(opcode==0x92)			retval=OP92(opdata,oplen);
			else if(opcode==0x95)			retval=OP95(opdata,oplen);
			else if(opcode==0x98)			retval=OP98(opdata,oplen);
			else if(opcode==0x99)			retval=OP99(opdata,oplen);
			else if(opcode==0x9D)			retval=OP9D(opdata,oplen);
			else if(opcode==0xA6)			retval=OPA6(opdata,oplen);
			else if(opcode==0xA7)			retval=OPA7(opdata,oplen);
			else if(opcode==0xAA)			retval=OPAA(opdata,oplen);
			else if(opcode==0xAD)			retval=OPAD(opdata,oplen);
			else if(opcode==0xAE)			retval=OPAE(opdata,oplen);
			else if(opcode==0xFF)			retval=OPFF(opdata,oplen);
			else{
				char buffer[3];
				uString code;
				for(int i=0;i<oplen;i++){
					sprintf(buffer,"%02X",opdata[+i]);
					code+=buffer;
				}
				LogError("Invalid opcode:0x%02X:0x%s",opcode,code.c_str());
			}
		}
		else{
			LogError("Script buffer overflowed");
		}
	}
	else{
		LogError("No Script Loaded");
	}
	return retval;
}

/*! End of script file
 */
bool Truelove::OP00(Uint8 *Data,Uint32 Size){
	LogTest("OP00:EOF:%d",Size);
	return true;
}

bool Truelove::OP04(Uint8 *Data,Uint32 Size){
	Uint16 p1=GETWORD(Data+0);
	Uint16 p2=GETWORD(Data+2);
	Uint16 p3=GETWORD(Data+4);
	Uint8 p4=GETBYTE(Data+6);
	LogTest("OP04:%d:%d:%d:%d",p1,p2,p3,p4);
	return false;
}

bool Truelove::OP16(Uint8 *Data,Uint32 Size){
	char buffer[Size+1];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size]=0;
	Uint8 param=GETBYTE(Data+Size-1);
	LogTest("OP16:%d:%s:%d",Size,buffer,param);
	return false;
}

bool Truelove::OP17(Uint8 *Data,Uint32 Size){
	Uint8 param=GETBYTE(Data+Size-1);
	LogTest("OP17:%d:%d",Size,param);
	return false;
}

bool Truelove::OP19(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+Size-2);
	char buffer[Size];
	strncpy(buffer,(char*)Data+1,Size-2);
	buffer[Size-1]=0;
	LogTest("OP19:SETNAME_L:%d:%s:%d",p1,buffer,p2);
	return false;
}

bool Truelove::OP1B(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint16 p2=GETWORD(Data+1);
	LogTest("OP1B:%d:%d",p1,p2);
	return false;
}

bool Truelove::OP20(Uint8 *Data,Uint32 Size){
	LogTest("OP20:%s",Data);
	return false;
}

bool Truelove::OP23(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETWORD(Data+1);
	Uint8 p3=GETWORD(Data+2);
	LogTest("OP23:%d:%d:%d",p1,p2,p3);
	return false;
}

bool Truelove::OP24(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETWORD(Data+1);
	Uint8 p3=GETWORD(Data+2);
	LogTest("OP24:%d:%d:%d",p1,p2,p3);
	return false;
}

/* Jump to an address
 */
bool Truelove::OP28(Uint8 *Data,Uint32 Size){
	Uint8 p=GETWORD(Data+0);
	Jump(p);
	return false;
}

/* Call an address
 */
bool Truelove::OP2B(Uint8 *Data,Uint32 Size){
	Uint8 p=GETWORD(Data+0);
	Call(p);
	return false;
}

bool Truelove::OP30(Uint8 *Data,Uint32 Size){
	LogTest("OP30:%d",Size);
	return false;
}

bool Truelove::OP31(Uint8 *Data,Uint32 Size){
	LogTest("OP31:%d",Size);
	return false;
}

bool Truelove::OP32(Uint8 *Data,Uint32 Size){
	LogTest("OP32:%d",Size);
	return false;
}

/*! Loads an image
 */
bool Truelove::OP33(Uint8 *Data,Uint32 Size){
	char buffer[Size+1];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size]=0;
	Uint8 param=GETBYTE(Data+Size-1);
	LogVerbose("Loading image %s to buffer %d",buffer,param);
	if(param<TLBUFFER_SIZE){
		SDL_Surface *tmp=LoadImage(buffer);
		if(tmp){
			EDL_BlitSurface(tmp,0,cache[param],0);
			SDL_FreeSurface(tmp);
		}
		else{
			LogError("Failed to load image: %s",buffer);
		}
	}
	return false;
}

bool Truelove::OP34(Uint8 *Data,Uint32 Size){
	LogTest("OP34:%d",Size);
	return false;
}

bool Truelove::OP35(Uint8 *Data,Uint32 Size){
	// Update screen/timer?
	LogTest("OP35:%d",Size);
	return false;
}

/*! Blit image
 */
bool Truelove::OP36(Uint8 *Data,Uint32 Size){
	//Uint8 time=GETBYTE(Data+0);
	//Uint8 key=GETBYTE(Data+1);
	Uint8 srci=GETBYTE(Data+2);
	Uint16 srcx=GETWORD(Data+3);
	Uint16 srcy=GETWORD(Data+5);
	Uint16 srcw=GETWORD(Data+7);
	Uint16 srch=GETWORD(Data+9);
	Uint8 dsti=GETBYTE(Data+11);
	Uint16 dstx=GETWORD(Data+12);
	Uint16 dsty=GETWORD(Data+14);
	//Uint16 p1=GETWORD(Data+16);
	//Uint16 p2=GETWORD(Data+18);
	//Uint16 p3=GETWORD(Data+20);
	//LogTest("OP36:BLIT:%d (%dx%dx%dx%d) -> %d (%dx%d)",
	//		srci,srcx,srcy,srcw,srch,dsti,dstx,dsty);
	if(srci<TLBUFFER_SIZE && dsti<TLBUFFER_SIZE){
		SDL_Rect srcr={srcx,srcy,srcw,srch};
		SDL_Rect dstr={dstx,dsty,srcw,srch};
LogTest("%dx%dx%dx%d     %dx%dx%dx%d",srcx,srcy,srcw,srch,dstx,dsty,srcw,srch);
		EDL_BlitSurface(cache[srci],&srcr,cache[dsti],&dstr);
		if(dsti==0){
			display->Blit(cache[0]);
		}
	}
	return (dsti==0);
}

/*! Loads an animation
 */
bool Truelove::OP38(Uint8 *Data,Uint32 Size){
	char buffer[Size+1];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size]=0;
	Uint8 param=GETBYTE(Data+Size-1);
	LogTest("OP38:LOAD:ANIMATION:%s:%d",buffer,param);
	return false;
}

bool Truelove::OP39(Uint8 *Data,Uint32 Size){
	LogTest("OP39:%d",Size);
	return false;
}

/*! Fill rect
 */
bool Truelove::OP3A(Uint8 *Data,Uint32 Size){
	Uint16 p=GETWORD(Data+0);
	Uint16 x=GETWORD(Data+2);
	Uint16 y=GETWORD(Data+4);
	Uint16 w=GETWORD(Data+6);
	Uint16 h=GETWORD(Data+8);
	Uint32 c=0xFF00FFFF;
	if(p==0)	c=0x000000FF;
	else{
		LogTest("OP3A:FILL:%dx%dx%dx%d=%04X",x,y,w,h,p);
	}
	SDL_Rect r={x,y,w,h};
	display->Fill(c,&r);
	return false;
}

bool Truelove::OP3C(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint16 p2=GETWORD(Data+1);
	Uint16 p3=GETWORD(Data+3);
	LogTest("OP3C:%d:%d:%d:%d",Size,p1,p2,p3);
	return false;
}

/* Hexrays: 40BEF0
 * Original calls 409D90 and 4054B0
 *
 * 4054B0
 *    return a3 & var[1][a2])!=0
 */
bool Truelove::OP40(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	Uint16 p3=GETWORD(Data+2);
	if((p2 & vars[1].GetUint8(p1))!=0){
		Jump(p3);
	}
	return false;
}

/* Conditional jump
 *
 * Hexrays: 40BF70
 * Original calls 409D90 and 4054B0
 */
bool Truelove::OP41(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	Uint16 p3=GETWORD(Data+2);
	if((p2 & vars[0].GetUint8(p1))!=0){
		Jump(p3);
	}
	return false;
}

/* Hexrays: 40BFF0
 * Original calls 409D90 and 4054D0
 *
 */
bool Truelove::OP42(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint16 p2=GETWORD(Data+1);
	LogTest("OP42:%d:%d",p1,p2);
	return false;
}

/* Hexrays: 40C0A0
 * Original calls 409D90 and 4052E0
 *
 */
bool Truelove::OP43(Uint8 *Data,Uint32 Size){
	if(Size==5){
		Uint8 flag=GETBYTE(Data+0);		// Decode with 4052E0
		Uint8 op=GETBYTE(Data+1);
		Uint8 i=GETWORD(Data+2);
		Uint16 label=GETWORD(Data+4);
		bool result=false;
		switch(op){
			case 0:		result=vars[2].GetUint8(flag)<=i;	break;
			case 1:		result=vars[2].GetUint8(flag)==i;	break;
			case 2:		result=vars[2].GetUint8(flag)>=i;	break;
			default:	result=vars[2].GetUint8(flag)>=i;	break;
		}
		if(result){
			LogError("OUCH ... Should have jumped here");
			//Jump(label);
		}
	}
	else{
		LogError("Invalid OP43 length: %d",Size);
	}
	return false;
}

/* Conditional jump
 *
 * Hexrays: 40C170
 * Original calls 409D90 and 40EEE0
 *
 * 409D90(this,label) seems to be a jump
 */
bool Truelove::OP44(Uint8 *Data,Uint32 Size){
	if(Size==6){
		Uint8 flag=GETBYTE(Data+0);		// Decode with 40EEE0
		Uint8 op=GETBYTE(Data+1);
		Uint16 i=GETWORD(Data+2);
		Uint16 label=GETWORD(Data+4);
		bool result=false;
		switch(op){
			case 0:		result=vars[3].GetUint16(flag)<=i;	break;
			case 1:		result=vars[3].GetUint16(flag)==i;	break;
			case 2:		result=vars[3].GetUint16(flag)>=i;	break;
			default:	result=vars[3].GetUint16(flag)>=i;	break;
		}
		if(result){
			Jump(label);
		}
	}
	else{
		LogError("Invalid OP44 length: %d",Size);
	}
	return false;
}

/* Hexrays: 40C2D0
 * Calls 405410(this+8,p1,p2,1)) (Very similar to OP49) (vars[1])
 */
bool Truelove::OP48(Uint8 *Data,Uint32 Size){
	Sint8 p1=GETBYTE(Data+0);
	Sint8 p2=GETBYTE(Data+1);
	int v=vars[1].GetUint32(1);
	vars[1].SetUint32(p1,v|p1);
	return false;
}


/* Hexrays: 40C310
 * Calls 405410(this+4,p1,p2,p3)) (Very similar to OP48) (vars[0])
 */
bool Truelove::OP49(Uint8 *Data,Uint32 Size){
	Sint8 p1=GETBYTE(Data+0);
	Sint8 p2=GETBYTE(Data+1);
	Uint8 p3=GETBYTE(Data+2);
	int v=vars[1].GetUint32(p3);
	if(p3){
		vars[0].SetUint32(p1,v|p2);
	}
	else{
		vars[0].SetUint32(p1,v&(0xFFFFFF00|~p2));
	}
	return false;
}

bool Truelove::OP4A(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Sint8 p2=GETBYTE(Data+1);

	LogTest("OP4A:%d:%d:%d",Size,p1,p2);
	return false;
}

/* Hexrays: 40C420
 * Uses 4052F0 to add value, 4052A0 to set it
 */
bool Truelove::OP4B(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Sint8 p2=GETBYTE(Data+1);
	if(p2){
		Sint8 v=vars[2].GetUint8(p1);
		vars[2].SetUint8(p1,p2+v);
	}
	else{
		vars[2].SetUint8(p1,0);
	}
	LogTest("OP4B:%d:%d:%d",Size,p1,p2);
	return false;
}

/* Hexrays: 40C4A0
 * Uses 40EED0 to add value, 40EE80 to set it
 */
bool Truelove::OP4C(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Sint16 p2=GETWORD(Data+1);
	if(p2){
		Sint16 v=vars[3].GetUint16(p1);
		vars[3].SetUint16(p1,p2+v);
	}
	else{
		vars[3].SetUint16(p1,0);
	}
	LogTest("OP4C:%d:%d:%d",Size,p1,p2);
	return false;
}

/*! Load script
 */
bool Truelove::OP52(Uint8 *Data,Uint32 Size){
	char buffer[Size+1];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size]=0;
	//Uint8 param=GETBYTE(Data+Size-1);
	LogTest("OP52:LOAD:%s",buffer);
	LoadTrueloveScript(buffer);
	return false;
}

/*! Play animation
 */
bool Truelove::OP53(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	Uint8 p3=GETBYTE(Data+2);
	LogTest("OP53:PLAY:ANIMATION:%d:%d:%d",p1,p2,p3);
	return false;
}

bool Truelove::OP54(Uint8 *Data,Uint32 Size){
	Uint16 p1=GETWORD(Data+0);
	Uint8 p2=GETBYTE(Data+2);
	Uint16 p3=GETWORD(Data+3);
	LogTest("OP54:%d:%d:%d",p1,p2,p3);
	return false;
}

/*! Play music
 */
bool Truelove::OP61(Uint8 *Data,Uint32 Size){
	char buffer[Size];
	strncpy(buffer,(char*)Data,Size-2);
	buffer[Size-1]=0;
	//Uint16 loop=GETWORD(Data+Size-2);
	PlayMusic(buffer);
	return false;
}

bool Truelove::OP62(Uint8 *Data,Uint32 Size){
	LogTest("OP62:%d",Size);
	return false;
}

/*! Stop music
 */
bool Truelove::OP63(Uint8 *Data,Uint32 Size){
	StopMusic();
	return false;
}

/*! Play sound
 */
bool Truelove::OP66(Uint8 *Data,Uint32 Size){
	char buffer[Size];
	strncpy(buffer,(char*)Data,Size-2);
	buffer[Size-1]=0;
	LogTest("OP66:PLAY:SE:%s",buffer);
	return false;
}

bool Truelove::OP67(Uint8 *Data,Uint32 Size){
	LogTest("OP67:%d",Size);
	return false;
}

/*! Dialog text
 */
bool Truelove::OP70(Uint8 *Data,Uint32 Size){
	// Parse parameters
	int start=3;
	//Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	//Uint8 p3=GETBYTE(Data+2);
	Uint8 p4=0;
	Uint8 p5=0;
	Uint8 p6=0;
	if(p2!=0xFF){
		p4=GETBYTE(Data+3);
		p5=GETBYTE(Data+4);
		p6=GETBYTE(Data+5);
		start=6;
	}

	uString text;
	if(Size-start>0){
		char buffer[(Size-start)+1];
		strncpy(buffer,(char*)Data+start,Size-start);
		buffer[Size-start]=0;
		text=buffer;
	}
	textview->PrintText(text);
	return false;
}

/*! Print text
 */
bool Truelove::OP71(Uint8 *Data,Uint32 Size){
	// Parse parameters
	Uint16 x=GETWORD(Data+0);
	Uint16 y=GETWORD(Data+2);
	Uint8 c=GETBYTE(Data+4);
	uString text;
	if(Size-5>0){
		char buffer[(Size-5)+1];
		strncpy(buffer,(char*)Data+5,Size-5);
		buffer[Size-5]=0;
		text=buffer;
	}
	return false;
}

bool Truelove::OP72(Uint8 *Data,Uint32 Size){
	Uint8 p=GETBYTE(Data+0);
	LogTest("OP72:%d",Size);
	return false;
}

bool Truelove::OP73(Uint8 *Data,Uint32 Size){
	Uint8 p=GETBYTE(Data+0);
	LogTest("OP73:Open font %d",p);
	return false;
}

bool Truelove::OP75(Uint8 *Data,Uint32 Size){
	Sint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	Uint8 p3=GETBYTE(Data+2);
	LogTest("OP75:%d:%d:%d",p1,p2,p3);
	return false;
}

bool Truelove::OP82(Uint8 *Data,Uint32 Size){
	Uint16 p1=GETWORD(Data+0);
	Uint16 p2=GETWORD(Data+2);
	Uint16 p3=GETWORD(Data+4);
	Uint16 p4=GETWORD(Data+6);
	Uint8 p5=GETBYTE(Data+8);
	LogTest("OP82:%d:%d:%d:%d:%d",p1,p2,p3,p4,p5);
	return false;
}

bool Truelove::OP83(Uint8 *Data,Uint32 Size){
	Uint16 p=GETWORD(Data+0);
	LogTest("OP83:%d",p);
	return false;
}

bool Truelove::OP84(Uint8 *Data,Uint32 Size){
	char buffer[Size];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size-1]=0;
	Uint8 p=GETBYTE(Data+Size-1);
	LogTest("OP84:INTERFACE:%s:%d",buffer,p);
	return false;
}

bool Truelove::OP85(Uint8 *Data,Uint32 Size){
	Sint8 p1=GETWORD(Data+0);
	Uint8 p2=GETWORD(Data+1);
	LogTest("OP85:INTERFACE:%d %d (%d)",p1,p2,Size);
	return false;
}

bool Truelove::OP86(Uint8 *Data,Uint32 Size){
	Uint16 p1=GETWORD(Data+0);
	Uint16 p2=GETWORD(Data+2);
	Sint8 p3=GETWORD(Data+4);
	Uint8 p4=GETWORD(Data+5);
	LogTest("OP86:INTERFACE:%d %d %d %d",p1,p2,p3,p4);
	return false;
}

bool Truelove::OP87(Uint8 *Data,Uint32 Size){
	char buffer[Size];
	strncpy(buffer,(char*)Data,Size-1);
	buffer[Size-1]=0;
	Uint8 p=GETBYTE(Data+Size-1);
	LogTest("OP87:INTERFACE:%s:%d",buffer,p);
	return false;
}

/*! Delay
 */
bool Truelove::OP89(Uint8 *Data,Uint32 Size){
	Uint16 delay=GETWORD(Data+0);
	SDL_Delay(delay);
	return false;
}

/*! Update screen
 */
bool Truelove::OP8A(Uint8 *Data,Uint32 Size){
	return true;
}

/*! Return from call
 */
bool Truelove::OP91(Uint8 *Data,Uint32 Size){
	Return();
	return false;
}

bool Truelove::OP92(Uint8 *Data,Uint32 Size){
	LogTest("OP92:%d",Size);
	return false;
}

/*! Set range of flags
 *
 *  Hexrays: 40D960
 *
 *  Type 0 => 405440(this+4,int)	=> 4053D0
 *  Type 1 => 405440(this+8,int)	=> 4053D0
 *  Type 2 => 4052B0(this+12,byte)
 *  Type 3 => 40EE90(this+16,word)
 */
bool Truelove::OP95(Uint8 *Data,Uint32 Size){
	Uint8 type=GETBYTE(Data+0);
	Uint16 flag=GETWORD(Data+1);
	Uint16 count=GETWORD(Data+3);
	Uint8 value=GETBYTE(Data+5);
	for(int i=flag;i<flag+count;i++){
		if(type==0)	vars[type].SetUint32(i,value);
		if(type==1)	vars[type].SetUint32(i,value);
		if(type==2)	vars[type].SetUint8(i,value);
		if(type==3)	vars[type].SetUint16(i,value);
	}
	return false;
}

/*! Sets a flag
 */
bool Truelove::OP98(Uint8 *Data,Uint32 Size){
	/*
	Uint16 flag=GETWORD(Data+0);

	Uint16 v1=flag&0x7FFF;
	Uint8 v9;
	int v8;

	Uint8 op=GETBYTE(Data+i);
	if(op!=4){
		do{
			v9=GETBYTE(Data
		}
	}

		int pos=0;
		Uint16 z=0;
		Uint16 l=0;
		Uint16 r=0;


		*/


	Uint16 flag=GETWORD(Data+0);
	if(flag & 0x8000){
		int pos=0;
		Uint16 z=0;
		Uint16 l=0;
		Uint16 r=0;
		for(unsigned int i=2;i<Size;i++){
			Uint8 op=GETBYTE(Data+i);
			if(op==0x04){
				break;
			}
			if(op==0x08){
				op=0;
				z=GETWORD(Data+i+1);
				i+=2;
			}
			else{
				z=GETWORD(Data+i+1);
				if((z&0xC000)==0xC000){
					z=rand()%(z&0x3FFF);
				}
				else if(z&0x8000){
					z=vars[3].GetUint16(z&0x7FFF);
				}
				i+=2;
			}

			if((op<0x00) || (op>0x03)){
				LogError("Unknown flag operation: %d",op);
				break;
			}

			if(pos++ & 2){
				switch(op){
					case 0:		r=z;		break;
					case 1:		r-=z;		break;
					case 2:		r*=z;		break;
					case 3:		if(z) r/=z;	break;
				}
			}
			else{
				switch(op){
					case 0:		l+=z;			break;
					case 1:		l-=z;			break;
					case 2:		l+=r*z;			break;
					case 3:		if(z) l+=r/z;	break;
				}
			}
		}
		vars[3].SetUint16(flag&0x7FFF,l+r);
		LogTest("SET FLAG[%d]=%d",flag&0x7FFF,l+r);
	}
	else{
		LogError("Variable not in FLAG_SET bank: 0x%04X",flag);
	}
	return false;
}

bool Truelove::OP99(Uint8 *Data,Uint32 Size){
	LogTest("OP99:SET FLAG RELATED:%d",Size);
	return false;
}

bool Truelove::OP9D(Uint8 *Data,Uint32 Size){
	Uint8 p1=GETBYTE(Data+0);
	Uint8 p2=GETBYTE(Data+1);
	LogTest("OP9D:%d:%d",Size,p1,p2);
	if(p2){
		// Update animation or sprite index p1?
	}
	else{
		// Flush anumation/sprite
	}
	return false;
}

/*! Wait for user input
 */
bool Truelove::OPA6(Uint8 *Data,Uint32 Size){
	Uint16 p1=GETWORD(Data+0);
	Uint16 p2=GETWORD(Data+2);
	if(p1!=1 || p2!=1){
		LogTest("OPA6:WAIT:%d:%d",p1,p2);
	}
	state=TLSTATE_WAITCLICK;
	return true;
}

/*! Detect mouse clicks
 */
bool Truelove::OPA7(Uint8 *Data,Uint32 Size){
	Uint16 x1=GETWORD(Data+0);
	Uint16 y1=GETWORD(Data+2);
	Uint16 x2=GETWORD(Data+4);
	Uint16 y2=GETWORD(Data+6);
	Uint16 label=GETWORD(Data+8);

	/*
	if(MOUSECLICK){
		JUMP(LABEL);
	}
	*/

	LogTest("OPA7:CLICKDETECT:%dx%dx%dx%d:%d",x1,y1,x2,y2,label);
	return false;
}

bool Truelove::OPAA(Uint8 *Data,Uint32 Size){
	Uint16 x1=GETWORD(Data+0);
	Uint16 y1=GETWORD(Data+2);
	Uint16 x2=GETWORD(Data+4);
	Uint16 y2=GETWORD(Data+6);

	LogTest("OPA7:%dx%dx%dx%d",x1,y1,x2,y2);
	return false;
}

/*! Invalidates on keyboard/mouse ... ?
 */
bool Truelove::OPAD(Uint8 *Data,Uint32 Size){
	/* Original processes peripherals here and store states in registers

	Uint16 left=GETWORD(Data+0);
	Uint16 right=GETWORD(Data+2);
	Uint16 miss=GETWORD(Data+4);	// Decodes with 409DD0
	Uint8 count=GETBYTE(Data+6);
	*/
	return false;
}

/*! Detect mouse events
 */
bool Truelove::OPAE(Uint8 *Data,Uint32 Size){
	Uint16 x1=GETWORD(Data+0);
	Uint16 y1=GETWORD(Data+2);
	Uint16 x2=GETWORD(Data+4);
	Uint16 y2=GETWORD(Data+6);
	Uint16 label=GETWORD(Data+8);
	//Uint8 type=GETWORD(Data+10);
	//Uint16 flag=GETWORD(Data+11);

	// We access system values directly
	int mx=mousex;
	int my=mousey-display->GetY();
	if(mx>=x1 && mx<x2 && my>=y1 && my<y2){
		/* Original gets system values from script

		int v=0;
		if(type==0)	v=vars[type].GetUint32(flag);
		if(type==1)	v=vars[type].GetUint32(flag);
		if(type==2)	v=vars[type].GetUint8(flag);
		if(type==3)	v=vars[type].GetUint16(flag);
		if(v){
			Jump(label);
		}
		*/

		if(keyok){
			Jump(label);
			keyok=false;
		}
	}
	return false;
}

bool Truelove::OPFF(Uint8 *Data,Uint32 Size){
	LogTest("OPFF:GAME_END:%d",Size);
	return true;
}



