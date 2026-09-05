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

IkuraDecoder::IkuraDecoder(int Width,int Height) : EngineVN(Width,Height){
	// Preset values
	state=IS_NORMAL;
	asurfaces=0;
	isf_keybuffer=0;
	isf_keysize=0;
	selresult=-1;
    nameMusic = "stop";
	// Create ikura widget
	SDL_Rect bgrect={0,0,Width,Height};
	w_display=new IkuraDisplay(bgrect);
	AddWidget(w_display,VL_BACKGROUND);

	// Create base widgets
	w_textview=new Textview(this);
	w_select=new Selection(this);
	AddWidget(w_textview,VL_TEXTVIEW);
	AddWidget(w_select,VL_CHOICES);

	// Configure default w_textview (Crescendo style)
	w_textview->SetVisible(false);
    w_textview->SetGlobalXY(false);

	w_textview->Resize(640,480);
	w_textview->SetTextPosition(20,20,600,460);
	w_textview->Fill(0,0,0,128);

	// Configure default selection (Crescendo style)
	w_select->SetVisible(false);
	w_select->Resize(640,480);
	w_select->SetColors(0x00000088,0x00000088,0x00000088,0xFFFFFF88);
	w_select->SetAlignment(HA_CENTER,VA_CENTER);
	w_select->SetFontSize(26);

	// Load encryption key
	if(Cfg::System::Keyfile.length()>0){
		RWops ops;
		LogError("Loading key file: %s",Cfg::System::Keyfile.c_str());
		if(ops.OpenFile(Cfg::System::Keyfile.c_str(),"rb")){
			isf_keysize=ops.Size();
			if(isf_keysize>0){
				isf_keybuffer=new char[isf_keysize];
				ops.Read(isf_keybuffer,isf_keysize);
			}
		}
		else{
			LogError("Failed to open key: %s",Cfg::System::Keyfile.c_str());
		}
	}

}

IkuraDecoder::~IkuraDecoder(){
	if(asurfaces){
		for(int i=0;asurfaces[i];i++){
			SDL_FreeSurface(asurfaces[i]);
		}
		delete asurfaces;
	}
	if(isf_keybuffer){
		delete [] isf_keybuffer;
	}
}

void IkuraDecoder::SetKey(char *Buffer,int Size){
	if(isf_keybuffer){
		delete [] isf_keybuffer;
	}
	isf_keysize=Size;
	isf_keybuffer=new char[Size];
	strncpy(isf_keybuffer,Buffer,Size);
}

void IkuraDecoder::DecodeScript(Uint8 *Buffer,int Size){
	if(Size>16 && !strncmp((char*)Buffer+Size-16,"SECRETFILTER100a",16)){
		if(isf_keybuffer && isf_keysize){
			Size-=16;
			int cycles=(Size/isf_keysize)+1;
			LogDebug("Decoding %d bytes ISF data in %d cycles",Size,cycles);
			for(int i=0;i<cycles;i++){
				int o=i*isf_keysize;
				for(int j=0;j<isf_keysize && j+o<Size;j++){
					Buffer[j+o]^=isf_keybuffer[j];
				}
			}
		}
		else{
			LogError("Cannot decrypt script file without a key");
		}
	}
}

bool IkuraDecoder::RunScript(uString Name){
	// Load script from saved position
	bool retval=false;
	RWops *blob=0;
	if((blob=LoadScript(Name.c_str(),"ISF"))){
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Load script data
				DecodeScript(tbuffer,tlength);
				retval=parser.LoadScript(Name.c_str(),tbuffer,tlength,0);
				state=IS_NORMAL;
			}
			delete [] tbuffer;
		}
		delete blob;
	}
	else{
		LogError("Failed to load %s",Name.c_str());
	}

	return retval;
}

void IkuraDecoder::EventGameDialog(VN_DIALOGS Dialog){
	if(Dialog==VD_TITLE){
		RWops *blob=0;
		if((blob=LoadScript("TITLE","ISF"))){
			int tlength=blob->Seek(0,SEEK_END);
			if(tlength>(int)0){
				blob->Seek(0,SEEK_SET);
				Uint8 *tbuffer=new Uint8[tlength];
				if(blob->Read(tbuffer,tlength)>(int)0){
					DecodeScript(tbuffer,tlength);
					parser.LoadScript("TITLE",tbuffer,tlength,0);
					state=IS_NORMAL;
				}
				delete [] tbuffer;
			}
			delete blob;
		}
	}
	else{
		EngineVN::EventGameDialog(Dialog);
	}
}

bool IkuraDecoder::EventBackgroundMouseRightDown(int X,int Y){
	w_textview->SetVisible(!w_textview->GetVisible());
    w_textview->SetGlobalXY(w_textview->GetVisible());

	return true;
}

void IkuraDecoder::EventSelect(int Selection){
	selresult=Selection;
}


bool IkuraDecoder::EventSaveSystem()
{
    EngineVN::EventSaveSystem();
    Savegame save(NativeID(),888);
    if(!parser.SaveSystem(&save)){
		LogError("Parser refused savegame request");
	}
	return save.Write();
}

bool IkuraDecoder::EventLoadSystem()
{
    EngineVN::EventLoadSystem();
    Savegame load(NativeID(),888);
	if(load.Read()){
		// Load settings
		if(!parser.LoadSystem(this,&load)){
			LogError("Parser refused savegame input");
		}
		return true;
	}
	else{
		return false;
	}
}


bool IkuraDecoder::EventLoad(int Index){
    StopSound();
	Savegame *load=new Savegame(NativeID(),Index);
	if(load->Read()){
		if(!parser.Load(this,load)){
			LogError("Parser refused savegame input");
		}
		else if(!w_display->Load(load)){
			LogError("Display module refused savegame input");
		}
		else{
			if(w_select){
				w_select->SetVisible(false);
			}
			if(w_textview){
				w_textview->ClearText();
			}
			LogError("Loaded savegame");
			uString sName;
			load->LoadString("savemsg",&sName);
		    SetSavename(sName);
            load->LoadString("savemusic",&nameMusic);
            if (nameMusic == "stop")
                StopMusic();
            else
                PlayMusic(nameMusic);

		}
		state=IS_NORMAL;
	}
	delete load;
    EventLoadSystem();
	return true;
}

/*! \brief Handles save events
 *  \param Index Internal savegame index
 *  \return True
 */
bool IkuraDecoder::EventSave(int Index){
	Savegame *save=new Savegame(NativeID(),Index);
	if(!parser.Save(save)){
		LogError("Parser refused savegame request");
	}
	else if(!w_display->Save(save)){
		LogError("Display module refused savegame request");
	}
	else{
		// Gather date string and save metadata
		uString date=EDL_DateString(EDL_UnixTime());
		uString time=EDL_TimeString(EDL_UnixTime());
		uString datetime=date+uString(" ")+time;
		save->SaveString("savedate",datetime);
		save->SaveString("savemsg",GetSavename());
		save->SaveString("savemusic",nameMusic);
		// Store graphics onscreen graphics
		SDL_Surface *screen=EDL_CreateSurface(NativeWidth(),NativeHeight());
		Paint(screen,VL_CHOICES);
		save->SaveSurface("screen-thumb",screen,96,72);
        SDL_FreeSurface(screen);
		// Write savegame to disk
		save->Write();
	}
	delete save;
	EventSaveSystem();
	return true;
}

bool IkuraDecoder::EventGameTick(){
	//*
	if(state==IS_WAITTIMER){
		if(timerend<SDL_GetTicks()){
			// Timed state has ended
			while(SkipAnimation());
			state=IS_NORMAL;
		}
		else if(keyctrl()){
            LogError("g1");
			// Skip state and fall back
			w_textview->CompleteText();
			while(SkipAnimation());
			state=IS_NORMAL;
		}
	}
	if(state==IS_WAITTEXT){
		if(!w_textview->GetRemainingText()){

			            LogError("g2");
			// Printing has completed
			state=IS_NORMAL;
		}
		else if(keyctrl()){
			// Skip state and fall back
			w_textview->CompleteText();
			while(SkipAnimation());
			state=IS_NORMAL;
			//LogError("keyctrl");
		}
		else if(keyok){
			//LogError("keyok");
            LogError("g3");
			// Complete printing text
			w_textview->CompleteText();
			keyok=false;
		}
	}
	if(state==IS_WAITCLICK){
		if(keyctrl()){
			// Skip state and fall back
			//LogError("keyctrl");
			w_textview->CompleteText();
			while(SkipAnimation());
			state=IS_NORMAL;
		}
		else if(keyok){
		            LogError("g4");
            //LogError("keyok");
			// Advance text
			if(!w_textview->GetRemainingText()){
				state=IS_NORMAL;
			}
			w_textview->CompleteText();
			while(SkipAnimation());
		}
	}

	return !(state==IS_NORMAL);
	//*/


}

bool IkuraDecoder::EventBackgroundMouseLeftDown(int X,int Y){
    bool retval=EngineVN::EventBackgroundMouseLeftDown(X,Y);
   LogError("EventBackgroundMouseLeftDown");
	return retval;
}
/*! \brief Advances the scripted engine
 *  \returns True if the engine is waiting for user input
 */
