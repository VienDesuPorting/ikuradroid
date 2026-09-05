/*! \class StdHalt
 *  \brief Standard dialog to confirm that the user wants to exit
 */
#ifndef _STDHALT_H_
#define _STDHALT_H_

#include "dlgbase.h"

class StdHalt : public DialogBase {
	private:
		// Flags wether dialog is already showning
		static bool showing;
	protected:
		// Dialog widgets
		Printer *w_title;
		BitmapButton *w_ok;
		BitmapButton *w_cancel;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		StdHalt(EngineVN *Engine);
		~StdHalt();
};

#endif

