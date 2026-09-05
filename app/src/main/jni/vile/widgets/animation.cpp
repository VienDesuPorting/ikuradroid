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

#include "animation.h"

Animation::Animation(int X,int Y,int Width,int Height) 
		: Widget(X,Y,Width,Height){
	fhittable=true;
}

Animation::Animation(SDL_Rect Dst) : Widget(Dst){
	fhittable=true;
}

Animation::Animation() : Widget(){
	fhittable=true;
}

/*! \brief Tells wether animation should continue
 *  \return True if animation is running
 */
bool Animation::Continue(){
	return false;
}

/*! \brief Skips or completes current animations
 *  \returns True if an animation was skipped
 *
 *  The base implementation of this method will skip the current animation
 *  and move onto the next, derivated animation widgets will finish what its
 *  doing.
 */
bool Animation::Skip(){
	return false;
}