bool IkuraDecoder::EventGameProcess(){
	Uint8 c;
	Uint32 l;
	const Uint8 *b;
	bool r=true;

	if(parser.GetOpcode(&c,&b,&l)){
   // if(!keyctrl()){
		switch(c){
			case IOP_CALC:			r=iop_calc(b,l);		/*LogError("iop_calc(%s,%d)",b,l);*/		break;
			case IOP_IM:			r=iop_im(b,l);				break;
			case IOP_IF:			r=iop_if(b,l);				break;
			case IOP_RT:			r=iop_rt(b,l);				break;
			case IOP_SET:			r=iop_set(b,l);				break;
			case IOP_SEP:			r=iop_sep(b,l);				break;
			case IOP_SER:			r=iop_ser(b,l);				break;
			case IOP_GN:			r=iop_gn(b,l);				break;
			case IOP_GC:			r=iop_gc(b,l);				break;
			case IOP_GL:			r=iop_gl(b,l);				break;
			case IOP_GO:			r=iop_go(b,l);				break;
			case IOP_GGE:			r=iop_gge(b,l);				break;
			case IOP_VSET:			r=iop_vset(b,l);			break;
			case IOP_GV:			r=iop_gv(b,l);				break;
			case IOP_GP:			r=iop_gp(b,l);				break;
			case IOP_JP:			r=iop_jp(b,l);				break;
			case IOP_JS:			r=iop_js(b,l);				break;
			case IOP_ONJP:			r=iop_onjp(b,l);			break;
			case IOP_ONJS:			r=iop_onjs(b,l);			break;
			case IOP_SSP:			r=iop_ssp(b,l);				break;
			case IOP_SCP:			r=iop_scp(b,l);				break;
			case IOP_WS:			r=iop_ws(b,l);				break;
			case IOP_WP:			r=iop_wp(b,l);				break;
			case IOP_WSS:			r=iop_wss(b,l);				break;
			case IOP_WSH:			r=iop_wsh(b,l);				break;
			case IOP_PPF:			r=iop_ppf(b,l);				break;
			case IOP_DAS:			r=iop_das(b,l);				break;
			case IOP_DAP:			r=iop_dap(b,l);				break;
			case IOP_PM:			r=iop_pm(b,l);				break;
			case IOP_PF:			r=iop_pf(b,l);				break;
			case IOP_MS:			r=iop_ms(b,l);				break;
			case IOP_ML:			r=iop_ml(b,l);				break;
			case IOP_SES:			r=iop_ses(b,l);				break;
			case IOP_PCMS:			r=iop_pcms(b,l);			break;
			case IOP_PCML:			r=iop_pcml(b,l);			break;
			case IOP_PCMON:			r=iop_pcmon(b,l);			break;
			case IOP_PCMEND:		r=iop_pcend(b,l);			break;
			case IOP_CSET:			r=iop_cset(b,l);			break;
			case IOP_CP:			r=iop_cp(b,l);				break;
			case IOP_CW:			r=iop_cw(b,l);				break;
			case IOP_CWO:			r=iop_cwo(b,l);				break;
			case IOP_CWC:			r=iop_cwc(b,l);				break;
			case IOP_WC:			r=iop_wc(b,l);				break;
			case IOP_WO:			r=iop_wo(b,l);				break;
			case IOP_CC:			r=iop_cc(b,l);				break;
			case IOP_EXC:			r=iop_exc(b,l);				break;
			case IOP_EXS:			r=iop_exs(b,l);				break;
			case IOP_STS:			r=iop_sts(b,l);				break;
			case IOP_IGINIT:		r=iop_iginit(b,l);			break;
			case IOP_IGRELEASE:		r=iop_igrelease(b,l);		break;
			case IOP_STX:			r=iop_stx(b,l);				break;
			case IOP_HDEC:			r=iop_hdec(b,l);			break;
			case IOP_HINC:			r=iop_hinc(b,l);			break;
			case IOP_TITLE:			r=iop_title(b,l);			break;
			case IOP_SETGAMEINFO:	r=iop_setgameinfo(b,l);		break;
			case IOP_CNS:			r=iop_cns(b,l);				break;
			case IOP_UDDA:			r=iop_udda(b,l);			break;
			case IOP_VOC:			r=iop_voc(b,l);				break;


			// Flag related
			case IOP_HS:			r=iop_hs(b,l);				break;
			case IOP_HSG:			r=iop_hsg(b,l);				break;
			case IOP_HT:			r=iop_ht(b,l);				break;
			case IOP_HP:			r=iop_hp(b,l);				break;
			case IOP_HXP:			r=iop_hxp(b,l);				break;
			case IOP_HF:			r=iop_hf(b,l);				break;
			case IOP_SP:			r=iop_sp(b,l);				break;
			case IOP_SK:			r=iop_sk(b,l);				break;
			case IOP_SKS:			r=iop_sks(b,l);				break;
			case IOP_HN:			r=iop_hn(b,l);				break;
			case IOP_EC:			r=iop_ec(b,l);				break;
			case IOP_ES:			r=iop_es(b,l);				break;
			case IOP_FT:			r=iop_ft(b,l);				break;

			// Forced wait states
			case IOP_CLK:			r=iop_clk(b,l);				break;

			// Input definitions
			case IOP_IG:			r=iop_ig(b,l);				break;
			case IOP_IH:			r=iop_ih(b,l);				break;
			case IOP_IHGP:			r=iop_ihgp(b,l);			break;
			case IOP_IHGC:			r=iop_ihgc(b,l);			break;
			case IOP_IHGL:			r=iop_ihgl(b,l);			break;
			case IOP_IHK:			r=iop_ihk(b,l);				break;
			case IOP_IHKDEF:		r=iop_ihkdef(b,l);			break;

			// Script loading
			case IOP_LS:			r=iop_ls(b,l);				break;
			case IOP_LSBS:			r=iop_lsbs(b,l);			break;
			case IOP_SRET:			r=iop_sret(b,l);			break;

			// Timerstuff
			case IOP_ATIMES:		r=iop_atimes(b,l);			break;
			case IOP_AWAIT:			r=iop_await(b,l);			break;
			case IOP_TIMERGET:		r=iop_timerget(b,l);		break;
			case IOP_TIMERSET:		r=iop_timerset(b,l);		break;
			case IOP_TIMEREND:									break;

			// Loading and saving
			case IOP_OPSL:			r=iop_opsl(b,l);			break;
			case IOP_SVF:			r=iop_svf(b,l);				break;

			// Option dialogue
			case IOP_OPPROP:		r=iop_opprop(b,l);			break;
			case IOP_ENABLE:		r=iop_enable(b,l);			break;
			case IOP_DISABLE:		r=iop_disable(b,l);			break;
			case IOP_SLDRSET:		r=iop_sldrset(b,l);			break;
			case IOP_CBSET:			r=iop_cbset(b,l);			break;
			case IOP_RBSET:			r=iop_rbset(b,l);			break;
			case IOP_TAGSET:		r=iop_tagset(b,l);			break;
			case IOP_FRAMESET:		r=iop_frameset(b,l);		break;

			// Animations
			case IOP_GAOPEN:		r=iop_gaopen(b,l);			break;
			case IOP_GASTART:		r=iop_gastart(b,l);			break;
			case IOP_AVIP:			r=iop_avip(b,l);			break;

			// Font and text control
			case IOP_SETFONTCOLOR:	r=iop_setfontcolor(b,l);	break;
			case IOP_SETFONTSTYLE:	r=iop_setfontstyle(b,l);	break;
			case IOP_PB:			r=iop_pb(b,l);				break;
			case IOP_GPB:			r=iop_gpb(b,l);				break;

			// Shut down engine
			case IOP_ED:			r=iop_ed(b,l);				break;

			// Undocumented opcodes
			case IOP_UDAE:			r=iop_udae(b,l);			break;
			case IOP_UDAF:			r=iop_udaf(b,l);			break;
            case IOP_SETINSIDEVOL:  r=iop_sdv(b,l);			    break;
			// Misc unsupported opcodes
			//case IOP_HLN:			// Set number of variables
			//case IOP_FLN:			// Set number of flags
			//case IOP_EXA:			// Space for extra variables
			//case IOP_CPS:			// Set character palette

			// Media stuff

			case IOP_MP:			// Play music (Played upon load)
			case IOP_MF:			// Fade music
			case IOP_PCMCN:			// Cache sound
			case IOP_GASTOP:		// Stop animation
			case IOP_GACLOSE:		// Close animation
			case IOP_GADELETE:		// Delete animation
			// Text management
			case IOP_KIDFN:			// Number of lines (Read-flags)
			case IOP_KIDMOJI:		// Set "read text" font color
			case IOP_KIDSCAN:		// Check wether line has been read
			case IOP_KIDCLR:		// Clear read flags?

			// Cursor control
			case IOP_IC:			// Change cursor
			case IOP_CLKEXMCSET:	// Set "waiting" cursor

																break;
			default:				r=iop_unknown(c,b,l);		break;
		}
        /*}
		else
		{
            switch(c){
            case IOP_CALC:			r=iop_calc(b,l);			break;
			case IOP_IM:			r=iop_im(b,l);				break;
			case IOP_IF:			r=iop_if(b,l);				break;
			case IOP_RT:			r=iop_rt(b,l);				break;
			case IOP_SET:			r=iop_set(b,l);				break;
			case IOP_SEP:			r=iop_sep(b,l);				break;
			//case IOP_SER:			r=iop_ser(b,l);				break;
			case IOP_GN:			r=iop_gn(b,l);				break;
			case IOP_GC:			r=iop_gc(b,l);				break;
			case IOP_GL:			r=iop_gl(b,l);				break;
			case IOP_GO:			r=iop_go(b,l);				break;
			case IOP_GGE:			r=iop_gge(b,l);				break;
			case IOP_VSET:			r=iop_vset(b,l);			break;
			case IOP_GV:			r=iop_gv(b,l);				break;
			case IOP_GP:			r=iop_gp(b,l);				break;
			case IOP_JP:			r=iop_jp(b,l);				break;
			case IOP_JS:			r=iop_js(b,l);				break;
			case IOP_ONJP:			r=iop_onjp(b,l);			break;
			case IOP_ONJS:			r=iop_onjs(b,l);			break;
			case IOP_SSP:			r=iop_ssp(b,l);				break;
			case IOP_SCP:			r=iop_scp(b,l);				break;
			case IOP_WS:			r=iop_ws(b,l);				break;
			case IOP_WP:			r=iop_wp(b,l);				break;
			case IOP_WSS:			r=iop_wss(b,l);				break;
			case IOP_WSH:			r=iop_wsh(b,l);				break;
			case IOP_PPF:			r=iop_ppf(b,l);				break;
			case IOP_DAS:			r=iop_das(b,l);				break;
			case IOP_DAP:			r=iop_dap(b,l);				break;
			case IOP_PM:			r=iop_pm(b,l);				break;
			case IOP_PF:			r=iop_pf(b,l);				break;
			case IOP_MS:			r=iop_ms(b,l);				break;
			case IOP_ML:			r=iop_ml(b,l);				break;
			case IOP_SES:			r=iop_ses(b,l);				break;
			case IOP_PCMS:			r=iop_pcms(b,l);			break;
			//case IOP_PCML:			r=iop_pcml(b,l);			break;
			case IOP_PCMON:			r=iop_pcmon(b,l);			break;
			case IOP_CSET:			r=iop_cset(b,l);			break;
			case IOP_CP:			r=iop_cp(b,l);				break;
			case IOP_CW:			r=iop_cw(b,l);				break;
			case IOP_CWO:			r=iop_cwo(b,l);				break;
			case IOP_CWC:			r=iop_cwc(b,l);				break;
			case IOP_WC:			r=iop_wc(b,l);				break;
			case IOP_WO:			r=iop_wo(b,l);				break;
			case IOP_CC:			r=iop_cc(b,l);				break;
			case IOP_EXC:			r=iop_exc(b,l);				break;
			case IOP_EXS:			r=iop_exs(b,l);				break;
			case IOP_STS:			r=iop_sts(b,l);				break;
			case IOP_IGINIT:		r=iop_iginit(b,l);			break;
			case IOP_IGRELEASE:		r=iop_igrelease(b,l);		break;
			case IOP_STX:			r=iop_stx(b,l);				break;
			case IOP_HDEC:			r=iop_hdec(b,l);			break;
			case IOP_HINC:			r=iop_hinc(b,l);			break;
			case IOP_TITLE:			r=iop_title(b,l);			break;
			case IOP_SETGAMEINFO:	r=iop_setgameinfo(b,l);		break;
			case IOP_CNS:			r=iop_cns(b,l);				break;
			case IOP_UDDA:			r=iop_udda(b,l);			break;
			case IOP_VOC:			r=iop_voc(b,l);				break;


			// Flag related
			case IOP_HS:			r=iop_hs(b,l);				break;
			case IOP_HSG:			r=iop_hsg(b,l);				break;
			case IOP_HT:			r=iop_ht(b,l);				break;
			case IOP_HP:			r=iop_hp(b,l);				break;
			case IOP_HXP:			r=iop_hxp(b,l);				break;
			case IOP_HF:			r=iop_hf(b,l);				break;
			case IOP_SP:			r=iop_sp(b,l);				break;
			case IOP_SK:			r=iop_sk(b,l);				break;
			case IOP_SKS:			r=iop_sks(b,l);				break;
			case IOP_HN:			r=iop_hn(b,l);				break;
			case IOP_EC:			r=iop_ec(b,l);				break;
			case IOP_ES:			r=iop_es(b,l);				break;
			case IOP_FT:			r=iop_ft(b,l);				break;

			// Forced wait states
			//case IOP_CLK:			r=iop_clk(b,l);				break;

			// Input definitions
			case IOP_IG:			r=iop_ig(b,l);				break;
			case IOP_IH:			r=iop_ih(b,l);				break;
			case IOP_IHGP:			r=iop_ihgp(b,l);			break;
			case IOP_IHGC:			r=iop_ihgc(b,l);			break;
			case IOP_IHGL:			r=iop_ihgl(b,l);			break;
			case IOP_IHK:			r=iop_ihk(b,l);				break;
			case IOP_IHKDEF:		r=iop_ihkdef(b,l);			break;

			// Script loading
			case IOP_LS:			r=iop_ls(b,l);				break;
			case IOP_LSBS:			r=iop_lsbs(b,l);			break;
			case IOP_SRET:			r=iop_sret(b,l);			break;

			// Timerstuff
			case IOP_ATIMES:		r=iop_atimes(b,l);			break;
			case IOP_AWAIT:			r=iop_await(b,l);			break;
			//case IOP_TIMERGET:		r=iop_timerget(b,l);		break;
			//case IOP_TIMERSET:		r=iop_timerset(b,l);		break;
			//case IOP_TIMEREND:									break;

			// Loading and saving
			//case IOP_OPSL:			r=iop_opsl(b,l);			break;
			//case IOP_SVF:			r=iop_svf(b,l);				break;

			// Option dialogue
			//case IOP_OPPROP:		r=iop_opprop(b,l);			break;
			//case IOP_ENABLE:		r=iop_enable(b,l);			break;
			//case IOP_DISABLE:		r=iop_disable(b,l);			break;
			//case IOP_SLDRSET:		r=iop_sldrset(b,l);			break;
			//case IOP_CBSET:			r=iop_cbset(b,l);			break;
			//case IOP_RBSET:			r=iop_rbset(b,l);			break;
			//case IOP_TAGSET:		r=iop_tagset(b,l);			break;
			//case IOP_FRAMESET:		r=iop_frameset(b,l);		break;

					// Animations
			case IOP_GAOPEN:		r=iop_gaopen(b,l);			break;
			case IOP_GASTART:		r=iop_gastart(b,l);			break;
		//	case IOP_AVIP:			r=iop_avip(b,l);			break;

		//	// Font and text control
		//	case IOP_SETFONTCOLOR:	r=iop_setfontcolor(b,l);	break;
		//	case IOP_SETFONTSTYLE:	r=iop_setfontstyle(b,l);	break;
		//	case IOP_PB:			r=iop_pb(b,l);				break;
		//	case IOP_GPB:			r=iop_gpb(b,l);				break;

			// Shut down engine
			//case IOP_ED:			r=iop_ed(b,l);				break;

			// Undocumented opcodes
			//case IOP_UDAE:			r=iop_udae(b,l);			break;
			//case IOP_UDAF:			r=iop_udaf(b,l);			break;
          //  case IOP_SETINSIDEVOL:  r=iop_sdv(b,l);			    break;

			// Media stuff

		//	case IOP_MP:			// Play music (Played upon load)
		//	case IOP_MF:			// Fade music
		//	case IOP_PCMCN:			// Cache sound
		//	case IOP_GASTOP:		// Stop animation
		//	case IOP_GACLOSE:		// Close animation
		//	case IOP_GADELETE:		// Delete animation
			// Text management
		//	case IOP_KIDFN:			// Number of lines (Read-flags)
		//	case IOP_KIDMOJI:		// Set "read text" font color
		//	case IOP_KIDSCAN:		// Check wether line has been read
		//	case IOP_KIDCLR:		// Clear read flags?

			// Cursor control
		//	case IOP_IC:			// Change cursor
		//	case IOP_CLKEXMCSET:	// Set "waiting" cursor


			default:				r=iop_unknown(c,b,l);		break;
            }
		}*/
     }
	return r;
}

bool IkuraDecoder::iop_check(int ExpectedLength,
		const Uint8 Code,const Uint8 *Data,int Length){
	if(ExpectedLength==Length){
		return true;
	}
	else{
		LogError("Expected %d (not %d!) bytes for %x:",
				ExpectedLength,Length,Code);
		for(int i=0;i<Length;i++){
			LogError("%x (%c)",Data[i],Data[i]);
		}
		return false;
	}
}

bool IkuraDecoder::iop_unknown(const Uint8 Code,
		const Uint8 *Data,int Length){
	uString data;
	char buffer[32];
	for(int i=0;i<Length;i++){
		sprintf(buffer," 0x%x",Data[i]);
		data+=buffer;
	}
	LogError("Unknown OPC:0x%x (Length=%d): %s",Code,Length,data.c_str());
	return false;
}

// Set text cliprect?
bool IkuraDecoder::iop_udae(const Uint8 *Data,int Length){
	if(iop_check(17,0xAE,Data,Length)){
		Uint8 dst=GETBYTE(Data);
		if(dst){
			LogError("ILLEGAL TARGET WINDOW: %d",dst);
		}
		Uint32 x=GETDWORD(Data+1);
		Uint32 y=GETDWORD(Data+5);
		Uint32 w=GETDWORD(Data+9);
		Uint32 h=GETDWORD(Data+13);
		LogError("0xAE window position: %d x %d x %d x %d",x,y,w,h);
	}
	return false;
}

// Position of wait animation?
bool IkuraDecoder::iop_udaf(const Uint8 *Data,int Length){
	if(iop_check(17,0xAE,Data,Length)){
		Uint8 dst=GETBYTE(Data);
		if(dst){
			LogError("ILLEGAL TARGET WINDOW: %d",dst);
		}
		Uint32 x=GETDWORD(Data+1);
		Uint32 y=GETDWORD(Data+5);
		Uint32 w=GETDWORD(Data+9);
		Uint32 h=GETDWORD(Data+13);
		LogError("0xAF window position: %d x %d x %d x %d",x,y,w,h);
		//SetTextDisplacement(x,y);
	}
	return false;
}

// Open options dialoge
bool IkuraDecoder::iop_opprop(const Uint8 *Data,int Length){
	EventGameDialog(VD_OPTIONS);
	return false;
}

// Enable access to options dialogue
bool IkuraDecoder::iop_enable(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_ENABLE:%d",Length);
	return false;
}

// Disable access to options dialogue
bool IkuraDecoder::iop_disable(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_DISABLE:%d",Length);
	return false;
}

// Set slider widget
bool IkuraDecoder::iop_sldrset(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_SLDRSET:%d",Length);
	return false;
}

// Set checkbox widget
bool IkuraDecoder::iop_cbset(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_CBSET:%d",Length);
	return false;
}

// Set radiobox widget
bool IkuraDecoder::iop_rbset(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_RBSET:%d",Length);
	return false;
}

