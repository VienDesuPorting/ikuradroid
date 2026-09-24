/*! \class IkuraDecoder
 *  \brief Engine core and decoder for Ikura GDL based games
 */
#ifndef _IKURADECODER_H_
#define _IKURADECODER_H_

//#include "ikuragui.h"
#include "../engine/evn.h"
#include "iparser.h"
#include "idisplay.h"

#define IKURA_FONTSTYLE_BOLD	0x01
#define IKURA_FONTSTYLE_ITALIC	0x02
#define IKURA_FONTSTYLE_MASK	0x03
#define IKURA_FONTSTYLE_SHADOW	0x04

enum IKURA_FONTSTYLE {
	IF_BOLD
};


// Ikura state machine
enum IKURA_STATE {
	IS_NORMAL,							//!< Normal opcode processing
	IS_WAITTIMER,						//!< Waiting for statetimer to time out
	IS_WAITTEXT,						//!< Waiting for text to print
	IS_WAITCLICK,						//!< Waiting for user input
	IS_WAITSELECT,						//!< Waiting for a command selection (CC cmd=0)
	IS_LOAD,							//!< Load dialog
	IS_SAVE,							//!< Save dialog
	IS_OPTIONS							//!< Options dialog
};

class IkuraDecoder : public EngineVN {
	protected:
        uString nameMusic;
		// Statemachine
		IKURA_STATE state;				//!< Current state
		IParser parser;

		// Timer
		Uint32 timerend;				//!< End of state timer
		Uint32 timerstart;				//!< Timer start (milliseconds)
		Uint32 delayvalue;				//!< Delay duration (milliseconds)

		// Selection data
		int selresult;					//!< Selection result (-1 = none)
		int selwait_dst;				//!< Register to store selection in while waiting
		SDL_Rect cmdrect;				//!< Command window geometry (CW opcode)

		// Gamedata
		Stringlist s_names;				//!< Character names (Newer games)
		SDL_Surface **asurfaces;		//!< Loaded animation

		// ISF Decrypt data
		char *isf_keybuffer;
		int isf_keysize;

		// Client widgets
		IkuraDisplay *w_display;		//!< Widget for ingame graphics
		Textview *w_textview;			//!< Textview control
		Selection *w_select;			//!< Selection widget

		// Override game events
		virtual bool EventGameTick();
		virtual bool EventGameProcess();
		virtual void EventSelect(int Selection);
		virtual bool EventSaveSystem();
		virtual bool EventLoadSystem();
		virtual bool EventSave(int Index);
		virtual bool EventLoad(int Index);
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventBackgroundMouseRightDown(int X,int Y);
		virtual bool EventBackgroundMouseLeftDown(int X,int Y);

		// Overridden opcode handlers
		virtual bool iop_wp(const Uint8 *Data,int Length);

		// Maps a CSET choice rectangle onto the screen. The base
		// implementation keeps the legacy y*3 scaling used by older
		// Ikura games; game modules may map rects relative to the
		// command window instead.
		virtual SDL_Rect MapChoiceRect(Uint32 X,Uint32 Y,
						Uint32 W,Uint32 H);
        virtual bool iop_pcend(const Uint8 *Data,int Length);

		// Opcode handlers
		bool iop_check(int ExpectedLength,const Uint8 Code,
				const Uint8 *Data,int Length);
		bool iop_unknown(const Uint8 Code,const Uint8 *Data,int Length);
		bool iop_calc(const Uint8 *Data,int Length);
		bool iop_lsbs(const Uint8 *Data,int Length);
		bool iop_sret(const Uint8 *Data,int Length);
		bool iop_ls(const Uint8 *Data,int Length);
		bool iop_title(const Uint8 *Data,int Length);
		bool iop_cns(const Uint8 *Data,int Length);
		bool iop_clk(const Uint8 *Data,int Length);
		bool iop_setgameinfo(const Uint8 *Data,int Length);
		bool iop_ws(const Uint8 *Data,int Length);
		bool iop_wss(const Uint8 *Data,int Length);
		bool iop_wsh(const Uint8 *Data,int Length);
		bool iop_scp(const Uint8 *Data,int Length);
		bool iop_ssp(const Uint8 *Data,int Length);
		bool iop_if(const Uint8 *Data,int Length);
		bool iop_im(const Uint8 *Data,int Length);
		bool iop_rt(const Uint8 *Data,int Length);
		bool iop_hdec(const Uint8 *Data,int Length);
		bool iop_hinc(const Uint8 *Data,int Length);
		bool iop_hsg(const Uint8 *Data,int Length);
		bool iop_hs(const Uint8 *Data,int Length);
		bool iop_ht(const Uint8 *Data,int Length);
		bool iop_hxp(const Uint8 *Data,int Length);
		bool iop_hp(const Uint8 *Data,int Length);
		bool iop_hf(const Uint8 *Data,int Length);
		bool iop_sp(const Uint8 *Data,int Length);
		bool iop_sk(const Uint8 *Data,int Length);
		bool iop_sks(const Uint8 *Data,int Length);
		bool iop_hn(const Uint8 *Data,int Length);
		bool iop_es(const Uint8 *Data,int Length);
		bool iop_ec(const Uint8 *Data,int Length);
		bool iop_ft(const Uint8 *Data,int Length);
		bool iop_ms(const Uint8 *Data,int Length);
		bool iop_ses(const Uint8 *Data,int Length);
		bool iop_pcmon(const Uint8 *Data,int Length);

