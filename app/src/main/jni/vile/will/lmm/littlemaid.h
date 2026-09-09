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

/*! \class LittleMyMaid
 *	\brief Little My Maid implementation (Sweet Basil, 1999)
 *
 *  Will-engine game running on the CP-era script grammar (script_v2).
 *  Archives are ARC-8 like Critical Point: scripts in Rio.arc, graphics
 *  (WIP/MSK/ANM/TBL) in Chip.arc, audio in Bgm/Se/Voice.arc. The GUI
 *  click zones live in *.TBL tables referencing *.MSK zone masks, and
 *  menu/sprite animations are driven by fixed-size .ANM frame tables.
 */

#ifndef _LITTLEMAID_H_
#define _LITTLEMAID_H_

#include "../will.h"

class LittleMyMaid : public EngineWill {
	private:
	public:
		LittleMyMaid(uString Path);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
};

#endif