// Set tab caption
bool IkuraDecoder::iop_tagset(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_TAGSET:%d",Length);
	return false;
}

// Control caption
bool IkuraDecoder::iop_frameset(const Uint8 *Data,int Length){
	LogDebug("Unhandled iop: IOP_FRAMESET:%d",Length);
	return false;
}

// Open save and load
bool IkuraDecoder::iop_opsl(const Uint8 *Data,int Length){
	if(iop_check(1,IOP_OPSL,Data,Length)){
		if(Data[0]==1){
			EventGameDialog(VD_LOAD);
		}
		else{
			EventGameDialog(VD_SAVE);
		}
	}
	return true;
}

// Enable/Disable save
bool IkuraDecoder::iop_svf(const Uint8 *Data,int Length){
	if(iop_check(1,IOP_SVF,Data,Length)){
		if(Data[0]){

			//LogError("Enable save");
		}
		else{
           // LogError("Disable save");
			// Disable save
		}
	}
	return false;
}

// Open text window
bool IkuraDecoder::iop_wo(const Uint8 *Data,int Length){
	w_textview->SetVisible(true);
	w_textview->SetGlobalXY(true);
	return false;
}

// Close text window
bool IkuraDecoder::iop_wc(const Uint8 *Data,int Length){
    w_textview->SetVisible(false);
    w_textview->SetGlobalXY(false);
	// w_textview->ClearText();
	return false;
}

// Set text window graphic
bool IkuraDecoder::iop_wp(const Uint8 *Data,int Length){
	// Get provided text window data
	Uint16 dst=GETWORD(Data);
	if(dst){
		LogError("ILLEGAL WINDOW: %d",dst);
	}
	int cnt=0;
	for(int i=2;i<Length;){
		SDL_Surface *tmpsurface=LoadImage((char*)Data+i);
		if(tmpsurface){
			if(cnt==0){
				// Load textview graphics
				w_textview->Resize(tmpsurface->w,tmpsurface->h);
				w_textview->Set(tmpsurface);
			}
			SDL_FreeSurface(tmpsurface);
			cnt++;
		}
		while(i<Length && Data[i++]);
	}
	return false;
}

// Set text window position
bool IkuraDecoder::iop_ws(const Uint8 *Data,int Length){
	if(iop_check(18,IOP_WS,Data,Length)){
		Uint8 dst=GETBYTE(Data);
		if(dst){
			LogError("ILLEGAL TARGET WINDOW: %d",dst);
		}
		Uint32 x=GETDWORD(Data+1);
		Uint32 y=GETDWORD(Data+5);
		Uint32 w=GETDWORD(Data+9);
		Uint32 h=GETDWORD(Data+13);
		LogError("WINDOW POSITION?: %d x %d x %d x %d",x,y,w,h);
		w_textview->MoveDialog(x,y);
		if(w && h){
			w_textview->Resize(w,h);
		}
	}
	return false;
}

// Hide text window
bool IkuraDecoder::iop_wsh(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_WSH,Data,Length)){
		//LogError("Hide text window:%d",parser.DecodeValue(GETDWORD(Data)));
		w_textview->SetVisible(false);
        w_textview->SetGlobalXY(false);
		w_textview->ClearText();
	}
	return false;
}

// Show text window
bool IkuraDecoder::iop_wss(const Uint8 *Data,int Length){
	// Set text window dimensions
	if(iop_check(4,IOP_WSS,Data,Length)){
		//LogError("Show text window:%d",parser.DecodeValue(GETDWORD(Data)));
		w_textview->SetVisible(true);
		w_textview->SetGlobalXY(true);
	}
	return false;
}

// Possibly textual commands to the windows mci api
bool IkuraDecoder::iop_im(const Uint8 *Data,int Length){
	LogError("IM:%d:%s",Data[0],Data+1);
	return false;
}

// IF-THEN logic block (May be nested!)
bool IkuraDecoder::iop_if(const Uint8 *Data,int Length){
	bool result=true;
	for(int i=0;result && i<Length;i+=10){
		// Get value and operator
		Uint8 o=GETBYTE(Data+i+4);
		int v1=parser.DecodeValue(GETDWORD(Data+i));
		int v2=parser.DecodeValue(GETDWORD(Data+i+5));

		// Comparative operation with last value
		switch(o){
			case 0x00:		result=(v1==v2);	break;
			case 0x01:		result=(v1<v2);		break;
			case 0x02:		result=(v1<=v2);	break;
			case 0x03:		result=(v1>v2);		break;
			case 0x04:		result=(v1>=v2);	break;
			case 0x05:		result=(v1!=v2);	break;
			default:		result=false;		break;
		}

		// Check next operation
		o=GETBYTE(Data+i+9);
		if(!result){
			// Dont process any further ..
		}
		else if(o==0x00){
			// Get jumptable ref
			parser.JumpFunction(GETWORD(Data+i+10));
			result=false;
		}
		else if(o==0x01){
			// Set variable
			Uint16 dst=GETWORD(Data+i+10);
			int val=parser.DecodeValue(GETDWORD(Data+i+12));
			parser.SetValue(dst,val);
			result=false;
		}
		else if(o==0x02){
			// AND next operation ...
		}
		else if(o==0xFF){
			// End if
			result=false;
		}
		else{
			LogError("UNKNOWN IF THEN OPERATOR: %x",o);
		}
	}
	return false;
}

// Decrements a register value
bool IkuraDecoder::iop_hdec(const Uint8 *Data,int Length){
	if(iop_check(2,IOP_HDEC,Data,Length)){
		Uint16 dest=GETWORD(Data);
		parser.SetValue(dest,parser.GetValue(dest)-1);
	}
	return false;
}

// Increments a register value
bool IkuraDecoder::iop_hinc(const Uint8 *Data,int Length){
	if(iop_check(2,IOP_HINC,Data,Length)){
		Uint16 dest=GETWORD(Data);
		parser.SetValue(dest,parser.GetValue(dest)+1);
	}
	return false;
}

// Set group of variables (Like a for-loop)
bool IkuraDecoder::iop_hsg(const Uint8 *Data,int Length){
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

// Assign variable
bool IkuraDecoder::iop_hs(const Uint8 *Data,int Length){
	if(iop_check(6,IOP_HS,Data,Length)){
		Uint16 entry=GETWORD(Data);
		int value=parser.DecodeValue(GETDWORD(Data+2));
		parser.SetValue(entry,value);
	}
	return false;
}

// Transfer variables
bool IkuraDecoder::iop_ht(const Uint8 *Data,int Length){
	if(iop_check(6,IOP_HT,Data,Length)){
		Uint16 src=GETWORD(Data);
		Uint16 dst=GETWORD(Data+2);
		Uint16 cnt=GETWORD(Data+4);
		for(int i=0;i<cnt;i++){
			parser.SetValue(dst+i,parser.GetValue(src+i));
		}
	}
	return false;
}

// Load system flag flags (Ignored ... handled internally in ViLE)
bool IkuraDecoder::iop_ec(const Uint8 *Data,int Length){
	Uint16 start=GETWORD(Data+0);
	Uint16 end=GETWORD(Data+2);
	Uint16 length=end-length;
	for(int i=0;i<length;i++){
	}
	LogError("Load system flags: %d -> %d",start,end);
	return false;
}

// Save system flag flags (Ignored ... handled internally in ViLE)
bool IkuraDecoder::iop_es(const Uint8 *Data,int Length){
	LogError("Save system flags: %d",parser.DecodeValue(GETDWORD(Data)));
	return false;
}

// Transfer flags
bool IkuraDecoder::iop_ft(const Uint8 *Data,int Length){
	LogError("UNHANDLED FT FLAGS: %d",Length);
	return false;
}

// Jump if flags[dst]==0? (3B)
bool IkuraDecoder::iop_hn(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_HN,Data,Length)){
		Uint8 value=parser.GetFlag(GETWORD(Data));
		Uint16 entry=GETWORD(Data+2);
		if(!value){
			parser.JumpFunction(entry);
		}
	}
	return false;
}

// Set flag pattern (35)
//
// Seems to set variable depending on read flags:
// 		SP  1,(106,107,108,109)
//
// Flags can be expanded like below, destination does not seem to be
// a flag and can be read by the hp opcode
bool IkuraDecoder::iop_sp(const Uint8 *Data,int Length){
	Uint8 dst=Data[0];
	Uint8 *buffer=new Uint8[Length-1];
	for(int i=1;i<Length;i++){
		buffer[i-1]=Data[i];
	}
	parser.Setpattern(dst,buffer,Length-1);
	return false;
}


// Jump if flag set (3C)
bool IkuraDecoder::iop_hxp(const Uint8 *Data,int Length){
	// Parse parameters
	Uint8 src=GETBYTE(Data);
	Uint8 cmd=GETBYTE(Data+1);
	Uint16 entry=GETWORD(Data+2);
	if(parser.Hitpattern(src,cmd)){
		parser.JumpFunction(entry);
	}
	return false;
}

// Jump if flag set
//
// Flag is the register set by IOP_SP (36)
bool IkuraDecoder::iop_hp(const Uint8 *Data,int Length){
	// Parse parameters
	Uint8 src=GETBYTE(Data);
	Uint16 entry=GETWORD(Data+1);
	if(parser.Hitpattern(src,0)){
		parser.JumpFunction(entry);
	}
	return false;
}

// Jump if flag set (0x33)
bool IkuraDecoder::iop_hf(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_HF,Data,Length)){
		int index=GETWORD(Data);
		int address=GETWORD(Data+2);
		bool flag=parser.GetFlag(index);
		if(flag){
			parser.JumpFunction(address);
		}
	}
	return false;
}

