/*! \class DiviDead
 *	\brief Dividead implementation
 */

#ifndef _DIVIDEAD_H_
#define _DIVIDEAD_H_

#include "../cware.h"
#include "divigui.h"
#include "divicg.h"

class DiviDead : public EngineCWare {
	private:
		DiviGUI *divigui;
		DiviMenu *main;
		DiviMenu *extra;
	public:
		DiviDead(uString Path);

		// Menu interface
		bool ToggleMainmenu();
		bool ToggleExtramenu();
		void ClearMenu();

		// Overrides
		virtual void EventNew();
		virtual bool EventLoad(int Index);
		virtual void EventGameDialog(VN_DIALOGS Dialog);
		virtual bool EventBackgroundMouseRightUp(int X,int Y);
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif


