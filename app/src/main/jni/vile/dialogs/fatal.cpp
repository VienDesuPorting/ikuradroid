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

#include "fatal.h"
#include "../engine/evn.h"

Fatal::Fatal(EngineVN *Engine,uString Title,uString Message) 
		: StdMessage(Engine,Title,Message){
}

Fatal::~Fatal(){
	DestroyWidgets();
}

bool Fatal::InputOk(Widget *Object){
	bool retval=false;
	if(Object==w_ok){
		// Close dialog
		engine->DestroyWidget(this);
		engine->EventGameShutdown();
		retval=true;
	}
	return retval;
}