// Set group of flags (32)
bool IkuraDecoder::iop_sks(const Uint8 *Data,int Length){
	if(iop_check(5,IOP_SKS,Data,Length)){
		Uint16 start=GETWORD(Data+0);
		Uint16 end=GETWORD(Data+2);
		Uint8 cmd=GETBYTE(Data+4);
		for(int i=start;i<end;i++){
			bool bit=parser.GetFlag(i);
			switch(cmd){
				case 0:		parser.SetFlag(i,0);	break;	// Clear bit
				case 1:		parser.SetFlag(i,1);	break;	// Set bit
				case 2:		parser.SetFlag(i,!bit);	break;	// Toggle bit
			}
		}
	}
	EventSaveSystem();
	return false;
}

// Set flag (31)
bool IkuraDecoder::iop_sk(const Uint8 *Data,int Length){
	if(iop_check(3,IOP_SK,Data,Length)){
		Uint16 dst=GETWORD(Data);
		Uint8 cmd=GETBYTE(Data+2);
		bool bit=parser.GetFlag(dst);
		switch(cmd){
			case 0:		parser.SetFlag(dst,0);		break;	// Clear bit
			case 1:		parser.SetFlag(dst,1);		break;	// Set bit
			case 2:		parser.SetFlag(dst,!bit);	break;	// Toggle bit
		}
	}
	EventSaveSystem();
	return false;
}

// Simple calculus
bool IkuraDecoder::iop_calc(const Uint8 *Data,int Length){
	/*
	   XKazoku seems to be using something like this. It looks insane to
	   me, and does not seem to be working very well either.

	int v1=0;
	int v2=0;
	Uint16 dest=GETWORD(Data);
	for(int i=2;i<Length;){
		Uint8 op=Data[i++];
		while(op<0x05){
			// Read another 32b value
			int value=parser.DecodeValue(GETDWORD(Data+i));
			i+=4;

			// Apply mathematical operator
			Uint8 op2=Data[i++];
			if((op2&0x0F)<2){
				switch(op2){
					case 0x00:	v2+=value;						break;
					case 0x01:	v2-=value;						break;
					case 0x02:	v2+=(v1*value);	v1=0;			break;
					case 0x03:	if(value) v2+=(v1/value); v1=0;	break;
					case 0x04:	if(value) v2+=(v1%value); v1=0;	break;
					default:									break;
				}
			}
			else{
				switch(op){
					case 0x00:	v1=value;					break;
					case 0x01:	v1=-value;					break;
					case 0x02:	v1*=value;					break;
					case 0x03:	v1=value?v1/value:0;		break;
					case 0x04:	v1=value?v1%value:0;		break;
					default:								break;
				}
			}
			op=op2;
		}
	}

	// Copy result to target
	parser.SetValue(dest,v1+v2);
	return false;
	*/

	int result=0;
	Uint16 dest=GETWORD(Data);
	for(int i=2;i<Length;){
		Uint8 op=Data[i++];
		if(op<0x05){
			// Read another 32b value
			int value=parser.DecodeValue(GETDWORD(Data+i));
			i+=4;

			// Apply mathematical operator
			switch(op){
				case 0x00:	result+=value;	break;
				case 0x01:	result-=value;	break;
				case 0x02:	result*=value;	break;
				case 0x03:	result/=value;	break;
				case 0x04:	result%=value;	break;
				default:					break;
			}
		}
		else if(op==0x05){
			// End
		}
		else{
			LogError("Unknown CALC operator:%x",op);
			for(int j=0;j<Length;j++){
				LogError("%x",Data[j]);
			}
			return false;
		}
	}

	// Copy result to target
	parser.SetValue(dest,result);
	return false;
}

// Enable or disable popup ...
bool IkuraDecoder::iop_ppf(const Uint8 *Data,int Length){
	if(iop_check(1,IOP_PPF,Data,Length)){
		if(Data[0]){
			// Enable popup
		}
		else{
			// Disable popup
		}
	}
	return false;
}

// Set text display speed
bool IkuraDecoder::iop_pf(const Uint8 *Data,int Length){
	if(iop_check(5,IOP_PF,Data,Length)){
		Uint32 dst=GETBYTE(Data);
		int val=parser.DecodeValue(GETDWORD(Data+1));
		if(!dst){
			w_textview->SetTextInterval(val);
		}
		else{
			LogError("OTHER WINDOWS NOT SUPPORTED: %d",dst);
		}
	}
	return false;
}

// Print text messages
bool IkuraDecoder::iop_pm(const Uint8 *Data,int Length){
/*
char b[255];
uString sc,sx;
for(int i=1;i<Length;i++){
sprintf(b,"% 2c ",Data[i]);
sc+=b;
sprintf(b,"%02X ",Data[i]);
sx+=b;
}
LogError("PM:%s",sc.c_str());
LogError("PM:%s",sx.c_str());
//*/
	Uint8 win=Data[0];
	if(win){
		LogError("Unknown text window:%d",win);
	}
	for(int i=1;i<Length;){
		Uint8 cmd=Data[i++];
		if(cmd==0x03){
			// Clear screen
			w_textview->ClearText();
		}
		else if(cmd==0x01 && Data[i]==0x00){
			// Specify color inline
			Uint8 r=Data[++i];
			Uint8 g=Data[++i];
			Uint8 b=Data[++i];
			w_textview->SetFontColor(r,g,b);
			i++;
		}
		else if(cmd==0x01 && Data[i]==0x01){
			// ???
			Uint16 x=GETWORD(Data+i+1);
			i+=2;
		}
		else if(cmd==0x04){
			// ???
			Uint8 x=GETBYTE(Data+i);
			i++;
		}
		else if(cmd==0x06){
			// ???
		}
		else if(cmd==0x08){
			// Previously read
			int linenumber=parser.DecodeValue(GETDWORD(Data+i));
			i+=4;
		}
		else if(cmd==0x11){
			// Name of talking character
			char tbuf[32];
			int name=parser.DecodeValue(GETDWORD(Data+i));
			sprintf(tbuf,"FW%3.3d",name);
			SDL_Surface *tmpsurface=LoadImage(tbuf);
			if(tmpsurface){
				w_textview->Blit(tmpsurface);
				SDL_FreeSurface(tmpsurface);
			}
			i+=4;
		}
		else if(cmd==0x13){
			if(Data[i]>0x1F){
                LogError("PlayVoice %s",(char*)Data+i);
				// Play associated voice file
				PlayVoice((char*)Data+i);
				while(i<Length && Data[i++]);
			}
			else if(Data[i]==0x04){
				// Print character name
				Uint16 index=GETWORD(Data+i+1);
				uString name;
				if(s_names.GetString(index,&name)){
					w_textview->PrintText(name);
					w_textview->CompleteText();
				}
				i+=3;
			}
			else{
				LogError("Unknown voice command: 0x%02X",Data[i]);
			}
		}
		else if(cmd==0xFF){
			// Print text
			uString text;
			int d=0;

			for(;Data[i];i++){
                d=Data[i];

                if((Data[i]>0xBF && Data[i]<=0xFF))  d+=0x0300;

				if((Data[i]<0x7F && Data[i]>0x1F)||(Data[i]>0xBF && Data[i]<=0xFF))
					text+=(char)d;

			}

			w_textview->SetVisible(true);
            w_textview->SetGlobalXY(true);

			w_textview->PrintNewline();
			w_textview->PrintText(text.c_str());

			// Change state
			state=IS_WAITTEXT;
		}
		else if(cmd){
			LogError("Unknown inline command: 0x%02X",cmd);
		}
	}
	return true;
}

// Stop music
bool IkuraDecoder::iop_ms(const Uint8 *Data,int Length){
	StopMusic();
    nameMusic = "stop";
	return false;
}

// Load music track
bool IkuraDecoder::iop_ml(const Uint8 *Data,int Length){
	//Uint8 par=Data[Length-1];
	PlayMusic((char*)Data);
    nameMusic = (char*)Data;
	return false;
}

// Stop CDDA music
bool IkuraDecoder::iop_das(const Uint8 *Data,int Length){
	StopMusic();
	return false;
}

// Load CDDA music track
bool IkuraDecoder::iop_dap(const Uint8 *Data,int Length){
	int channel=parser.DecodeValue(GETDWORD(Data));
	if(!PlayMusic(EDL_Format("TK%02d",channel))){
		if(!PlayMusic(EDL_Format("TK_%02d",channel))){
			if(!PlayMusic(EDL_Format("TK-%02d",channel))){
				LogError("CDDA playback not supported!");
			}
		}
	}
    LogError("Load CDDA music track %s",(char*)Data);
	PlayMusic((char*)Data);
	return false;
}

// Stop sound effect
bool IkuraDecoder::iop_ses(const Uint8 *Data,int Length){
	if(iop_check(8,IOP_SES,Data,Length)){
		int channel=parser.DecodeValue(GETDWORD(Data));
		//int duration=parser.DecodeValue(GETDWORD(Data+4));
		StopSound(channel);
	}
	return false;
}

// Play PCM (voice)
bool IkuraDecoder::iop_pcmon(const Uint8 *Data,int Length){
   // LogError("iop_pcmon PCM (voice) %s",(char*)Data);
	return false;
}

bool IkuraDecoder::iop_pcend(const Uint8 *Data,int Length){
   // LogError("iop_pcmend PCM (voice) %s",(char*)Data);
	return false;
}

// Stop PCM (voice)
bool IkuraDecoder::iop_pcms(const Uint8 *Data,int Length){
	StopSound(VA_VOICES);
	return false;
}

// Load PCM (voice)
bool IkuraDecoder::iop_pcml(const Uint8 *Data,int Length){
    uString Name = (char*)Data;
    LogError("iop_pcml (voice) %s",(char*)Data);

	PlayVoice(Name.c_str(),VA_VOICES);

	return true;
}

// Return from subscript
bool IkuraDecoder::iop_sret(const Uint8 *Data,int Length){
	parser.ReturnScript();
	return false;
}

