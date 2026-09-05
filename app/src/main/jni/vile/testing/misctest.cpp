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

#include "misctest.h"

MiscTest::MiscTest(){
	// Test string handling
	Header("String handling");
	aString a1="Test";
	aString a2="";
	Assert(a1.compare("Test")==0,"ANSI string reference test I");
	Assert(a1.compare("")>0,"ANSI string reference test II");
	Assert(a1.compare("test")<0,"ANSI string reference test III");
	Assert(a1.compare("t")<0,"ANSI string reference test IV");
	Assert(a1.compare("Testx")<0,"ANSI string reference test V");
	Assert(a1.compare("testx")<0,"ANSI string reference test VI");
	Assert(a1.compare("T")>0,"ANSI string reference test VII");
	Assert(a1.compare("Tes")>0,"ANSI string reference test VIII");
	Assert(a1.compare(" ")>0,"ANSI string reference test IX");
	Assert(a1.compare("5")>0,"ANSI string reference test X");
	Assert(a2.compare("")==0,"ANSI string reference test XI");
	Assert(a2.compare("Test")<0,"ANSI string reference test XII");
	Assert(a2.compare("t")<0,"ANSI string reference test XIII");
	Assert(a2.compare(" ")<0,"ANSI string reference test XIV");
	Assert(a2.compare("1")<0,"ANSI string reference test XV");
	Assert(a2.compare("testx")<0,"ANSI string reference test XVI");
	Assert(a2.compare("T")<0,"ANSI string reference test XVII");
	Assert(a2.compare("Tes")<0,"ANSI string reference test XVIII");

#ifdef VILE_BUILD_UNICODE
	wString w1=L"Test";
	wString w2=L"";
	Assert(w1.compare(L"Test")==0,"Wide string reference test I");
	Assert(w1.compare(L"")>0,"Wide string reference test II");
	Assert(w1.compare(L"test")<0,"Wide string reference test III");
	Assert(w1.compare(L"t")<0,"Wide string reference test IV");
	Assert(w1.compare(L"Testx")<0,"Wide string reference test V");
	Assert(w1.compare(L"testx")<0,"Wide string reference test VI");
	Assert(w1.compare(L"T")>0,"Wide string reference test VII");
	Assert(w1.compare(L"Tes")>0,"Wide string reference test VIII");
	Assert(w1.compare(L" ")>0,"Wide string reference test IX");
	Assert(w1.compare(L"5")>0,"Wide string reference test X");
	Assert(w2.compare(L"")==0,"Wide string reference test XI");
	Assert(w2.compare(L"Test")<0,"Wide string reference test XII");
	Assert(w2.compare(L"t")<0,"Wide string reference test XIII");
	Assert(w2.compare(L" ")<0,"Wide string reference test XIV");
	Assert(w2.compare(L"1")<0,"Wide string reference test XV");
	Assert(w2.compare(L"testx")<0,"Wide string reference test XVI");
	Assert(w2.compare(L"T")<0,"Wide string reference test XVII");
	Assert(w2.compare(L"Tes")<0,"Wide string reference test XVIII");

	uString u1="Test";
	uString u2="";
	Assert(u1.compare(L"Test")==0,"String reference test I");
	Assert(u1.compare(L"")>0,"String reference test II");
	Assert(u1.compare(L"test")<0,"String reference test III");
	Assert(u1.compare(L"t")<0,"String reference test IV");
	Assert(u1.compare(L"Testx")<0,"String reference test V");
	Assert(u1.compare(L"testx")<0,"String reference test VI");
	Assert(u1.compare(L"T")>0,"String reference test VII");
	Assert(u1.compare(L"Tes")>0,"String reference test VIII");
	Assert(u1.compare(L" ")>0,"String reference test IX");
	Assert(u1.compare(L"5")>0,"String reference test X");
	Assert(u2.compare(L"")==0,"String reference test XI");
	Assert(u2.compare(L"Test")<0,"String reference test XII");
	Assert(u2.compare(L"t")<0,"String reference test XIII");
	Assert(u2.compare(L" ")<0,"String reference test XIV");
	Assert(u2.compare(L"1")<0,"String reference test XV");
	Assert(u2.compare(L"testx")<0,"String reference test XVI");
	Assert(u2.compare(L"T")<0,"String reference test XVII");
	Assert(u2.compare(L"Tes")<0,"String reference test XVIII");
	u1=L"Test";
	u2=L"";
	Assert(u1.compare(L"Test")==0,"String reference test I");
	Assert(u1.compare(L"")>0,"String reference test II");
	Assert(u1.compare(L"test")<0,"String reference test III");
	Assert(u1.compare(L"t")<0,"String reference test IV");
	Assert(u1.compare(L"Testx")<0,"String reference test V");
	Assert(u1.compare(L"testx")<0,"String reference test VI");
	Assert(u1.compare(L"T")>0,"String reference test VII");
	Assert(u1.compare(L"Tes")>0,"String reference test VIII");
	Assert(u1.compare(L" ")>0,"String reference test IX");
	Assert(u1.compare(L"5")>0,"String reference test X");
	Assert(u2.compare(L"")==0,"String reference test XI");
	Assert(u2.compare(L"Test")<0,"String reference test XII");
	Assert(u2.compare(L"t")<0,"String reference test XIII");
	Assert(u2.compare(L" ")<0,"String reference test XIV");
	Assert(u2.compare(L"1")<0,"String reference test XV");
	Assert(u2.compare(L"testx")<0,"String reference test XVI");
	Assert(u2.compare(L"T")<0,"String reference test XVII");
	Assert(u2.compare(L"Tes")<0,"String reference test XVIII");
#endif

	// Constructor sanitation tests
	uString u("Test");
	Assert(u.substr(1,2)=="es","std::substr compliancy");
	Assert(u.substr(0)=="Test","std::substr compliancy");
	Assert(u.substr(0,5)=="Test","std::substr compliancy");
	Assert(u.substr(0,4)=="Test","std::substr compliancy");
	Assert(u.substr(0,1)=="T","std::substr compliancy");
	Assert(u.substr(1,1)=="e","std::substr compliancy");
	Assert(u.substr(1,0)=="","std::substr compliancy");
	Assert(u.substr(1)=="est","std::substr compliancy");
	Assert(u.substr(4)=="","std::substr compliancy");
	Assert(uString("Test")=="Test","String constructor test I");
	Assert(uString("Test")!="test","String constructor test II");
#ifdef VILE_BUILD_UNICODE
	Assert(uString(L"Test")=="Test","String constructor test III");
	Assert(uString("Test")==L"Test","String constructor test IV");
	Assert(uString('T')=="T","String constructor test V");
	Assert(uString(L'T')=="T","String constructor test VI");
	Assert(uString('T')==L"T","String constructor test VII");
	Assert(uString("Test").bytes()==4,"String constructor test VIII");
	Assert(uString(L"Test").bytes()==4,"String constructor test IX");
	Assert(uString('T').bytes()==1,"String constructor test X");
	Assert(uString(L'T').bytes()==1,"String constructor test XI");
	Assert(uString('T').bytes()==1,"String constructor test XII");
	Assert(uString("Test").points()==4,"String constructor test XIII");
	Assert(uString(L"Test").points()==4,"String constructor test XIV");
	Assert(uString('T').points()==1,"String constructor test XV");
	Assert(uString(L'T').points()==1,"String constructor test XVI");
	Assert(uString('T').points()==1,"String constructor test XVII");
	Assert(uString(L"10").to_int()==10,"Wide strings converts integers");
	Assert(uString(L"10").to_int(16)==16,"Wide strings converts hex values");
	Assert(uString(L"1.1").to_double()==1.1,"Wide strings converts doubles");

	// UTF8 decoding
	char s2b[]={0xC2,0x80,0x00};
	char d2b[]={0xC2,0x80,0xC2,0x80,0x00};
	char s3b[]={0xE0,0xA0,0x80,0x00};
	char d3b[]={0xE0,0xA0,0x80,0xE0,0xA0,0x80,0x00};
	char s4b[]={0xF0,0x90,0x80,0x80,0x00};
	char d4b[]={0xF0,0x90,0x80,0x80,0xF0,0x90,0x80,0x80,0x00};
	Assert(aString(s2b).length()==2,"2-byte UTF8 point can be stored");
	Assert(uString(s2b)==uString(s2b),"2-byte UTF8 point can be matched");
	Assert(uString(s2b)!=uString("."),"2-byte UTF8 point can be compared");
	Assert(uString(s2b).points()==1,"2-byte UTF8 point stored");
	Assert(uString(s2b).bytes()==2,"2-byte UTF8 point decoded");
	Assert(aString(d2b).length()==4,"2-byte UTF8 Sequence can be stored");
	Assert(uString(d2b).points()==2,"2-byte UTF8 Sequence stored");
	Assert(uString(d2b).bytes()==4,"2-byte UTF8 Sequence decoded");
	Assert(uString(d2b)==uString(d2b),"2-byte UTF8 Sequence can be matched");
	Assert(uString(d2b)!=uString(s2b),"2-byte UTF8 Sequence can be compared");
	Assert(aString(s3b).length()==3,"3-byte UTF8 point can be stored");
	Assert(uString(s3b)==uString(s3b),"3-byte UTF8 point can be matched");
	Assert(uString(s3b)!=uString(".."),"3-byte UTF8 point can be compared");
	Assert(uString(s3b).points()==1,"3-byte UTF8 point stored");
	Assert(uString(s3b).bytes()==3,"3-byte UTF8 point decoded");
	Assert(aString(d3b).length()==6,"3-byte UTF8 Sequence can be stored");
	Assert(uString(d3b).points()==2,"3-byte UTF8 Sequence stored");
	Assert(uString(d3b).bytes()==6,"3-byte UTF8 Sequence decoded");
	Assert(uString(d3b)==uString(d3b),"3-byte UTF8 Sequence can be matched");
	Assert(uString(d3b)!=uString(s2b),"3-byte UTF8 Sequence can be compared");
	Assert(aString(s4b).length()==4,"4-byte UTF8 point can be stored");
	Assert(uString(s4b)==uString(s4b),"4-byte UTF8 point can be matched");
	Assert(uString(s4b)!=uString("."),"4-byte UTF8 point can be compared");
	Assert(uString(s4b).points()==1,"4-byte UTF8 point stored");
	Assert(uString(s4b).bytes()==4,"4-byte UTF8 point decoded");
	Assert(aString(d4b).length()==8,"4-byte UTF8 Sequence can be stored");
	Assert(uString(d4b).points()==2,"4-byte UTF8 Sequence stored");
	Assert(uString(d4b).bytes()==8,"4-byte UTF8 Sequence decoded");
	Assert(uString(d4b)==uString(d4b),"4-byte UTF8 Sequence can be matched");
	Assert(uString(d4b)!=uString(s4b),"4-byte UTF8 Sequence can be compared");
#endif
	Assert(u.to_lower()=="test","ANSI strings converts to lower case");
	Assert(u.to_upper()=="TEST","ANSI strings converts to upper case");
	Assert(uString("10").to_int()==10,"ANSI strings converts integers");
	Assert(uString("10").to_int(16)==16,"ANSI strings converts hex values");
	Assert(uString("1.1").to_double()==1.1,"ANSI strings converts doubles");
	Assert(u[0]=="T" && u[1]=="e","Substring array works for ANSI strings");
	Assert(!strcmp(u.c_str(),"Test"),"ANSI string exports correctly");

	// Test hashing
	Header("String hashing");
	uString s0="";
	uString s1="abc";
	uString s2="abC";
	uString s3="bac";
	uString s4="abcc";
	uString s5="abc";
	Uint32 h1=EDL_HashString(s1);
	Assert(h1!=0,"Hash produces non-zero values");
	Uint32 h0=EDL_HashString(s0);
	Assert(h1!=h0,"Hash produces unique values");
	Uint32 h2=EDL_HashString(s2);
	Assert(h1!=h2,"Hash identifies case changes");
	Uint32 h3=EDL_HashString(s3);
	Assert(h1!=h3,"Hash identifies position changes");
	Uint32 h4=EDL_HashString(s4);
	Assert(h1!=h4,"Hash identifies string changes");
	Uint32 h5=EDL_HashString(s5);
	Assert(h1==h5,"Hash identifies string content");

	// Sum up
	Report();
}


