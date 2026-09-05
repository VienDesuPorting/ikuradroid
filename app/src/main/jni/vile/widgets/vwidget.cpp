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

#include "vwidget.h"
#include "../engine/evn.h"

VideoWidget::VideoWidget(EngineVN *Engine,RWops *Video) : Animation() {
	engine=Engine;
	video=Video;
}

bool VideoWidget::Continue(){
	return video;
}

bool VideoWidget::Skip(){
	bool retval=video;
	if(video){
		if(engine){
			engine->PlayVideo(video);
		}
		delete video;
		video=0;
	}
	return retval;
}

void VideoWidget::Render(){
	if(video){
		if(engine){
			engine->PlayVideo(video);
		}
		delete video;
		video=0;
	}
}