// Call a subscript
bool IkuraDecoder::iop_lsbs(const Uint8 *Data,int Length){
	RWops *blob=0;
	if((blob=LoadScript((char*)Data,"ISF"))){
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Load script and CALL it
				DecodeScript(tbuffer,tlength);
				parser.CallScript((char*)Data,tbuffer,tlength);
			}
			delete [] tbuffer;
		}
		delete blob;
	}
	return false;
}

// Loads a new script
bool IkuraDecoder::iop_ls(const Uint8 *Data,int Length){
	RWops *blob=0;
	if((blob=LoadScript((char*)Data,"ISF"))){
		int tlength=blob->Seek(0,SEEK_END);
		if(tlength>(int)0){
			blob->Seek(0,SEEK_SET);
			Uint8 *tbuffer=new Uint8[tlength];
			if(blob->Read(tbuffer,tlength)>(int)0){
				// Load script and JMP to it
				DecodeScript(tbuffer,tlength);
				parser.JumpScript((char*)Data,tbuffer,tlength);
			}
			delete [] tbuffer;
		}
		delete blob;
	}
	EventLoadSystem();
	return false;
}

// Sets window caption
bool IkuraDecoder::iop_title(const Uint8 *Data,int Length){
	uString t="ViLE: ";
	t+=(char*)Data;
//	SDL_WM_SetCaption(t.c_str(),t.c_str());
	return false;
}

// Sets a name for the savegame
bool IkuraDecoder::iop_setgameinfo(const Uint8 *Data,int Length){
	SetSavename((char*)Data);
	return false;
}

// Store character names
bool IkuraDecoder::iop_cns(const Uint8 *Data,int Length){
	//Uint8 par1=Data[0];
	Uint8 index=Data[1];
	char name[Length];
	strncpy(name,(char*)Data+2,Length-2);
	name[Length-2]=0;
	s_names.SetString(index,name);
	LogError("Registering character name %d=%s",index,name);
	return false;
}

// Voice file registration (For enabling disabling individual voices)
bool IkuraDecoder::iop_voc(const Uint8 *Data,int Length){
	Uint32 value1=parser.DecodeValue(GETDWORD(Data));
	Uint16 value2=GETWORD(Data+4);
	uString voc=(char*)Data+6;
	LogError("Registering voice file %d %d:%s",value1,value2,voc.c_str());
	return false;
}

// Registers number of voicefile types
bool IkuraDecoder::iop_udda(const Uint8 *Data,int Length){
	Uint32 value=parser.DecodeValue(GETDWORD(Data));
	LogError("Voicefile registrations:%d",value);
	return false;
}

bool IkuraDecoder::iop_clk(const Uint8 *Data,int Length){
	state=IS_WAITCLICK;
	return true;
}

bool IkuraDecoder::iop_cp(const Uint8 *Data,int Length){
	LogError("Loading command window:%s",Data);
	return false;
}

// Close command screen
bool IkuraDecoder::iop_cwc(const Uint8 *Data,int Length){
	LogError("Closing Command window: %d",Data[0]);
	w_textview->SetVisible(false);
    w_textview->SetGlobalXY(false);

	return false;
}

// Set command window position
bool IkuraDecoder::iop_cw(const Uint8 *Data,int Length){
	Uint8 dst=GETBYTE(Data);
	Uint32 x=GETDWORD(Data+1);
	Uint32 y=GETDWORD(Data+5);
	Uint32 w=GETDWORD(Data+9);
	Uint32 h=GETDWORD(Data+13);
	Uint8 cmd=GETBYTE(Data+17);
	if(dst){
		LogError("ILLEGAL COMMAND WINDOW:%d",dst);
	}
	LogError("Command window position:%d,%d,%d,%d,%d,%d",dst,x,y,w,h,cmd);
	return false;
}

// Open command screen
// par1 might have something to do with a destination
// par2 might be number of options
bool IkuraDecoder::iop_cwo(const Uint8 *Data,int Length){
	// Decode parameters
	Uint8 par1=GETBYTE(Data+0);
	Uint8 par3=GETBYTE(Data+5);
	int cnt=parser.DecodeValue(GETDWORD(Data+1));
	LogTest("Opening Command selection: %d,%d,%d",par1,cnt,par3);
	if(w_select){
		w_select->SetVisible(true);
		selresult=-1;
	}
	return true;
}

// Execute command selection
bool IkuraDecoder::iop_cc(const Uint8 *Data,int Length){
		w_textview->SetVisible(false);
        w_textview->SetGlobalXY(false);

        w_textview->ClearText();

    //LogError("iop_cc");

	if(iop_check(4,IOP_CC,Data,Length)){
		// Parse values
		//LogTest("CC:%x:%x:%x:%x",Data[0],Data[1],Data[2],Data[3]);
		Uint8 par1=Data[0];
		Uint8 dst=Data[1];
		Uint8 par2=Data[2];
		Uint8 cmd=Data[3];

		// Interpret command field

		if(!w_select){
			LogError("Invalid selection control");
		}
		else if(cmd==0x01){
			// Set default value for target register
			parser.SetValue(dst,-1);
		}
		else if(cmd==0x02){
			// Closing command selection
			w_select->SetVisible(false);
			w_select->Clear();
		}
		else if(cmd==0x03){
			// Get command status
			if(selresult>=0){
				parser.SetValue(dst,selresult);
			}
		}
		else{
			LogError("Unknown selection command:%d (%d,%d)",cmd,par1,par2);
		}
	}
	return false;
}

// Configures a command option
bool IkuraDecoder::iop_cset(const Uint8 *Data,int Length){
	Uint8 dst=GETBYTE(Data);		// Where to place the result
	Uint8 val=GETBYTE(Data+1);		// Identifying value
	Uint32 x=GETDWORD(Data+2);
	Uint32 y=GETDWORD(Data+6);
	Uint32 w=GETDWORD(Data+10);
	Uint32 h=GETDWORD(Data+14);

	// Check destination
	if(dst){
		LogError("Unexpected CSET destination:%d",dst);
	}

	// Extract and terminate caption
	char caption[Length-18];
	strncpy(caption,(char*)Data+18,Length-18);
	caption[Length-18]=0;

	// Create a new command option
	SDL_Rect rect={x,y*3,w,h};
	if(w_select){
		w_select->SetText(caption,rect,val);
	}

	return false;
}

// Set font color
bool IkuraDecoder::iop_setfontcolor(const Uint8 *Data,int Length){
	if(Length==5){
		if(iop_check(5,IOP_SETFONTCOLOR,Data,Length)){
			Uint8 window=GETBYTE(Data+0);
			Uint8 font=GETBYTE(Data+1);
			Uint8 r=GETBYTE(Data+2);
			Uint8 g=GETBYTE(Data+3);
			Uint8 b=GETBYTE(Data+4);
			if(window==0 && font==0){
				w_textview->SetFontColor(r,g,b);
			}
		}
	}
	// Found colors stored as DWORDs in Cat Girl Alliance
	else if(Length==14){
		if(iop_check(14,IOP_SETFONTCOLOR,Data,Length)){
			Uint8 window=GETBYTE(Data+0);
			Uint8 font=GETBYTE(Data+1);
			int r=parser.DecodeValue(GETDWORD(Data+2));
			int g=parser.DecodeValue(GETDWORD(Data+6));
			int b=parser.DecodeValue(GETDWORD(Data+10));
			if(window==0 && font==0){
				w_textview->SetFontColor(r,g,b);
			}
		}
	}
	return false;
}

// Set font style
bool IkuraDecoder::iop_setfontstyle(const Uint8 *Data,int Length){
	if(iop_check(2,IOP_SETFONTSTYLE,Data,Length)){
		Uint8 window=GETBYTE(Data);
		Uint8 style=GETBYTE(Data+1);
		if(window==0){
			// Set native styles
			int ttfstyle=0;
			if(style&IKURA_FONTSTYLE_BOLD){
				ttfstyle|=TTF_STYLE_BOLD;
			}
			if(style&IKURA_FONTSTYLE_ITALIC){
				ttfstyle|=TTF_STYLE_BOLD;
			}
			w_textview->SetFontStyle(ttfstyle);

			// Enable effects
			SDL_Color c={0,0,0};
			if(style&IKURA_FONTSTYLE_MASK){
				w_textview->SetFontShadow(2,2,c);
			}
			if(style&IKURA_FONTSTYLE_SHADOW){
				w_textview->SetFontShadow(2,2,c);
			}
		}
	}
	return false;
}

// Set font size
bool IkuraDecoder::iop_pb(const Uint8 *Data,int Length){
	if(iop_check(5,IOP_PB,Data,Length)){
		Uint8 window=GETBYTE(Data);
		int size=parser.DecodeValue(GETDWORD(Data+1));
		if(window==0){
			w_textview->SetFontSize(size);
		}
	}
	return false;
}

// Set text size
bool IkuraDecoder::iop_gpb(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_GPB,Data,Length)){
		int size=parser.DecodeValue(GETDWORD(Data));
		w_textview->SetFontSize(size);
	}
	return false;
}

// Return from subroute
bool IkuraDecoder::iop_rt(const Uint8 *Data,int Length){
	parser.ReturnFunction();
	return false;
}

bool IkuraDecoder::iop_set(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_SET,Data,Length)){
		int channel=parser.DecodeValue(GETDWORD(Data));
		StopSound(channel);
	}
	return false;
}

bool IkuraDecoder::iop_sep(const Uint8 *Data,int Length){
	//int channel=parser.DecodeValue(GETDWORD(Data));
	//int tick=0;
	if(Length==8){
		// Extra duration parameter
		//tick=parser.DecodeValue(GETDWORD(Data+4));
	}
	return false;
}

// Play sound
bool IkuraDecoder::iop_ser(const Uint8 *Data,int Length){
	// A register with the channel to use is also passed,
	// but we dont deal with channels and ignore it.
	int channel=parser.DecodeValue(GETDWORD(Data+Length-4));
	PlaySound((char*)Data,VA_SOUNDS);
	return false;
}

