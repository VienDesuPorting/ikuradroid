/*! \class StdLoad
 *  \brief Standard (ie. simple!) save dialog
 */
#ifndef _STDLOAD_H_
#define _STDLOAD_H_

#include "stdsave.h"

class StdLoad : public StdSave {
	private:
		virtual void Save(int Index);
	public:
		StdLoad(EngineVN *Engine);
		virtual void Load(int Index);
};

#endif

