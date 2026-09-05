#ifndef _JUMC_H_
#define _JUMC_H_

#include "jast.h"
#include "jumcmain.h"

// Declare game titles
enum JUMC_TITLE {		//!< Enumerates game titles
	JT_COMMON,			//!< Common part
	JT_SOS,				//!< Season of Sakura
	JT_3SIS,			//!< 3 Sisters story
	JT_RUN,				//!< Runaway city
};


class JUMC : public EngineJAST {
	private:
		// Identifiers
		virtual const uString NativeID();
		virtual const uString NativeName();

		// Override events
		virtual void EventGameDialog(VN_DIALOGS Dialog);

		// Widgets
		JUMCMain *main;
		JUMC_TITLE title;
	public:
		JUMC(uString Path);
		~JUMC();
		bool StartGame(JUMC_TITLE Title);
};

#endif

