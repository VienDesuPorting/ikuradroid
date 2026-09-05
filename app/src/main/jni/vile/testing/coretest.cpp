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

#include "coretest.h"

CoreTest::CoreTest(){
	// Test stringlist objects
	Header("Core macros");
	Assert(EDL_ABS(-1)==1,"ABS gives absolute on negative integers");
	Assert(EDL_ABS(1)==1,"ABS gives absolute on positive integers");
	Assert(EDL_ABS(0)==0,"ABS gives null on null integer");
	Assert(EDL_ABS((double)-1)==1,"ABS gives absolute on negative floats");
	Assert(EDL_ABS((double)1)==1,"ABS gives absolute on positive floats");
	Assert(EDL_ABS((double)0)==0,"ABS gives null on null float");
	Assert(EDL_MAX(0,1)==1,"MAX picks largest positive number");
	Assert(EDL_MAX(1,1)==1,"MAX picks among equals");
	Assert(EDL_MAX(1,-1)==1,"MAX picks positives over negatives");
	Assert(EDL_MAX(0,-1)==0,"MAX picks null over negatives");
	Assert(EDL_MIN(0,1)==0,"MIN picks smallest positive number");
	Assert(EDL_MIN(1,1)==1,"MIN picks among equals");
	Assert(EDL_MIN(1,-1)==-1,"MIN picks negatives over posives");
	Assert(EDL_MIN(0,-1)==-1,"MAX picks negatives over null");
	Assert(EDL_LIMIT(0,1,2)==1,"LIMIT floors values correctly");
	Assert(EDL_LIMIT(3,1,2)==2,"LIMIT limits values correctly");
	Assert(EDL_LIMIT(1,1,2)==1,"LIMIT accepts lower limit");
	Assert(EDL_LIMIT(2,1,2)==2,"LIMIT accepts upper limit");
	Assert(EDL_LIMIT(2,1,3)==2,"LIMIT accepts range");

	Header("Random number generator");
	int nulls=0;
	int ones=0;
	int max=0;
	int min=2;
	for(int i=0;i<1000;i++){
		int seed=EDL_Random(0,1);
		if(seed>max){
			max=seed;
		}
		if(seed<min){
			min=seed;
		}
		if(seed==0){
			nulls++;
		}
		if(seed==1){
			ones++;
		}
	}
	Assert(max<2,"Upper range is honoured when really narrow");
	Assert(min>=0,"Lower range is honoured when really narrow");
	Assert(nulls+ones==1000,"Correct number of narrowly ranged hits");
	Assert(nulls>100,"A reasonable number if nulls on narrow range");
	Assert(ones>100,"A reasonable number if ones on narrow range");
	nulls=0;
	ones=0;
	max=0;
	min=1001;
	for(int i=0;i<1000;i++){
		int seed=EDL_Random(0,1000);
		if(seed>max){
			max=seed;
		}
		if(seed<min){
			min=seed;
		}
		if(seed>=0&&seed<=1000){
			ones++;
		}
		if(seed>=0&&seed<=500){
			nulls++;
		}

	}
	Assert(max<1001,"Upper range is honoured when wide");
	Assert(min>=0,"Lower range is honoured when wide");
	Assert(ones==1000,"Correct number of widely ranged hits");
	Assert(nulls>100,"A reasonable number of lows on narrow range");
	Assert(nulls<900,"A reasonable number of highs on narrow range");


	Header("Core Objects");
	Stringlist sl1;
	Assert(sl1.GetCount()==0,"New stringlist objects are empty");
	sl1.SetString(0,"1");
	Assert(sl1.GetCount()==1,"Adding null indexed data gives single string");
	sl1.SetString(0,"1");
	Assert(sl1.GetCount()==1,"Changing null indexed data does not add data");
	for(int i=1;i<1000;i++){
		sl1.SetString(i,"Text");
	}
	Assert(sl1.GetCount()==1000,"Stringlist handles 1000 items unprepared");
	Stringlist sl2(1000);
	Assert(sl2.GetCount()==1000,"Stringlist can preload 1000 items");
	for(int i=0;i<1000;i++){
		sl2.SetString(i,"Text");
	}
	Assert(sl2.GetCount()==1000,"Preloaded items can be modified");
	uString s;
	Assert(sl2.GetString(500,&s),"Strings can be loaded");
	Assert(s=="Text","Loaded strings export as expected");
	Stringlist sl3(sl2);
	Assert(sl3.GetCount()==1000,"Copyconstructor copies loaded strings");
	Assert(sl3.GetString(500,&s),"Constructor-copied strings can be loaded");
	Assert(s=="Text","Contructor-copied strings export as expected");
	Stringlist sl4;
	Stringlist sl5;
	Assert(sl4.GetCount()==0,"New stringlist objects are still empty");
	sl4=sl3;
	Assert(sl4.GetCount()==1000,"Stringlists copy operator works");
	sl4=sl5;
	Assert(sl4.GetCount()==0,"Stringlists copy operator takes empty lists");
	Assert(!sl4.GetString(500,&s),"Empty strings not available");
	Assert(sl3.GetCount()==1000,"Original list maintains own copy");
	Assert(sl3.GetString(500,&s),"Original strings can be loaded");
	Assert(s=="Text","Original strings export as expected");

	// Vector objects
	DVector v1;
	Assert(v1.GetUint32(0)==0,"Empty Uint32 vector items returns null");
	Assert(v1.GetUint16(0)==0,"Empty Uint16 vector items returns null");
	Assert(v1.GetUint8(0)==0,"Empty Uint8 vector items returns null");
	Assert(v1.GetSint32(0)==0,"Empty Sint32 vector items returns null");
	Assert(v1.GetSint16(0)==0,"Empty Sint16 vector items returns null");
	Assert(v1.GetSint8(0)==0,"Empty Sint8 vector items returns null");
	v1.SetUint32(0,1);
	Assert(v1.GetUint32(0)==1,"Uint32 vector items register");
	v1.SetUint16(0,2);
	Assert(v1.GetUint16(0)==2,"Uint16 vector items register");
	v1.SetUint8(0,3);
	Assert(v1.GetUint8(0)==3,"Uint8 vector items register");
	v1.SetSint32(0,-1);
	Assert(v1.GetSint32(0)==-1,"Sint32 vector items register");
	v1.SetSint16(0,-2000);
	Assert(v1.GetSint16(0)==-2000,"Sint16 vector items register");
	v1.SetSint32(0,10);
	Assert(v1.GetSint8(0)==10,"Sint8 vector items register");
	for(int i=0;i<1000;i++){
		v1.SetUint32(i,i);
	}
	Assert(v1.GetUint32(999)==999,"Vector handles 1000 items unprepared");
	bool test=true;
	for(unsigned int i=0;i<1000;i++){
		test&=v1.GetUint32(i)==i;
	}
	Assert(test,"Vector items readtest without preloading");
	DVector v2(1000);
	for(int i=0;i<1000;i++){
		v2.SetUint32(i,i);
	}
	Assert(v2.GetUint32(999)==999,"Vector handles 1000 items prepared");
	test=true;
	for(unsigned int i=0;i<1000;i++){
		test&=v2.GetUint32(i)==i;
	}
	Assert(test,"Vector items readtest with preloading");
	DVector v3=v2;
	Assert(v3.GetUint32(999)==999,"Vector copy operator handles 1000 items");
	test=true;
	for(unsigned int i=0;i<1000;i++){
		test&=v3.GetUint32(i)==i;
	}
	Assert(test,"Vector items readtest for copied items");

	// Test stack objects
	DStack s1;
	Assert(s1.Count()==0,"New stack objects are empty");
	Assert(s1.Pop()==0,"Empty stack objects cant pop");
	Assert(s1.Peek()==0,"Empty stack objects cant peek");
	Assert(s1.Drop(0)==0,"Empty stack objects cant drop");
	Assert(s1.Get(0)==0,"Empty stack objects dont have null indexed item");
	Assert(s1.Get(100)==0,"Empty stack objects dont have items");
	int i1=99;
	s1.Queue(&i1);
	Assert(s1.Get(0)==&i1,"Empty stack objects can queue items");
	Assert(s1.Get(0)==&i1,"Read items are not popped");
	Assert(s1.Pop()==&i1,"Queued items can be popped");
	Assert(s1.Get(0)==0,"Popped items dissapears");
	s1.Push(&i1);
	Assert(s1.Get(0)==&i1,"Empty stack objects can push items");
	Assert(s1.Peek()==&i1,"Pushed items can be peeked");
	Assert(s1.Peek()==&i1,"Peeked objects remains on the stack");
	Assert(s1.Pop()==&i1,"Pushed items can be popped");
	Assert(s1.Get(0)==0,"Popped items dissapears");

	// Sum up
	Report();
}
