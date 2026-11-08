/*! \class DRSDecoder
 *  \brief Engine decoder for Digital Romance System based games
 *
 *  The Digital Romance System is the predecessor of IkuraGDL so the two 
 *  shares many methods and most formats. The DRS system is therefore 
 *  implemented as an override to Ikura.
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
#ifndef _DRSDECODER_H_
#define _DRSDECODER_H_

#include "ikuradecoder.h"

class DRSDecoder : public IkuraDecoder {
	protected:
		// Override ikura interpreter
		virtual bool EventGameProcess();

		// DRS specific wait states (typed text lines + page breaks)
		virtual bool EventGameTick();

		// drs opcode handlers
		inline bool OP02(const Uint8 *Data,int Length);		// Call script
		inline bool OP03(const Uint8 *Data,int Length);		// Return from call
		inline bool OP05(const Uint8 *Data,int Length);		// Call subroutine
		inline bool OP06(const Uint8 *Data,int Length);		// Return from sub
		inline bool OP07(const Uint8 *Data,int Length);		// Jumpswitch
		inline bool OP08(const Uint8 *Data,int Length);		// Callswitch
		inline bool OP0C(const Uint8 *Data,int Length);		// Read lines flags
		inline bool OP0D(const Uint8 *Data,int Length);		// Page break (waits for click)
		inline bool OP0E(const Uint8 *Data,int Length);		// Extended flags
		inline bool OP11(const Uint8 *Data,int Length);		// Set font size
		inline bool OP15(const Uint8 *Data,int Length);		// Open window
		inline bool OP20(const Uint8 *Data,int Length);		// Window position
		inline bool OP21(const Uint8 *Data,int Length);		// Window graphics
		inline bool OP2A(const Uint8 *Data,int Length);		// Show window
		inline bool OP2B(const Uint8 *Data,int Length);		// Hide window
		inline bool OP2C(const Uint8 *Data,int Length);		// Print text
		inline bool OP3C(const Uint8 *Data,int Length);		// CG flags
		inline bool OP3D(const Uint8 *Data,int Length);		// CG flags
		inline bool OP42(const Uint8 *Data,int Length);		// Set variables
		inline bool OP46(const Uint8 *Data,int Length);		// Calculation	
		inline bool OP47(const Uint8 *Data,int Length);		// If
		inline bool OP54(const Uint8 *Data,int Length);		// Fade effect
		inline bool OP55(const Uint8 *Data,int Length);		// Load image
		inline bool OP77(const Uint8 *Data,int Length);		// Play SE
		inline bool OP79(const Uint8 *Data,int Length);		// Play CDDA
		inline bool iop_ic(const Uint8 *Data,int Length);		// Change cursor
		inline bool OP85(const Uint8 *Data,int Length);		// Get input
		inline bool OPF3(const Uint8 *Data,int Length);		// Play Video
		inline bool OPF8(const Uint8 *Data,int Length);		// Enable save
		inline bool OPF9(const Uint8 *Data,int Length);		// Disable save
		inline bool OPFA(const Uint8 *Data,int Length);		// Reset save

		// CDDA state (the title screen loop re-issues OP79 every
		// cycle, so the handler must not rescan the archives or
		// restart the track on every poll)
		int cdda_track;						//!< Last requested CDDA track (-1 = none)
		bool cdda_failed;					//!< Last lookup found no playable source

		// Input polling state (OP85 idle detection)
		int ig_lastpos;
		int ig_lastflag;
	public:
		DRSDecoder(int Width,int Height);
		~DRSDecoder();
};

#endif

