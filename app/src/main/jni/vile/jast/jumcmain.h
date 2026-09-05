#ifndef _JUMCMAIN_H_
#define _JUMCMAIN_H_

#include "../dialogs/selection.h"

class JUMC;

enum JUMC_ITEMS {
	JI_RUNAWAY_NEW,
	JI_RUNAWAY_LOAD,
	JI_RUNAWAY_CG,
	JI_SISTERS_NEW,
	JI_SISTERS_LOAD,
	JI_SISTERS_CG,
	JI_SAKURA_NEW,
	JI_SAKURA_LOAD,
	JI_SAKURA_CG,
	JI_SYSTEM_CG,
	JI_SYSTEM_MUSIC,
	JI_SYSTEM_SETUP,
	JI_SYSTEM_EXIT
};

class JUMCMain : public DialogBase {
	private:
		// Dialog widgets
		Selection *d_runaway;
		Selection *d_sisters;
		Selection *d_sakura;
		Selection *d_system;

		// Override events
		virtual bool InputOk(Widget *Object);
		virtual bool FocusEnter(Widget *Object);
		virtual bool FocusLeave(Widget *Object);
	public:
		JUMCMain(JUMC *Engine);
};

#endif

