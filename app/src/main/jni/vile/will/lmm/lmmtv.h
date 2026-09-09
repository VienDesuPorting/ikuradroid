/*
 * IkuraDroid - Little My Maid engine module
 * Copyright (c) 2026, VienDesu! Porting Team
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _LMMTV_H_
#define _LMMTV_H_

#include "../../dialogs/textview.h"

#define LMMTV_BACKGROUND			0
#define LMMTV_HEADER							25
#define LMMTV_POINTS_BAR					15	//!< POINTS gauge fill (252x14 @ 381,4)

class LittleMyMaid;

class LMMTextview : public Textview {
	private:
		Printer *header;
		Printer *select; //!< "SELECT" caption shown above choices
		Widget *bar; //!< POINTS gauge fill sprite (winbase0 frame 15)
	public:
		LMMTextview(LittleMyMaid *Engine);
		~LMMTextview();

		void PrintText(uString Title,uString Text);
		void PrintText(uString Text);
		void PrintSelectTitle();
		void HideSelectTitle();
};

#endif

