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

#include "drsdecoder.h"

DRSDecoder::DRSDecoder(int Width,int Height) : IkuraDecoder(Width,Height){
	w_display->SetTarget(1);
	cdda_track=-1;
	cdda_failed=false;
	ig_lastpos=0x7FFFFFFF;
	ig_lastflag=0x7FFFFFFF;
	page_has_text=false;
	// Text pacing, reversed from KANAUS.EXE: the 0x2C stream prints
	// one character every cfg[0x212]+1 engine ticks (the countdown at
	// 0x41a300 reloads 2 and fires on 0), and the tick is the 60Hz
	// idle gate (0x40d2a0, one tick per 0x11 ms). UserSetup byte 12
	// (registry HKCU\Software\DO\KANAUS) pins cfg[0x212]=2, the EXE
	// default is 2 as well: 3 ticks = 50ms per character. The 25ms
	// engine default typed the text twice as fast as the PC original.
	SetMessageDelayEnabled(true);
	SetMessageDelayInterval(50);
	for(int i=0;i<6;i++){
		cursors[i].sheet=0;
		cursors[i].fw=0;
		cursors[i].fh=0;
		cursors[i].follow=0;
		cursors[i].x=0;
		cursors[i].y=0;
		cursors[i].delay=8;
	}
	cursor_active=-1;
	textqueue_len=0;
	textqueue_idx=0;
}

DRSDecoder::~DRSDecoder(){
	for(int i=0;i<6;i++){
		if(cursors[i].sheet){
			SDL_FreeSurface(cursors[i].sheet);
			cursors[i].sheet=0;
		}
	}
}

/*! \brief DRS wait state machine
 *
 *  DRS text works different from Ikura: OP2C types out single lines that
 *  accumulate inside the current page, and OP0D is the page break which
 *  pauses the interpreter until the player dismisses the page (reversed
 *  from KANAUS.EXE: the text opcode stays pending until the text layer
 *  releases it, and 0x0D only occurs in story scripts after text blocks).
 *  keyok is always consumed here: the base class leaves it set in the
 *  IS_WAITCLICK branch, which lets one stale click advance several pages.
 */
