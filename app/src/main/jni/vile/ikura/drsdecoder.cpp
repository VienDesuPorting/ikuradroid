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

#include "drsdecoder.h"

DRSDecoder::DRSDecoder(int Width,int Height) : IkuraDecoder(Width,Height){
	w_display->SetTarget(1);
	cdda_track=-1;
	cdda_failed=false;
	ig_lastpos=0x7FFFFFFF;
	ig_lastflag=0x7FFFFFFF;
}

DRSDecoder::~DRSDecoder(){
}

// Opcode trace flood control: the interpreter streams one trace line
// per executed opcode, and steady-state script loops (the title
// screen poll) would flood logcat with the same short cycle forever.
// Each gate id reports at most once per window while brand new
// opcodes still show up immediately.  Ids 0-255 cover the dispatcher
// trace, higher ids belong to handlers that log on their own.
static bool TraceGate(int Id){
	static Uint32 last[512];
	static bool init=false;
	Uint32 now=SDL_GetTicks();
	if(!init){
		for(int i=0;i<512;i++){
			last[i]=now-10000;
		}
		init=true;
	}
	if(now-last[Id]<10000){
		return false;
	}
	last[Id]=now;
	return true;
}

/*! \brief Advances the scripted engine
 *  \returns True if the engine is waiting for user input
 */
bool DRSDecoder::EventGameProcess(){
	Uint8 c;
	Uint32 l;
	const Uint8 *b;
	bool r=true;
	if(parser.GetOpcode(&c,&b,&l)){
		// DRS specifies length off by one
		if(l){
			l--;
		}
		if(TraceGate(c)){
			LogVerbose("OPCODE %02X (%d bytes)",c,l);
		}

		// Map opcode between ikura and drs handlers
		if(c==0x00)					r=iop_ed(b,l);
		else if(c==0x01)			r=iop_ls(b,l);
		else if(c==0x02)			r=OP02(b,l);
		else if(c==0x03)			r=OP03(b,l);
		else if(c==0x04)			r=iop_jp(b,l);
		else if(c==0x05)			r=OP05(b,l);
		else if(c==0x06)			r=OP06(b,l);
		else if(c==0x07)			r=OP07(b,l);
		else if(c==0x08)			r=OP08(b,l);
		else if(c==0x09)			r=iop_setgameinfo(b,l);
		//else if(c==0x0A)			r=OP0A(b,l);
		//else if(c==0x0B)			r=OP0B(b,l);
		else if(c==0x0C)			r=OP0C(b,l);
		else if(c==0x0D)			r=OP0D(b,l);
		else if(c==0x0E)			r=OP0E(b,l);
		else if(c==0x10)			r=iop_cw(b,l);
		else if(c==0x11)			r=OP11(b,l);
		//else if(c==0x12)			r=OP12(b,l);
		//else if(c==0x13)			r=OP13(b,l);
		else if(c==0x14)			r=iop_cset(b,l);
		else if(c==0x15)			r=OP15(b,l);
		else if(c==0x16)			r=iop_cwc(b,l);
		else if(c==0x17)			r=iop_cc(b,l);
		else if(c==0x20)			r=OP20(b,l);
		else if(c==0x21)			r=OP21(b,l);
		//else if(c==0x22)			r=OP22(b,l);
		else if(c==0x28)			r=iop_wo(b,l);
		else if(c==0x29)			r=iop_wc(b,l);
		else if(c==0x2A)			r=OP2A(b,l);
		else if(c==0x2B)			r=OP2B(b,l);
		else if(c==0x2C)			r=OP2C(b,l);
		//else if(c==0x30)			r=iop_fln(b,l);
		else if(c==0x31)			r=iop_sk(b,l);
		else if(c==0x32)			r=iop_sks(b,l);
		else if(c==0x34)			r=iop_hf(b,l);
		else if(c==0x35)			r=iop_sp(b,l);
		else if(c==0x36)			r=iop_pf(b,l);
		else if(c==0x37)			r=iop_sts(b,l);
		else if(c==0x39)			r=iop_es(b,l);
		else if(c==0x3A)			r=iop_ec(b,l);
		else if(c==0x3C)			r=OP3C(b,l);
		else if(c==0x3D)			r=OP3D(b,l);
		//else if(c==0x40)			r=iop_hln(b,l);
		else if(c==0x41)			r=iop_hs(b,l);
		else if(c==0x42)			r=OP42(b,l);
		else if(c==0x43)			r=iop_hinc(b,l);
		else if(c==0x44)			r=iop_hdec(b,l);
		else if(c==0x46)			r=OP46(b,l);
		else if(c==0x47)			r=OP47(b,l);
		else if(c==0x50)			r=iop_vset(b,l);
		else if(c==0x51)			r=iop_gn(b,l);
		else if(c==0x53)			r=iop_gc(b,l);
		else if(c==0x54)			r=OP54(b,l);
		else if(c==0x55)			r=OP55(b,l);
		else if(c==0x56)			r=iop_gp(b,l);
		else if(c==0x76)			r=iop_ser(b,l);
		else if(c==0x77)			r=OP77(b,l);
		else if(c==0x78)			r=iop_set(b,l);
		else if(c==0x79)			r=OP79(b,l);
		else if(c==0x7A)			r=iop_dap(b,l);
		else if(c==0x7B)			r=iop_das(b,l);
		else if(c==0x80)			r=iop_im(b,l);
		else if(c==0x81)			r=iop_ic(b,l);
		else if(c==0x84)			r=iop_ih(b,l);
		else if(c==0x85)			r=OP85(b,l);
		else if(c==0xF1)			r=iop_atimes(b,l);
		else if(c==0xF2)			r=iop_await(b,l);
		else if(c==0xF3)			r=OPF3(b,l);
		else if(c==0xF4)			r=iop_ppf(b,l);
		else if(c==0xF5)			r=iop_svf(b,l);
		else if(c==0xF6)			r=iop_opsl(b,l);
		else if(c==0xF8)			r=OPF8(b,l);
		else if(c==0xF9)			r=OPF9(b,l);
		else if(c==0xFA)			r=OPFA(b,l);
		else						r=iop_unknown(c,b,l);
	}
	return r;
}

