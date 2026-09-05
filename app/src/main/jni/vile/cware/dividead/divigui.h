/*! \class DiviGUI
 *  \brief Provides buttons to open up the menu interface
 */
#ifndef _DIVIGUI_H_
#define _DIVIGUI_H_

#include "../../dialogs/dlgbase.h"
#include "divimenu.h"

class DiviGUI : public DialogBase {
	private:
		// Remember objects
		ValueButton *mainmenu;
		ValueButton *extramenu;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		DiviGUI(DiviDead *Engine);
		void Clear();
};

#endif

