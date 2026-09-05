/*! \class Fatal
 *  \brief Presents a message before shutting down the application
 */
#ifndef _FATAL_H_
#define _FATAL_H_

#include "stdmsg.h"

class Fatal : public StdMessage {
	private:
		// Override events
		virtual bool InputOk(Widget *Object);
	public:
		Fatal(EngineVN *Engine,uString Title,uString Message);
		~Fatal();
};

#endif