bool DRSDecoder::EventGameTick(){
	if(state==IS_WAITTIMER){
		if(timerend<SDL_GetTicks()){
			// Timed state has ended
			while(SkipAnimation());
			state=IS_NORMAL;
		}
		else if(keyctrl()){
			// Skip state and fall back
			w_textview->CompleteText();
			while(SkipAnimation());
			state=IS_NORMAL;
		}
	}
	if(state==IS_WAITTEXT){
		if(!w_textview->GetRemainingText()){
			// Line printed; run the remaining 0x2C control tail (newline,
			// page wait, clear) before the interpreter moves on
			state=IS_NORMAL;
			if(RunTextQueue()){
				return true;
			}
		}
		else if(keyctrl()){
			// Skip state and fall back
			w_textview->CompleteText();
			while(SkipAnimation());
			SkipTextQueue();
			state=IS_NORMAL;
		}
		else if(keyok){
			// Complete printing text
			w_textview->CompleteText();
			keyok=false;
		}
	}
	if(state==IS_WAITCLICK){
		if(keyctrl()){
			// Skip state and fall back; the drained control stream
			// clears the page wherever it says so (ctl03)
			w_textview->CompleteText();
			while(SkipAnimation());
			SkipTextQueue();
			state=IS_NORMAL;
			keyok=false;
		}
		else if(keyok){
			// Dismiss the waited line/page and resume the control
			// stream; the stream itself owns the page clear (ctl03),
			// so a line wait (ctl02) releases without wiping the
			// page; the cursor hides until the next wait (0x41a000)
			w_textview->CompleteText();
			while(SkipAnimation());
			ShowTextCursor(-1,false);
			state=IS_NORMAL;
			keyok=false;
			if(RunTextQueue()){
				return true;
			}
		}
	}

	return !(state==IS_NORMAL);
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
		else if(c==0x01){
			// Script switch: hotspots of the previous screen (title
			// menu buttons, full-screen catch-alls) must not eat the
			// next scene's background taps - a swallowed tap never
			// reaches keyok, which used to freeze EV11 forever on
			// its first page break. Every DRS screen redefines its
			// spots right before its own IG polls.
			w_display->Clear();
			r=iop_ls(b,l);
		}
		else if(c==0x02){
			w_display->Clear();
			r=OP02(b,l);
		}
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
		else if(c==0x13)			r=OP13(b,l);
		else if(c==0x14)			r=iop_cset(b,l);
		else if(c==0x15)			r=OP15(b,l);
		else if(c==0x16)			r=iop_cwc(b,l);
		else if(c==0x17)			r=iop_cc(b,l);
		else if(c==0x20)			r=OP20(b,l);
		else if(c==0x21)			r=OP21(b,l);
		else if(c==0x22)			r=OP22(b,l);
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

// Page break between text blocks. The original engine keeps this opcode
// pending until the player dismisses the page: lines of the block flow
// into the window automatically, but the transition to the next block
// waits for a click. 0x0D never occurs in system scripts (START, TITLE,
// CG_MODE), only in story scripts right after a finished text block.
// Page break marker (0x0D)
//
// The DRS dialect uses 0x0D purely as a separator between text
// blocks; the original VM never blocks or clears on it. The page
// pause and the clear live inside the 0x2C control tail (11 = wait
// for the dismissing click, 03 = clear the page), so this handler
// only needs to consume the opcode.
bool DRSDecoder::OP0D(const Uint8 *Data,int Length){
	LogVerbose("OP0D:pagebreak:%d",Length);
	return false;
}

bool DRSDecoder::OP0E(const Uint8 *Data,int Length){
	LogVerbose("OP0C:FLAG:SYSEX:%d",Length);
	return false;
}

// Set font size (IOP_PB)
//
// The DRS rows advance by fontsize+8 (reversed: the newline handler
// 0x41b4d0 walks the pen by [fontsize]+8), so font 24 paces the window
// in 32px steps - 13 rows over the 416px print area, matching the
// reference screenshots.
bool DRSDecoder::OP11(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_PB,Data,Length)){
		int size=parser.DecodeValue(GETDWORD(Data));
		w_textview->SetFontSize(size);
		w_textview->SetLeading(size+8);
	}
	return false;
}

// Set text colors (0x13)
//
// Reversed from KANAUS.EXE 0x4200d0: three RGB triples follow the
// length word (window, text ink, shadow). The DRS window draws no
// frame or fill, so only the text ink applies (white in Kana).
bool DRSDecoder::OP13(const Uint8 *Data,int Length){
	if(Length>=9){
		w_textview->SetFontColor(Data[3],Data[4],Data[5]);
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

// Set text window position and print area (IOP_WS)
//
// Reversed from KANAUS.EXE 0x420340: [dst][x][y][w][h] frames the
// dialog, then [tx][ty][tw][th] pins the inner print area that defines
// the text origin, the word wrap width and the page capacity. Kana
// uses a full screen dialog (0,0,640,480) with the print area
// (44,36,552,416) - the missing print area is why the port neither
// wrapped at the original boundary nor kept its margins.
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
		// The click cursor clamps to the dialog window, not to the
		// inner print area (0x41a1c0 reads the window right/bottom)
		w_textview->SetCursorClamp(w,h);
	}
	if(Length>=33){
		// Inner print area, relative to the dialog
		Uint32 tx=GETDWORD(Data+17);
		Uint32 ty=GETDWORD(Data+21);
		Uint32 tw=GETDWORD(Data+25);
		Uint32 th=GETDWORD(Data+29);
		w_textview->SetTextPosition(tx-x,ty-y,tw,th);
	}
	return false;
}


