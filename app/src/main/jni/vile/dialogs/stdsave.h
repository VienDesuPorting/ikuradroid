/*! \class StdSave
 *  \brief Standard (ie. simple!) save dialog
 */
#ifndef _STDSAVE_H_
#define _STDSAVE_H_

#include "dlgbase.h"

class StdSave : public DialogBase {
	protected:
		// Dialog widgets
		SaveSlab *w_slabs[8];
		TextButton *w_pages[5];
		TextButton *w_exit;
		Printer *w_title;
		int page;

		// Override events
		virtual bool InputOk(Widget *Object);
		virtual bool FocusEnter(Widget *Object);
		virtual bool FocusLeave(Widget *Object);
	public:
		StdSave(EngineVN *Engine);
		~StdSave();

		// Autonomous events
		void SetCaption(uString Caption);
		virtual void SetPage(int Page);
		virtual void Save(int Index);
};

#endif