// Load script and CALL it (IOP_LSBS)
// An extra word before the script name compared to ikura
bool DRSDecoder::OP02(const Uint8 *Data,int Length){
	RWops *blob=0;
	if((blob=LoadScript((char*)Data+2,"ISF"))){
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				DecodeScript(tbuffer,tlength);
				parser.CallScript((char*)Data+2,tbuffer,tlength);
			}
			delete [] tbuffer;
		}
		delete blob;
	}
	else{
		LogError("Failed to load script %s",(char*)Data+2);
	}
	return false;
}

// Return from call (IOP_SRET)
bool DRSDecoder::OP03(const Uint8 *Data,int Length){
	parser.ReturnScript();
	return false;
}

// Call subroutine ssoffset+jumptable[WORD] (IOP_JS)
// An extra word before the script address
bool DRSDecoder::OP05(const Uint8 *Data,int Length){
	Uint16 tableentry=GETWORD(Data+2);
	parser.CallFunction(tableentry);
	return false;
}

// Return from subroute (IOP_RT)
bool DRSDecoder::OP06(const Uint8 *Data,int Length){
	parser.ReturnFunction();
	return false;
}

// Jumpswitch (IOP_ONJP)
//
// The DRS layout widened the ISF byte-sized count to a word, which also
// shifts the entry table: [dword register][word count][word entries],
// so the entries start at offset 6. The ISF variant reads from offset 5
// because its count is a single byte. Reading from 5 here shifted every
// table index into its high byte (index*256), the jump landed far outside
// the jumptable, and the Kana title menu loop derailed into the script
// start on every dispatcher pass - skipping its input polling entirely.
bool DRSDecoder::OP07(const Uint8 *Data,int Length){
	int reg=parser.DecodeValue(GETDWORD(Data));
	Uint16 cnt=GETWORD(Data+4);
	// Diagnostics for the title screen dispatch: the register
	// polled and the entry chosen are the ground truth of every
	// menu loop.  Steady menu loops repeat the same dispatch, so
	// only transitions get logged
	static int onjp_reg=-1;
	static int onjp_cnt=-1;
	if(reg!=onjp_reg || cnt!=onjp_cnt){
		onjp_reg=reg;
		onjp_cnt=cnt;
		LogTest("ONJP: reg=%d cnt=%d",reg,cnt);
	}
	if(reg>=(int)0 && reg<cnt){
		for(int i=6;i<Length;i+=2){
			if(!reg){
				int target=GETWORD(Data+i);
				static int onjp_target=-1;
				if(target!=onjp_target){
					onjp_target=target;
					LogTest("ONJP: reg switch -> entry %d",target);
				}
				parser.JumpFunction(target);
				break;
			}
			else{
				reg--;
			}
		}
	}
	else{
		LogTest("ONJP: reg=%d out of range (cnt=%d)",reg,cnt);
	}
	return false;
}