// Load animated cursor sheet (0x21)
//
// Reversed from KANAUS.EXE 0x420430 + 0x419d00: [dst][slot][name\0]
// [frame width][frame height] loads a horizontal frame strip into one
// of six cursor slots; the frame count is sheet width / frame width
// (0x419e04). START.DRS loads ICON_01 - 11 frames of 24x24, an opening
// book with an arrow - into slots 0 (line waits) and 1 (page waits).
// The old IOP_WP interpretation treated the sheet as a window graphic
// and resized the textview to the 24x24 frame: that both wrecked the
// layout and tiled the book frames over the dialog.
bool DRSDecoder::OP21(const Uint8 *Data,int Length){
	if(Length<18){
		LogError("OP21: illegal length %d",Length);
		return false;
	}
	int slot=GETBYTE(Data+1);
	if(slot<0 || slot>5){
		LogError("OP21: illegal cursor slot %d",slot);
		return false;
	}
	char *filename=(char*)Data+2;
	int fw=parser.DecodeValue(GETDWORD(Data+Length-8));
	int fh=parser.DecodeValue(GETDWORD(Data+Length-4));
	SDL_Surface *sheet=LoadImage(filename);
	if(!sheet){
		LogError("OP21: failed to load cursor sheet %s",filename);
		return false;
	}
	if(cursors[slot].sheet){
		SDL_FreeSurface(cursors[slot].sheet);
	}
	cursors[slot].sheet=sheet;
	cursors[slot].fw=fw;
	cursors[slot].fh=fh;
	LogTest("OP21: cursor slot %d = %s (%dx%d frames)",slot,filename,fw,fh);
	return false;
}

