/*! \class StdMessage
 *  \brief Presents a message
 */
#ifndef _STDMSG_H_
#define _STDMSG_H_

#include "dlgbase.h"

class StdMessage : public DialogBase {
	protected:
		// Dialog widgets
		Printer *w_title;
		Printer *w_message;
		Widget *w_ok;

		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		StdMessage(EngineVN *Engine,uString Title,uString Message);
		~StdMessage();
};

#endif

