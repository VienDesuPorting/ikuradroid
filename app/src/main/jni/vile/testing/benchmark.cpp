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

#include "benchmark.h"

Uint32 Benchmark::total_time=0;
Uint32 Benchmark::total_runs=0;

Benchmark::Benchmark() : UnitTest() {
	time=0;
	runs=0;
}

void Benchmark::Start(uString Text,...){
	// Rebuild input string
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);
	LogRaw(LLMESSAGE,"|  %-50s",Text.c_str());

	// Register start
	start=SDL_GetTicks();
}

Uint32 Benchmark::Stop(int Runs){
	Uint32 delta=SDL_GetTicks()-start;
	LogRaw(LLMESSAGE,"% 8dmS % 8dmS |\r\n",delta,delta/Runs);
	total_time+=delta;
	total_runs+=Runs;
	time+=delta;
	runs+=Runs;
	return delta;
}

void Benchmark::Summary(){
}

void Benchmark::Report(){
	if(runs){
		uString dash;
		for(int i=0;i<76;i++)	dash+="-";
		LogMessage(dash);
		Uint32 old_time=total_time;
		Uint32 old_runs=total_runs;
		Start("Runtime statistics");
		start=start-time;
		Stop(runs);
		total_time=old_time;
		total_runs=old_runs;
		time=0;
		runs=0;
		LogMessage(dash);
	}
}

