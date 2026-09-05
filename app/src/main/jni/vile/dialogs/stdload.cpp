/*
 * ViLE - Visual Library Engine
 * Copyright (c) 2010-2011, ViLE Team (team@vilevn.org)
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

#include "stdload.h"
#include "../engine/evn.h"

StdLoad::StdLoad(EngineVN *Engine) : StdSave(Engine){
	// Change title
	w_title->Clear();
	w_title->Autosize("Load");
	w_title->Print("Load",0);
}

/*! \brief Override save event to load instead
 *  \param Index Integer for naming the game
 */
void StdLoad::Save(int Index){
	Load(Index);
}

/*! \brief Load game
 *  \param Index Integer for naming the game
 */
void StdLoad::Load(int Index){
	if(engine->EventLoad(Index)){
		engine->DestroyWidget(this);
		engine->SetTransition();
	}
}