// Initialize surfaces
bool IkuraDecoder::iop_vset(const Uint8 *Data,int Length){
	if(iop_check(12,IOP_VSET,Data,Length)){
		int dst=parser.DecodeValue(GETDWORD(Data+0));
		int w=parser.DecodeValue(GETDWORD(Data+4));
		int h=parser.DecodeValue(GETDWORD(Data+8));
		w_display->SetSurface(dst,w,h);
	}
	return false;
}

// Effects using grayscale (Not fully supported)
bool IkuraDecoder::iop_gge(const Uint8 *Data,int Length){
	//int type=parser.DecodeValue(GETDWORD(Data+0));
	int num=parser.DecodeValue(GETDWORD(Data+4));
	int tick=parser.DecodeValue(GETDWORD(Data+8));
	//int dir=parser.DecodeValue(GETDWORD(Data+12));
	//int len=parser.DecodeValue(GETDWORD(Data+16));
	//char *name=(char*)Data+20;

	// No support for effect files (yet)
	w_display->BlitSurface(num,0,0,0);
	SDL_Surface *surface=w_display->GetSurface(num);
	if(surface){
		SDL_Rect r={0,0,surface->w,surface->h};
		AddAnimation(new Fade(r,surface,r,tick));
		//SDL_FreeSurface(surface);
	}

	return true;
}

// Shaking effect
bool IkuraDecoder::iop_gv(const Uint8 *Data,int Length){
	if(iop_check(8,IOP_GV,Data,Length)){
		// Parse parameters
		Uint16 count=GETWORD(Data);
		Uint8 x=GETBYTE(Data+2);
		Uint8 y=GETBYTE(Data+3);
		int duration=parser.DecodeValue(GETDWORD(Data+4));

		// Shake surface
		SDL_Surface *source=w_display->GetSurface(0);
		SDL_Rect srect={0,0,NativeWidth(),NativeHeight()};
		SDL_Rect erect={x,y,srect.w,srect.h};
		for(int i=0;i<count;i++){
				AddAnimation(new Slide(source,srect,erect,duration/2));
		}
	}
	return true;
}

// Blitter
bool IkuraDecoder::iop_gp(const Uint8 *Data,int Length){
	// Parse values
	Uint8 cmd=GETBYTE(Data);
	int src=parser.DecodeValue(GETDWORD(Data+1));
	int srcx=parser.DecodeValue(GETDWORD(Data+5));
	int srcy=parser.DecodeValue(GETDWORD(Data+9));
	int srcw=parser.DecodeValue(GETDWORD(Data+13));
	int srch=parser.DecodeValue(GETDWORD(Data+17));
	int dst=parser.DecodeValue(GETDWORD(Data+21));
	int dstx=parser.DecodeValue(GETDWORD(Data+25));
	int dsty=parser.DecodeValue(GETDWORD(Data+29));
	int color=0;
	int ccut=0;

	// Parse optional parameters
	if(Length>36){
		color=parser.DecodeValue(GETDWORD(Data+33));
	}
	if(Length>40){
		ccut=parser.DecodeValue(GETDWORD(Data+37));
	}

	if(Length>41){
		LogError("ILLEGAL GP LENGTH:%d",Length);
	}

	if(dstx>0xFFFF)		dstx|=0xFFFF0000;
	if(dsty>0xFFFF)		dsty|=0xFFFF0000;
	if(srcx>0xFFFF)		srcx|=0xFFFF0000;
	if(srcy>0xFFFF)		srcy|=0xFFFF0000;
	if(srcw>0xFFFF)		srcw|=0xFFFF0000;
	if(srch>0xFFFF)		srch|=0xFFFF0000;

	// Convert values
	SDL_Rect srcr={srcx,srcy,srcw,srch};
	SDL_Rect dstr={dstx,dsty,srcw,srch};

	if(src>=0 && src<GBSIZE){
		// Commands with source specifier
		// 		cmd=0|20 is basic blits
		// 		cmd=1|21 is transparent blits
		if(cmd==0 || cmd==20){
			w_display->BlitSurface(src,&srcr,dst,&dstr);
		}
		else if(cmd==1 || cmd==21){
			w_display->BlendSurface(src,&srcr,dst,&dstr);
		}


		// Effects to copy things to display surface
		else if(cmd==15){
			// Some kind of vertical effect -- Destination is duration
			w_display->BlitSurface(src,&srcr,0,&dstr);
		}
		else if(cmd==16){
			// Some kind of horizontal effect -- Destination is duration
			w_display->BlitSurface(src,&srcr,0,&dstr);
		}
		else if(cmd==17){
			// Move effect -- Destination is duration
			w_display->BlitSurface(src,&srcr,0,&dstr);
		}
		else if(cmd==19 || cmd==7){
			// Fade effect -- Destination is duration
			w_display->BlendSurface(src,&srcr,0,&dstr);
			//LogError("Fade effect dst = %d!",dstr);

			SDL_Surface *surface=w_display->GetSurface(src);
			if(surface){

				AddAnimation(new Fade(dstr,surface,srcr,dst));
				//SDL_FreeSurface(surface);
			}
		}
		else{
			LogError("UNKNOWN GP OPCODE:%d",cmd);
			for(int i=0;i<Length;i++){
				LogError("0x%x",Data[i]);
			}
		}
	}
	else{
		LogError("BAD GP OPCODE:%d",cmd);
		for(int i=0;i<Length;i++){
			LogError("0x%x",Data[i]);
		}
	}

	return !Widget::PeekGlobalRefresh();
}

// Prepare display??
bool IkuraDecoder::iop_gn(const Uint8 *Data,int Length){
	if(iop_check(12,IOP_GN,Data,Length)){
	}
	return false;
}

// Clear graphics (Fill rect)
bool IkuraDecoder::iop_gc(const Uint8 *Data,int Length){
	if(iop_check(7,IOP_TIMERSET,Data,Length)){
		// Parse destination and colors
		Uint8 dst=Data[0];
		Uint8 par2=Data[1];
		Uint8 par3=Data[2];
		Uint8 par4=Data[3];
		Uint8 r=Data[4];
		Uint8 g=Data[5];
		Uint8 b=Data[6];
		if(par2+par3+par4){
			LogError("UNKNOWN PARAMETERS: %x  %x  %x",par2,par3,par4);
		}
		w_display->FillSurface(dst,r,g,b);
	}
	return false;
}

// Wait while fading in graphics
bool IkuraDecoder::iop_go(const Uint8 *Data,int Length){
	//LogError("iop_go!");
	int delay=parser.DecodeValue(GETDWORD(Data));
	Uint8 r=GETBYTE(Data+4);
	Uint8 g=GETBYTE(Data+5);
	Uint8 b=GETBYTE(Data+6);
	SDL_Surface *tmps=EDL_CreateSurface(NativeWidth(),NativeHeight());
	SDL_Rect src={0,0,tmps->w,tmps->h};
	EDL_SetBox(tmps,0,0,tmps->w,tmps->h,r,g,b,0xFF);
	//boxRGBA(EDLRenderer,0,0,tmps->w,tmps->h,r,g,b,0xFF);

	w_display->SetSurface(0,tmps);
	AddAnimation(new Fade(src,tmps,src,delay));
	SDL_FreeSurface(tmps);

	// Set waitstate
	if(state==IS_WAITTIMER){
		timerend+=delay;
	}
	else{
		timerend=SDL_GetTicks()+delay;
		state=IS_WAITTIMER;
	}
	return false;
}

// Open animation
bool IkuraDecoder::iop_gaopen(const Uint8 *Data,int Length){
	int v=parser.DecodeValue(GETDWORD(Data));
	char *filename=(char*)Data+4;
	//LogError("Loading animation: %d %s",v,filename);
	if(asurfaces){
		for(int i=0;asurfaces[i];i++){
			SDL_FreeSurface(asurfaces[i]);
			asurfaces[i]=0;
		}
		delete asurfaces;
		asurfaces=0;
	}
	asurfaces=LoadAnimation(filename);
	return false;
}

// Start animation
bool IkuraDecoder::iop_gastart(const Uint8 *Data,int Length){
	if(asurfaces){
		PlayAnimation(asurfaces);
        asurfaces=0;
	}
	return !(asurfaces);
}

// Load video file
bool IkuraDecoder::iop_avip(const Uint8 *Data,int Length){
	int srcw=parser.DecodeValue(GETDWORD(Data+0));
	int srch=parser.DecodeValue(GETDWORD(Data+4));
	int dstw=parser.DecodeValue(GETDWORD(Data+8));
	int dsth=parser.DecodeValue(GETDWORD(Data+12));
	LogError("Play video: %d %d %d %d",srcw,srch,dstw,dsth);

	PlayVideo((char*)Data+16);
	return true;
}

// Load graphics
bool IkuraDecoder::iop_gl(const Uint8 *Data,int Length){
	// Load image into the GDL buffers
	// Load image into the GDL buffers
	int index=parser.DecodeValue(GETDWORD(Data));
	uString name=(char*)Data+4;
	if (name=="NUKI")
	{
       // SDL_Surface *surface=LoadImage(name);
       // SDL_FreeSurface(surface);
        w_display->SetSurface(index,1,1);
	}
	else
	{
       SDL_Surface *surface=LoadImage(name);
        if(surface){
            w_display->SetSurface(index,surface);
            SDL_FreeSurface(surface);
        }
	}
	return false;
}

// Jump to ssoffset+jumptable[WORD]
bool IkuraDecoder::iop_jp(const Uint8 *Data,int Length){
	parser.JumpFunction(GETWORD(Data));
	return false;
}

