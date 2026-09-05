/*! \class Whisper
 *  \brief Indexes voice resources for Windy games
 */
#ifndef _WHISPER_H_
#define _WHISPER_H_

#include "../engine/evn.h"

class Whisper {
	private:
		// Store voices
		struct WhisperItem {
			uString name;
			Uint16 address;
			WhisperItem *next;
		} *head;
	protected:
		// Keep track of resources
		Resources scripts;
		EngineVN *engine;

		// Adjusts queue
		void DropVoice(int Position);
		void PushVoice(int Position);
	public:
		Whisper(EngineVN *Engine);
		virtual ~Whisper();

		// Syncronize with new scripts
		virtual void Synchronize(uString Name,Stringlist List)=0;

		// Add external resources
		void AddResource(ArchiveBase *Archive);

		// Manage indexed voice resources
		uString GetVoice(Uint16 Address);
		void SetVoice(Uint16 Address,uString Name);
		void ClearVoices();
};

#endif

