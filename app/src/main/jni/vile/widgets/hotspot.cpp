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

#include "hotspot.h"

Hotspot::Hotspot(int X,int Y,int Width,int Height) : Widget(X,Y,Width,Height){
	fhittable=true;
}

Hotspot::Hotspot(SDL_Rect Dst) : Widget(Dst){
	fhittable=true;
}

Hotspot::Hotspot() : Widget(){
	fhittable=true;
}