// Callswitch (IOP_ONJS)
//
// The DRS layout prepends an extra word and widens the count to a word:
// [word extra][dword register][word count][word entries], so the entries
// start at offset 8 (the ISF variant reads from 5 because its header is
// only five bytes wide).
bool DRSDecoder::OP08(const Uint8 *Data,int Length){
	int reg=parser.DecodeValue(GETDWORD(Data+2));
	Uint16 cnt=GETWORD(Data+6);
	if(reg>=(int)0 && reg<cnt){
		for(int i=8;i<Length;i+=2){
			if(!reg){
				LogTest("ONJS: reg switch -> entry %d",GETWORD(Data+i));
				parser.CallFunction(GETWORD(Data+i));
				break;
			}
			else{
				reg--;
			}
		}
	}
	return false;
}

bool DRSDecoder::OP0C(const Uint8 *Data,int Length){
	LogVerbose("OP0C:KID:%d",Length);
	return false;
}

bool DRSDecoder::OP0D(const Uint8 *Data,int Length){
	LogVerbose("OP0C:KID:%d",Length);
	return false;
}

bool DRSDecoder::OP0E(const Uint8 *Data,int Length){
	LogVerbose("OP0C:FLAG:SYSEX:%d",Length);
	return false;
}

// Set font size (IOP_PB)
// No window parameter
bool DRSDecoder::OP11(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_PB,Data,Length)){
		int size=parser.DecodeValue(GETDWORD(Data));
		w_textview->SetFontSize(size);
	}
	return false;
}

// Open command screen (IOP_CWO)
bool DRSDecoder::OP15(const Uint8 *Data,int Length){
	// Decode parameters
	//int par=parser.DecodeValue(GETDWORD(Data));
	if(w_select){
		w_select->SetVisible(true);
		selresult=-1;
	}
	return true;
}

// Set text window position (IOP_WS)
// Completely off
bool DRSDecoder::OP20(const Uint8 *Data,int Length){
	Uint8 dst=GETBYTE(Data);
	if(dst){
		LogError("ILLEGAL TARGET WINDOW: %d",dst);
	}
	Uint32 x=GETDWORD(Data+1);
	Uint32 y=GETDWORD(Data+5);
	Uint32 w=GETDWORD(Data+9);
	Uint32 h=GETDWORD(Data+13);
	w_textview->MoveDialog(x,y);
	if(w && h){
		w_textview->Resize(w,h);
	}
	return false;
}


// Set text window graphic (IOP_WP)
// Much simpler than Ikura
bool DRSDecoder::OP21(const Uint8 *Data,int Length){
	// Get provided text window data
	Uint8 dst=GETBYTE(Data);
	char *filename=(char*)Data+2;
	int width=parser.DecodeValue(GETDWORD(Data+Length-8));
	int height=parser.DecodeValue(GETDWORD(Data+Length-4));
	if(dst){
		LogError("ILLEGAL WINDOW: %d",dst);
	}
	else{
		SDL_Surface *tmpsurface=LoadImage(filename);
		if(tmpsurface){
			w_textview->Resize(width,height);
			w_textview->Set(tmpsurface);
			SDL_FreeSurface(tmpsurface);
		}
	}
	return false;
}

// Show text window (IOP_WSS)
bool DRSDecoder::OP2A(const Uint8 *Data,int Length){
	// Set text window dimensions
	Uint8 dst=GETBYTE(Data);
	if(dst){
		LogError("ILLEGAL WINDOW: %d",dst);
	}
	else{
		w_textview->SetVisible(true);
	}
	return false;
}

// Hide text window (IOP_WSH)
bool DRSDecoder::OP2B(const Uint8 *Data,int Length){
	// Set text window dimensions
	Uint8 dst=GETBYTE(Data);
	if(dst){
		LogError("ILLEGAL WINDOW: %d",dst);
	}
	else{
		w_textview->SetVisible(false);
	}
	return false;
}