		bool iop_pcms(const Uint8 *Data,int Length);
		bool iop_pcml(const Uint8 *Data,int Length);
		bool iop_cset(const Uint8 *Data,int Length);
		bool iop_cw(const Uint8 *Data,int Length);
		bool iop_cp(const Uint8 *Data,int Length);
		bool iop_cwo(const Uint8 *Data,int Length);
		bool iop_cwc(const Uint8 *Data,int Length);
		bool iop_wc(const Uint8 *Data,int Length);
		bool iop_wo(const Uint8 *Data,int Length);
		bool iop_cc(const Uint8 *Data,int Length);
		bool iop_ml(const Uint8 *Data,int Length);
		bool iop_gge(const Uint8 *Data,int Length);
		bool iop_vset(const Uint8 *Data,int Length);
		bool iop_gv(const Uint8 *Data,int Length);
		bool iop_gp(const Uint8 *Data,int Length);
		bool iop_set(const Uint8 *Data,int Length);
		bool iop_sep(const Uint8 *Data,int Length);
		bool iop_ser(const Uint8 *Data,int Length);
		bool iop_gaopen(const Uint8 *Data,int Length);
		bool iop_gastart(const Uint8 *Data,int Length);
		bool iop_avip(const Uint8 *Data,int Length);
		bool iop_gl(const Uint8 *Data,int Length);
		bool iop_go(const Uint8 *Data,int Length);
		bool iop_gn(const Uint8 *Data,int Length);
		bool iop_gc(const Uint8 *Data,int Length);
		bool iop_js(const Uint8 *Data,int Length);
		bool iop_ppf(const Uint8 *Data,int Length);
		bool iop_jp(const Uint8 *Data,int Length);
		bool iop_onjp(const Uint8 *Data,int Length);
		bool iop_onjs(const Uint8 *Data,int Length);
		bool iop_das(const Uint8 *Data,int Length);
		bool iop_dap(const Uint8 *Data,int Length);
        bool iop_pm(const Uint8 *Data,int Length);
		bool iop_pf(const Uint8 *Data,int Length);
		bool iop_exc(const Uint8 *Data,int Length);
		bool iop_exs(const Uint8 *Data,int Length);
		bool iop_sts(const Uint8 *Data,int Length);
		bool iop_stx(const Uint8 *Data,int Length);
		bool iop_timerset(const Uint8 *Data,int Length);
		bool iop_timerget(const Uint8 *Data,int Length);
		bool iop_atimes(const Uint8 *Data,int Length);
		bool iop_await(const Uint8 *Data,int Length);
		bool iop_iginit(const Uint8 *Data,int Length);
		bool iop_igrelease(const Uint8 *Data,int Length);
		bool iop_ig(const Uint8 *Data,int Length);
		bool iop_ih(const Uint8 *Data,int Length);
		bool iop_ihgp(const Uint8 *Data,int Length);
		bool iop_ihgc(const Uint8 *Data,int Length);
		bool iop_ihgl(const Uint8 *Data,int Length);
		bool iop_ihk(const Uint8 *Data,int Length);
		bool iop_ihkdef(const Uint8 *Data,int Length);
		bool iop_opsl(const Uint8 *Data,int Length);
		bool iop_svf(const Uint8 *Data,int Length);
		bool iop_udae(const Uint8 *Data,int Length);
		bool iop_udaf(const Uint8 *Data,int Length);
		bool iop_udda(const Uint8 *Data,int Length);
		bool iop_voc(const Uint8 *Data,int Length);
		bool iop_opprop(const Uint8 *Data,int Length);
		bool iop_enable(const Uint8 *Data,int Length);
		bool iop_disable(const Uint8 *Data,int Length);
		bool iop_sldrset(const Uint8 *Data,int Length);
		bool iop_cbset(const Uint8 *Data,int Length);
		bool iop_rbset(const Uint8 *Data,int Length);
		bool iop_tagset(const Uint8 *Data,int Length);
		bool iop_frameset(const Uint8 *Data,int Length);
		bool iop_setfontcolor(const Uint8 *Data,int Length);
		bool iop_setfontstyle(const Uint8 *Data,int Length);
		bool iop_pb(const Uint8 *Data,int Length);
		bool iop_gpb(const Uint8 *Data,int Length);
		bool iop_ed(const Uint8 *Data,int Length);
		bool iop_sdv(const Uint8 *Data,int Length);

	public:
		IkuraDecoder(int Width,int Height);
		~IkuraDecoder();
		void SetKey(char *Buffer,int Size);
		void DecodeScript(Uint8 *Buffer,int Size);
		bool RunScript(uString Name);
};

#endif