// Configure animated cursor (0x22)
//
// Reversed from KANAUS.EXE 0x420520 + 0x419e60: [dst][slot][follow]
// [x][y][delay:word]. follow=0 lets the cursor track the text
// carriage (Kana), a nonzero flag would pin it at a fixed offset;
// delay counts engine ticks per animation frame.
bool DRSDecoder::OP22(const Uint8 *Data,int Length){
	if(Length<15){
		LogError("OP22: illegal length %d",Length);
		return false;
	}
	int slot=GETBYTE(Data+1);
	if(slot<0 || slot>5){
		LogError("OP22: illegal cursor slot %d",slot);
		return false;
	}
	cursors[slot].follow=GETBYTE(Data+2);
	cursors[slot].x=parser.DecodeValue(GETDWORD(Data+3));
	cursors[slot].y=parser.DecodeValue(GETDWORD(Data+7));
	cursors[slot].delay=GETWORD(Data+11);
	if(!cursors[slot].delay){
		cursors[slot].delay=8;
	}
	LogVerbose("OP22: cursor slot %d follow=%d pos=(%d,%d) delay=%d",
				slot,cursors[slot].follow,cursors[slot].x,
				cursors[slot].y,cursors[slot].delay);
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
//
// Reversed from KANAUS.EXE 0x420840 -> 0x4198f0: the payload after
// [dst][length] is a control stream. 0xFF introduces a string of
// 7f-prefixed character pairs that ends at a NUL; every other byte
// goes through the 19-entry control table (0x41ab2c):
//   02  line wait - hold the VM until the line has typed out
//   03  clear the page and home the carriage
//   06  newline (a full page waits for the dismissing click)
//   11  page end - wait for a click with the animated book cursor
//   00, 01, 04..10, 12 and anything above 0x12 is skipped
// The stream is parsed into a queue and executed step by step as the
// wait states complete. The old handler glued every byte below 0x7F
// into the line, so the control-only opcodes (bare 06/03) leaked
// their bytes - and the headers of the following opcodes - into the
// page as tofu boxes and stray punctuation.
bool DRSDecoder::OP2C(const Uint8 *Data,int Length){
	if(GETBYTE(Data)){
		LogError("ILLEGAL WINDOW: %d",GETBYTE(Data));
		return false;
	}
	textqueue_len=0;
	textqueue_idx=0;
	int pos=3;
	while(pos<Length && textqueue_len<24){
		Uint8 c=Data[pos++];
		if(c==0xFF){
			// String of (prefix,character) pairs until the NUL
			uString text;
			while(pos+1<Length && Data[pos]){
				if(Data[pos]==0x7F && Data[pos+1]){
					text+=(char)Data[pos+1];
				}
				pos+=2;
			}
			if(pos<Length && !Data[pos]){
				pos++;
			}
			textqueue[textqueue_len].op=TQ_TEXT;
			textqueue[textqueue_len].text=text;
			textqueue_len++;
		}
		else if(c==0x02){
			textqueue[textqueue_len++].op=TQ_LINEWAIT;
		}
		else if(c==0x03){
			textqueue[textqueue_len++].op=TQ_CLEAR;
		}
		else if(c==0x06){
			textqueue[textqueue_len++].op=TQ_NEWLINE;
		}
		else if(c==0x11){
			textqueue[textqueue_len++].op=TQ_PAGEWAIT;
		}
	}
	return RunTextQueue();
}

// Execute the parsed 0x2C control stream up to the next wait state
bool DRSDecoder::RunTextQueue(){
	while(textqueue_idx<textqueue_len){
		TEXTQUEUEITEM &cmd=textqueue[textqueue_idx++];
		switch(cmd.op){
			case TQ_TEXT:
				w_textview->SetVisible(true);
				if(cmd.text.length()){
					w_textview->PrintText(cmd.text.c_str());
					page_has_text=true;
				}
				// Hold the VM until the line has been typed out. The
				// original draws no cursor while the pen moves: the
				// book only appears when the 02/11 wait starts
				state=IS_WAITTEXT;
				return true;
			case TQ_LINEWAIT:
				// ctl02: a real click wait - 0x41b250(0) parks the
				// interpreter (state 2) until a key or click releases
				// it (0x41ab80); the book cursor (slot 0) sits at the
				// carriage while it waits
				ShowTextCursor(0,true);
				state=IS_WAITCLICK;
				return true;
			case TQ_NEWLINE:
				w_textview->PrintNewline();
				continue;
			case TQ_PAGEWAIT:
				// ctl11: page end - click to continue with the book
				// cursor (slot 1)
				ShowTextCursor(1,true);
				state=IS_WAITCLICK;
				return true;
			case TQ_CLEAR:
				// ctl03: drop the page and home the carriage
				w_textview->ClearText();
				page_has_text=false;
				ShowTextCursor(-1,false);
				continue;
		}
	}
	return false;
}

// Apply a cursor slot (0x21 sheet + 0x22 config) to the textview
void DRSDecoder::ShowTextCursor(int Slot,bool Enable){
	if(!Enable){
		w_textview->ShowCursor(false);
		return;
	}
	if(Slot<0 || Slot>5 || !cursors[Slot].sheet){
		return;
	}
	if(Slot!=cursor_active){
		cursor_active=Slot;
		// The original advances the frame counter every `delay` engine
		// ticks; the tick is the 60Hz frame gate of the idle loop
		// (KANAUS.EXE 0x40d2a0: one tick per 0x11 ms), so one frame
		// holds delay*1000/60 ms (delay 8 = 133ms; the book flips
		// open over ~1.5s). The follow flag (0x22) is always 0 in
		// Kana, so the cursor tracks the carriage, not the offset.
		w_textview->SetCursor(cursors[Slot].sheet,cursors[Slot].fw,
				cursors[Slot].fh,cursors[Slot].delay*1000/60);
	}
	w_textview->ShowCursor(true);
}

// Drain the 0x2C control queue without waiting (skip mode)
void DRSDecoder::SkipTextQueue(){
	while(textqueue_idx<textqueue_len){
		int op=textqueue[textqueue_idx++].op;
		if(op==TQ_NEWLINE){
			w_textview->PrintNewline();
		}
		else if(op==TQ_CLEAR){
			w_textview->ClearText();
			page_has_text=false;
		}
	}
	w_textview->ShowCursor(false);
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




