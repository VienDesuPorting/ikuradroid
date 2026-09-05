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

#include "utest.h"

Uint32 UnitTest::total_passed=0;
Uint32 UnitTest::total_failed=0;
Uint32 UnitTest::total_time=0;

UnitTest::UnitTest(){
	Reset();
	LogMessage("");
}

void UnitTest::Reset(){
	passed=failed=0;
	start=SDL_GetTicks();
	round=start;
}

uString UnitTest::RebuildString(uString String,va_list Arg){
	vsnprintf(buffer,UNIT_BUFFER_SIZE,String.c_str(),Arg);
	buffer[UNIT_BUFFER_SIZE-1]=0;
	return buffer;
}

void UnitTest::Summary(){
	UnitTest test;
	test.Header("Summary");
	test.Assert(total_passed+total_failed>0,"Test suite executed");
	test.Assert(total_failed==0,"All tests completed without errors");
	test.Assert(total_time<1000,"No test took too long");
	test.passed=total_passed;
	test.failed=total_failed;
	test.start=SDL_GetTicks()-total_passed;
	test.Report();
}

void UnitTest::Report(){
	// Create header
	uString dash;
	Uint32 t=SDL_GetTicks()-start;
	total_passed+=passed;
	total_failed+=failed;
	total_time+=t;
	for(int i=0;i<76;i++)	dash+="-";
	LogMessage(dash);
	LogMessage("| Passed tests %59d |",passed);
	LogMessage("| Failed tests %59d |",failed);
	LogMessage("| Total number of tests %50d |",passed+failed);
	LogMessage("| Passed %64d% |",(int)(passed/(double)(passed+failed)*100));
	LogMessage("| Execution time %54d mS |",t);
	LogMessage(dash);
	LogMessage("");
	LogMessage("");
}

void UnitTest::Header(uString Text,...){
	// Rebuild input string
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Create header
	uString dash;
	for(int i=0;i<76;i++)	dash+="-";
	LogMessage(dash);
	LogMessage("|  %-71s |",Text.c_str());
	LogMessage(dash);
}

void UnitTest::Assert(bool Test,uString Text,...){
	// Rebuild textstring
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Check test
	uString result;
	if(Test){
		result="PASS";
		passed++;
	}
	else{
		result="FAIL";
		failed++;
	}

	// Format report entry
	sprintf(buffer,"| % 4d %-53s % 5dmS  %s |",
			passed+failed,
			Text.c_str(),
			SDL_GetTicks()-round,
			result.c_str());
	LogMessage(buffer);
	round=SDL_GetTicks();
}