// Print text messages (IOP_PM)
bool DRSDecoder::OP2C(const Uint8 *Data,int Length){
	// Set text window dimensions
	Uint8 dst=GETBYTE(Data);
	//Uint16 length=GETWORD(Data+1);
	if(dst){
		LogError("ILLEGAL WINDOW: %d",dst);
	}
	else{
		// Print text (Stripping unicode encoding)
		uString text;
		for(int j=3;Data[j];j++){
			if(Data[j]<0x7F){
				text+=(char)Data[j];
			}
		}
		w_textview->SetVisible(true);
		w_textview->PrintText(text.c_str());
	}
	return true;
}

bool DRSDecoder::OP3C(const Uint8 *Data,int Length){
	if(TraceGate(0x3C+256)){
		LogVerbose("OP3C:CGFLAG:%d",Length);
	}
	return false;
}

bool DRSDecoder::OP3D(const Uint8 *Data,int Length){
	if(TraceGate(0x3D+256)){
		LogVerbose("OP3D:CGFLAG:%d",Length);
	}
	return false;
}

// Set group of variables (IOP_HSG)
bool DRSDecoder::OP42(const Uint8 *Data,int Length){
	if(iop_check(8,IOP_HSG,Data,Length)){
		// Extract iteration data
		Uint16 start=GETWORD(Data);
		Uint16 end=GETWORD(Data+2);
		int value=parser.DecodeValue(GETDWORD(Data+4));

		// Set group of variables
		for(int i=start;i<end;i++){
			parser.SetValue(i,value);
		}
	}
	return false;
}

// Stacked calculation (IOP_CALC)
// The guy who designed this must be nuts.
bool DRSDecoder::OP46(const Uint8 *Data,int Length){
	Uint16 p=GETWORD(Data+0);
	Uint16 length=GETWORD(Data+2);
	Uint8 cmd=0;
	Uint8 func=0;
	int offset=4;
	int index=0;
	int values[32];
	int v1,v2;
	while(length>0){
		cmd=GETBYTE(Data+(offset++));
		length--;
		if(cmd==0){
			v1=parser.DecodeValue(GETDWORD(Data+offset));
			length-=4;
			offset+=4;
		}
		else{
			func=0xFF;
			v1=v2=0;
			if(length){
				func=GETBYTE(Data+(offset++));
				length--;
			}
			if(index){
				v2=values[--index];
			}
			if(index){
				v1=values[--index];
			}
			if(func==0)			v1+=v2;
			else if(func==1)	v1-=v2;
			else if(func==2)	v1*=v2;
			else if(func==3)	v2?v1/=v2:0;
			else if(func==4)	v2?v1%=v2:0;
		}
		if(index<32){
			values[index++]=v1;
		}
	}
	parser.SetValue(p,index?values[--index]:0);
	return false;
}

// If-then structure (IOP_IF)
bool DRSDecoder::OP47(const Uint8 *Data,int Length){
	Sint32 v1,v2;
	Uint8 cmd;
	bool flag;
	int offset=0;
	while(offset<Length){
		// Extract operands
		v1=parser.DecodeValue(GETDWORD(Data+offset+0));
		cmd=GETBYTE(Data+(offset+4));
		v2=parser.DecodeValue(GETDWORD(Data+offset+5));
		offset+=9;
		flag=false;

		// Perform operation
		if(cmd==0)		flag=v1==v2;
		else if(cmd==1)	flag=v1<v2;
		else if(cmd==2)	flag=v1<=v2;
		else if(cmd==3)	flag=v1>v2;
		else if(cmd==4)	flag=v1>=v2;
		else if(cmd==5)	flag=v1!=v2;

		// Break on false result
		if(flag==false){
			break;
		}

		// Determine what to do with the result
		cmd=GETBYTE(Data+(offset++));
		if(cmd==0x00){
			parser.JumpFunction(GETWORD(Data+offset));
			break;
		}
		if(cmd==0x01){
			return OP46(Data+offset,Length-offset);
		}
		if(cmd==0xFF){
			break;
		}
	}
	return false;
}

// Fade effect
bool DRSDecoder::OP54(const Uint8 *Data,int Length){
	Sint32 ticks=parser.DecodeValue(GETDWORD(Data+0));
	Sint32 src=parser.DecodeValue(GETDWORD(Data+4));
	Sint32 dir=parser.DecodeValue(GETDWORD(Data+8));
	if(dir){
		LogVerbose("FADEIN:%d:%d",ticks,src);
	}
	else{
		LogVerbose("FADEOUT:%d:%d",ticks,src);
	}
	return false;
}

