/*! \class MCWhisper
 *  \brief Suffleur class for mayclub remake
 */
#ifndef _MCWHISPER_H_
#define _MCWHISPER_H_

#include "../whisper.h"

class MCWhisper : public Whisper {
	private:
		// Buffer for english legacy script
		uString datname;
		Uint8 *datbuffer;
		int datsize;

		// Buffer for head s21 script
		Uint8 *pakbuffer;
		int paksize;
		int pakindex;

		// Buffer for current s21 script
		Uint8 *s21buffer;
		int s21size;
		int s21index;

		// Internal processing mechanisms
		void ProcessScript(Uint16 Index);
		void ProcessStub(Uint16 Start,Uint16 End);
		bool ParseVoice(Uint16 Start);
		void ParseScript();
	public:
		MCWhisper(EngineVN *Engine);
		virtual void Synchronize(uString Name,Stringlist List);
};

#endif

