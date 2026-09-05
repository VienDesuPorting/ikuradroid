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

#include "miscbench.h"

MiscBench::MiscBench(int Runs){
	// Test string handling
	Header("String handling");
	Start("Loading empty %d string objects",Runs);
	for(int i=0;i<Runs;i++){
		uString s="";
	}
	Stop(Runs);
	Start("Loading %d string objects using literal",Runs);
	for(int i=0;i<Runs;i++){
		uString s="Testing string loading";
	}
	Stop(Runs);
	Start("Loading %d string objects using string object",Runs);
	uString s1="Testing string loading";
	for(int i=0;i<Runs;i++){
		uString s=s1;
	}
	Stop(Runs);
	Report();
}


