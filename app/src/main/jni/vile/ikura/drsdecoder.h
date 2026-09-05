/*! \class DRSDecoder
 *  \brief Engine decoder for Digital Romance System based games
 *
 *  The Digital Romance System is the predecessor of IkuraGDL so the two 
 *  shares many methods and most formats. The DRS system is therefore 
 *  implemented as an override to Ikura.
 */
#ifndef _DRSDECODER_H_
#define _DRSDECODER_H_

#include "ikuradecoder.h"

class DRSDecoder : public IkuraDecoder {
	protected:
		// Override ikura interpreter
		virtual bool EventGameProcess();

		// drs opcode handlers
		inline bool OP02(const Uint8 *Data,int Length);		// Call script
		inline bool OP03(const Uint8 *Data,int Length);		// Return from call
		inline bool OP05(const Uint8 *Data,int Length);		// Call subroutine
		inline bool OP06(const Uint8 *Data,int Length);		// Return from sub
		inline bool OP07(const Uint8 *Data,int Length);		// Jumpswitch
		inline bool OP08(const Uint8 *Data,int Length);		// Callswitch
		inline bool OP0C(const Uint8 *Data,int Length);		// Read lines flags
		inline bool OP0D(const Uint8 *Data,int Length);		// Read lines flags
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
		inline bool OPF3(const Uint8 *Data,int Length);		// Play Video
		inline bool OPF8(const Uint8 *Data,int Length);		// Enable save
		inline bool OPF9(const Uint8 *Data,int Length);		// Disable save
		inline bool OPFA(const Uint8 *Data,int Length);		// Reset save
	public:
		DRSDecoder(int Width,int Height);
		~DRSDecoder();
};

#endif

