#ifndef _DIVICG_H_
#define _DIVICG_H_

#include "../../widgets/hotspot.h"

class DiviDead;

class DiviCG : public Hotspot {
	private:
		int pageindex;				//!< Pageindex
		bool overview;				//!< Wether the overview is showing
		DiviDead *game;				//!< Holds pointer to engine
		void loadpage(int Index);
	public:
		DiviCG(DiviDead *Engine);
		virtual bool MouseLeftDown(int X,int Y);
};

#endif