// Jumpswitch
bool IkuraDecoder::iop_onjp(const Uint8 *Data,int Length){
	int reg=parser.DecodeValue(GETDWORD(Data));
	Uint8 cnt=GETBYTE(Data+4);

	// Check if jump is applicable
	if(reg>=(int)0 && reg<cnt){
		for(int i=5;i<Length;i+=2){
			if(!reg){
				parser.JumpFunction(GETWORD(Data+i));
				break;
			}
			else{
				reg--;
			}
		}
	}
	return false;
}

// Call subroutine ssoffset+jumptable[WORD]
bool IkuraDecoder::iop_js(const Uint8 *Data,int Length){
	Uint16 tableentry=GETWORD(Data);
	parser.CallFunction(tableentry);
	return false;
}

// Jumpswitch
bool IkuraDecoder::iop_onjs(const Uint8 *Data,int Length){
	int reg=parser.DecodeValue(GETDWORD(Data));
	Uint8 cnt=GETBYTE(Data+4);

	// Check if jump is applicable
	if(reg>=(int)0 && reg<cnt){
		for(int i=5;i<Length;i+=2){
			if(!reg){
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

bool IkuraDecoder::iop_exc(const Uint8 *Data,int Length){
	return false;
}

bool IkuraDecoder::iop_exs(const Uint8 *Data,int Length){
	return false;
}

// Start delay
bool IkuraDecoder::iop_await(const Uint8 *Data,int Length){

	if(state==IS_WAITTIMER){
		timerend+=delayvalue;
		//LogError("iop_await state==IS_WAITTIMER timerend = %d delayvalue = %d!",timerend,delayvalue);
	}
	else{
		timerend=SDL_GetTicks()+delayvalue;
		state=IS_WAITTIMER;
       // LogError("iop_await state=!IS_WAITTIMER timerend = %d delayvalue = %d!",timerend,delayvalue);
	}

	return true;
}

// Set delayvalue
bool IkuraDecoder::iop_atimes(const Uint8 *Data,int Length){

	if(iop_check(4,IOP_ATIMES,Data,Length)){
		delayvalue=parser.DecodeValue(GETDWORD(Data));
	}

	return false;
}

// Presets timerstart
bool IkuraDecoder::iop_timerset(const Uint8 *Data,int Length){
	if(iop_check(4,IOP_TIMERSET,Data,Length)){
		timerstart=SDL_GetTicks()+GETDWORD(Data);
		//LogError("iop_timerset timerstart = %d!",timerstart);
	}
	return false;
}

// Read timerstart
bool IkuraDecoder::iop_timerget(const Uint8 *Data,int Length){
	if(iop_check(2,IOP_TIMERGET,Data,Length)){
		Uint16 dst=GETWORD(Data);
		int timer=SDL_GetTicks()-timerstart;
		//LogError("iop_timerget timer = %d!",timer);
		if(timer<(int)0){
			timer=0;
		}
		parser.SetValue(dst,timer);
	}
	return true;
}

bool IkuraDecoder::iop_iginit(const Uint8 *Data,int Length){
	keyok=false;
	keycancel=false;
	return false;
}

bool IkuraDecoder::iop_igrelease(const Uint8 *Data,int Length){
	keyok=false;
	keycancel=false;
	return false;
}

// Set system flags (Message skip, automode, bgm disable, etc)
bool IkuraDecoder::iop_sts(const Uint8 *Data,int Length){
	Uint8 dst=Data[0];
	Uint8 val=Data[1];
	parser.SetSystem(dst,val);
	return false;
}

// Jump on system flags
//	cmd==0x00		Dialog type?
//	cmd==0x01		Message type, Voice=0, Text=1, Both=2
//	cmd==0x02		Message disable?
//	cmd==0x03		Voice disable?
//	cmd==0x04		SE disable?
//	cmd==0x05		BGM disable?
//	cmd==0x06		Voicetest disable?
//	cmd==0x07		SEtest disable?
//	cmd==0x08		Message skip? (Keyboard + mouse + setting)
//	cmd==0x09		Animation disable?
//	cmd==0x0a		Ctrl key status
//	cmd==0x0c		Automode
//	cmd==0x0d		Active window?
//	cmd==0x0e		Skip read
bool IkuraDecoder::iop_stx(const Uint8 *Data,int Length){
	// Parse parameters
	Uint8 cmd=GETBYTE(Data+0);
	Uint8 val=GETBYTE(Data+1);
	Uint16 entry=GETWORD(Data+2);
	bool jump=false;

	// Override internal system commands upon request
	if(cmd==0x08){
		// Message skip? (Keyboard + mouse + setting)
		if(keyok || keyctrl()){
			jump=true;
			keyok=false;
		}
	}
	else if(cmd==0x0A){
		// Control key status
		jump=keyctrl();
	}
	else if(cmd==0x0C){
		// Automode is not supported for now
		jump=false;
	}
	else if(cmd==0x0D){
		// Active window??
		//jump=false;
		if(val==1){
			jump=true;
		}
	}
	else{
		// Check value towards the stored setting
		if(parser.GetSystem(cmd)){
			jump=val;
		}
		else{
			jump=!val;
		}
	}

	// Execute jump if applicable
	if(jump){
		parser.JumpFunction(entry);
	}
	return false;
}

// Copy system variables
bool IkuraDecoder::iop_scp(const Uint8 *Data,int Length){
	if(iop_check(3,IOP_SCP,Data,Length)){
		Uint16 dst=GETWORD(Data);
		Uint8 cmd=GETBYTE(Data+2);
		if(cmd==0x0A){
			parser.SetValue(dst,keyctrl());
		}
		else{
			parser.SetValue(dst,parser.GetSystem(cmd));
		}
	}
	return false;
}

// Copy parameters to system variables
bool IkuraDecoder::iop_ssp(const Uint8 *Data,int Length){
	if(iop_check(3,IOP_SSP,Data,Length)){
		Uint16 val=GETWORD(Data);
		Uint8 dst=GETBYTE(Data+2);
		parser.SetSystem(dst,val);
	}
	return false;
}

bool IkuraDecoder::iop_ihgp(const Uint8 *Data,int Length){
	return false;
}

// Close existing hitmap?
bool IkuraDecoder::iop_ihgc(const Uint8 *Data,int Length){
	w_display->DropMap();
	return false;
}

// Load graphic rect
bool IkuraDecoder::iop_ihgl(const Uint8 *Data,int Length){
	SDL_Surface *hitmap=LoadImage((char*)Data);
	if(hitmap){
		w_display->SetMap(hitmap);
		SDL_FreeSurface(hitmap);
	}
	else{
		//LogError("Failed to load hitmap:%s",Data);
	}
	return hitmap;
}

// Configure keymap for keyboard input
bool IkuraDecoder::iop_ihk(const Uint8 *Data,int Length){
	Uint8 index=GETBYTE(Data);
	for(int i=1;i<Length;i+=4){
		w_display->SetKeyMap(index,(i-1)/4,parser.DecodeValue(GETDWORD(Data+i)));
	}
	return false;
}

// Set default keymap
bool IkuraDecoder::iop_ihkdef(const Uint8 *Data,int Length){
	w_display->SetKeyDefault(parser.DecodeValue(GETDWORD(Data)));
	return false;
}

// Defines screen areas (Kinda like the widgets)
bool IkuraDecoder::iop_ih(const Uint8 *Data,int Length){
	if(Length==23 || Length==31){
		Uint8 cmd=GETBYTE(Data);
		Uint32 x=GETDWORD(Data+1);
		Uint32 y=GETDWORD(Data+5);
		Uint32 w=GETDWORD(Data+9);
		Uint32 h=GETDWORD(Data+13);
		//Uint8 type=GETBYTE(Data+17);
		//Uint16 flag=GETWORD(Data+18);
		//Uint8 c2=GETBYTE(Data+20);
		//Uint8 c3=GETBYTE(Data+21);
		//Uint8 c4=GETBYTE(Data+22);

		SDL_Rect area={x,y,w-x,h-y};
		w_display->SetSpot(cmd,area);
	}
	else{
		LogError("FATAL: Unexpected IH opcode format");
	}
	return false;
}

/*
 * Example input bflag:
 *		Snow sakura:		1	(Left button+enter)
 *		Sagara family:		1	(Left button+enter)
 *		Sagara family:		0	(Left button+enter simple skip, right cancel)
 *		Crescendo:			0	(Left button+enter)
 *		Crescendo:			5	(Left button+enter)
 *		Crescendo:			4	(Left button+enter)
 *
 *
 *				1:		  001
 *				2:		  010
 *				4:		  100
 *				5:		  101
 */
bool IkuraDecoder::iop_ig(const Uint8 *Data,int Length){
	if(iop_check(6,IOP_IG,Data,Length)){
		// Parse parameters
		Uint16 cur=GETWORD(Data);
		Uint16 btn=GETWORD(Data+2);
		//Uint8 cnt=GETBYTE(Data+4);
		//Uint8 bflag=GETBYTE(Data+5);

		// Calculate results
		int h=w_display->GetSelected(false);
		Sint16 pos=h;
		Sint16 flag=0;
		if(h!=-1){
			// Check for hotspots input
			if(h==w_display->GetResult(false)){
				w_display->GetResult(true);
				flag=2;
			}
		}
		else{
			// Get ordinary clicks
			if(keycancel){
				keycancel=false;
				flag=4;
			}
			if(keyok){
				keyok=false;
				flag=2;
			}
		}

		// Store results
		//LogTest("IG: %d %d %d %d (%d %d)",cur,btn,cnt,bflag,pos,flag);
		parser.SetValue(cur,pos);
		parser.SetValue(btn,flag);
	}
	return false;
}

bool IkuraDecoder::iop_sdv(const Uint8 *Data,int Length){
   // w_textview->ClearText();
   // LogError("FATAL: iop_sdv");
	return false;
}

bool IkuraDecoder::iop_ed(const Uint8 *Data,int Length){
	EventGameDialog(VD_SHUTDOWN);
	return true;
}