// Load image into the GDL buffers
bool DRSDecoder::OP55(const Uint8 *Data,int Length){
	int index=parser.DecodeValue(GETDWORD(Data));
	uString name=(char*)Data+4;
	LogVerbose("Loading surface[%d]=%s",index,name.c_str());
	SDL_Surface *surface=LoadImage(name);
	if(surface){
		w_display->SetSurface(index,surface);
		SDL_FreeSurface(surface);
	}
	return false;
}

// Play indexed SE (IOP_SEP)
bool DRSDecoder::OP77(const Uint8 *Data,int Length){
	int index=parser.DecodeValue(GETDWORD(Data));
	//Uint8 cmd=GETBYTE(Data+4);
	LogVerbose("OP77:SEP:%d",index);
	return false;
}

// Play CDDA track
bool DRSDecoder::OP79(const Uint8 *Data,int Length){
	int track=parser.DecodeValue(GETDWORD(Data));
	if(track<=0){
		// Track 0 stops playback (mirrors iop_das)
		LogVerbose("OP79:CDDA:%d (stop)",track);
		StopMusic();
		nameMusic="stop";
		cdda_track=-1;
		cdda_failed=false;
		return false;
	}
	// The title screen loop re-issues OP79 every cycle: only act when
	// the track changes, the music has ended or the last lookup failed
	if(track==cdda_track && (Mix_PlayingMusic()||cdda_failed)){
		LogVerbose("OP79:CDDA:%d (latched)",track);
		return false;
	}
	LogTest("OP79:CDDA:%d",track);
	cdda_track=track;
	cdda_failed=false;
	uString name;
	if(PlayCDDAFiles(track,&name) || PlayCDDAScan(track,&name) ||
				PlayTrack(track)){
		// Record the name so savegames resume the same track
		if(name.length()){
			nameMusic=name;
		}
		return false;
	}
	// Nothing matched: latch the failure so the ~250ms loop does not
	// rescan every archive each cycle
	cdda_failed=true;
	LogError("CDDA track %d has no matching BGM file",track);
	return false;
}

// Change mouse cursor (0x81). The PC originals swap the cursor sprite
// over hotspots; a touchscreen has no cursor to change, so the operand
// is consumed and dropped. Registered so the interpreter stops
// reporting the opcode as unknown on every title loop cycle.
bool DRSDecoder::iop_ic(const Uint8 *Data,int Length){
	(void)Data;
	(void)Length;
	return false;
}

// Get input (IOP_IG)
//
// The DRS title menus poll the input from a tight script loop that
// carries no await of their own, so an idle poll must hand the rest of
// the processing budget back to the host. Without this the menu loop
// burns through the full 20 ms opcode budget of every host tick and the
// log drowns in thousands of IG lines per second. The first poll after
// a state change still returns false so the script reacts to hover
// moves and clicks within the very same tick.
bool DRSDecoder::OP85(const Uint8 *Data,int Length){
	bool retval=iop_ig(Data,Length);
	if(!retval){
		Uint16 cur=GETWORD(Data);
		Uint16 btn=GETWORD(Data+2);
		int pos=parser.GetValue(cur);
		int flag=parser.GetValue(btn);
		if(pos==ig_lastpos && flag==ig_lastflag){
			return true;
		}
		ig_lastpos=pos;
		ig_lastflag=flag;
	}
	return retval;
}

// Play video (IOP_AVIP)
bool DRSDecoder::OPF3(const Uint8 *Data,int Length){
	int srcw=parser.DecodeValue(GETDWORD(Data+0));
	int srch=parser.DecodeValue(GETDWORD(Data+4));
	int dstw=parser.DecodeValue(GETDWORD(Data+8));
	int dsth=parser.DecodeValue(GETDWORD(Data+12));
	LogVerbose("Play video: %d %d %d %d",srcw,srch,dstw,dsth);
	PlayVideo((char*)Data+16);
	return true;
}

// Save enable
bool DRSDecoder::OPF8(const Uint8 *Data,int Length){
	return false;
}

// Save disable
bool DRSDecoder::OPF9(const Uint8 *Data,int Length){
	return false;
}

// Savedata reset
bool DRSDecoder::OPFA(const Uint8 *Data,int Length){
	return false;
}




