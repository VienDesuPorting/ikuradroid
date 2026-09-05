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

#include "mwidget.h"
#include "../engine/evn.h"

MusicWidget::MusicWidget(EngineVN *Engine,RWops *Music) : Animation() {
	engine=Engine;
	music=Music;
}

bool MusicWidget::Continue(){
	return music;
}

bool MusicWidget::Skip(){
	bool retval=music;
	if(music){
		if(engine){
			engine->PlayMusic(music);
		}
		delete music;
		music=0;
	}
	return retval;
}

void MusicWidget::Render(){
	if(music){
		if(engine){
			engine->PlayMusic(music);
		}
		delete music;
		music=0;
	}
}

