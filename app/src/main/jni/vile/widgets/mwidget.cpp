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
/*
 *  IkuraDroid modification notice - 2026
 *
 *  This file is modified for IkuraDroid (su.viende.ikuradroid) - the
 *  ViLE-based Android visual novel player - and differs from the
 *  original ViLE 0.4.13 source.
 *
 *  Base: ViLE 0.4.13 by the ViLE Team <team@vilevn.org>. The Android
 *  port continues the original VILE for Android (2015-2016) and is
 *  modified for IkuraDroid by the VienDesu! Porting Team
 *  <github.com/VienDesuPorting/ikuradroid>.
 *
 *  Distributed under the same GNU GPL (version 3) as the original
 *  ViLE; this notice supplements the original copyright.
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

