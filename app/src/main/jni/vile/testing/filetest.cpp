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

#include "filetest.h"

FileTest::FileTest(){
	// Test archive handling
	Header("Archive Handling");
	ArchiveItem ai1("dir/file",0,0,0);
	Assert(ai1.dir=="dir/","Parsing directory \"dir/file\" -> \"dir/\"");
	Assert(ai1.base=="file","Parsing basename \"dir/file\" -> \"file\"");
	Assert(ai1.ext=="","Parsing extension \"dir/file\" -> \"\"");
	ai1.Set("/dir/file",0,0,0);
	Assert(ai1.dir=="/dir/","Parsing directory \"/dir/file\" -> \"/dir/\"");
	Assert(ai1.base=="file","Parsing basename \"/dir/file\" -> \"file\"");
	Assert(ai1.ext=="","Parsing extension \"/dir/file\" -> \"\"");
	ai1.Set("file",0,0,0);
	Assert(ai1.dir=="","Parsing directory \"file\" -> \"\"");
	Assert(ai1.base=="file","Parsing basename \"file\" -> \"file\"");
	Assert(ai1.ext=="","Parsing extension \"file\" -> \"\"");
	ai1.Set("file.ext",0,0,0);
	Assert(ai1.dir=="","Parsing directory \"file.ext\" -> \"\"");
	Assert(ai1.base=="file","Parsing basename \"file.ext\" -> \"file\"");
	Assert(ai1.ext=="ext","Parsing extension \"file.ext\" -> \"ext\"");
	ai1.Set("dir/file.ext",0,0,0);
	Assert(ai1.dir=="dir/","Parsing directory \"dir/file.ext\" -> \"dir/\"");
	Assert(ai1.base=="file","Parsing basename \"dir/file.ext\" -> \"file\"");
	Assert(ai1.ext=="ext","Parsing extension \"dir/file.ext\" -> \"ext\"");

	// Test red-black search tree
	RedBlackTree tree(new ArchiveNil,new ArchiveRoot);
	Assert(tree.Count()==0,"Default red-black tree is empty");
	char buffer[12];
	for(int i=0;i<1000;i++){
		sprintf(buffer,"%d.%d",i,i);
		tree.Insert(new ArchiveItem(buffer,i,i,i),SORT_STRICT);
	}
	Assert(tree.Count()==1000,"Red-black tree accepted 1000 items");
	ai1.Set("530",0,0,0);
	ArchiveItem *ai2=(ArchiveItem*)tree.Search(&ai1,SORT_STRICT);
	Assert(ai2==0,"Red-black strict search filtered missing extension");
	ai2=(ArchiveItem*)tree.Search(&ai1,SORT_NOEXT);
	Assert(ai2,"Red-black noext search ignored missing extension");
	Assert(ai2 && ai2->dsize==530,"Red-black search retrieved correct size");
	Assert(ai2 && ai2->offset==530,"Red-black search retrieved correct offset");
	ai1.ext="530";
	ai2=(ArchiveItem*)tree.Search(&ai1,SORT_STRICT);
	Assert(ai2,"Red-black strict search accepted extension");
	Assert(ai2 && ai2->dsize==530,"Red-black search retrieved correct size");
	Assert(ai2 && ai2->offset==530,"Red-black search retrieved correct offset");
	ai2=(ArchiveItem*)tree.Search(&ai1,SORT_NOEXT);
	Assert(ai2,"Red-black noext search accepted extension");
	Assert(ai2 && ai2->dsize==530,"Red-black search retrieved correct size");
	Assert(ai2 && ai2->offset==530,"Red-black search retrieved correct offset");
	ai1.ext="0";
	ai2=(ArchiveItem*)tree.Search(&ai1,SORT_STRICT);
	Assert(ai2==0,"Red-black strict search filtered bad extension");
	ai2=(ArchiveItem*)tree.Search(&ai1,SORT_NOEXT);
	Assert(ai2,"Red-black noext search ignored bad extension");
	Assert(ai2 && ai2->dsize==530,"Red-black search retrieved correct size");
	Assert(ai2 && ai2->offset==530,"Red-black search retrieved correct offset");

	// Cumulative search tests
	bool test=true;
	for(int i=0;i<1000;i++){
		sprintf(buffer,"%d.%d",i,i);
		ai1.Set(buffer,0,0,0);
		test&=tree.Search(&ai1,SORT_STRICT)!=0;
	}
	Assert(test,"Red-black successive search test"); 
	test=true;
	for(int i=999;i>=0;i--){
		sprintf(buffer,"%d.%d",i,i);
		ai1.Set(buffer,0,0,0);
		test&=tree.Search(&ai1,SORT_STRICT)!=0;
	}
	Assert(test,"Reverse red-black successive search test");  
	test=true;
	srand(time(NULL));
	for(int i=0;i<1000;i++){
		int j=rand()%1000;
		sprintf(buffer,"%d.%d",j,j);
		ai1.Set(buffer,0,0,0);
		test&=tree.Search(&ai1,SORT_STRICT)!=0;
	}
	Assert(test,"Red-black random order search test"); 


	Header("Filename Handling");
	// Path directory
	uString i="/home/me/isafile";
	uString o="/home/me/";
	uString r=EDL_FileDirectory(i);

	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),r.c_str());
	i="/home/me/isafile.ext";
	o="/home/me/";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="me/dir/";
	o="me/dir/";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="me/thisisafile";
	o="me/";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());

	// File basename
	i="/home/me/isafile";
	o="isafile";
	r=EDL_Searchname(i);
	Assert(r==o,"EDL_Searchname(%s) = %s",i.c_str(),o.c_str());
	i="/home/me/ISAfilE.eXt";
	o="isafile";
	r=EDL_Searchname(i);
	Assert(r==o,"EDL_Searchname(%s) = %s",i.c_str(),o.c_str());
	i="/home/me/isafile";
	o="isafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="/home/me/isafile.ext";
	o="isafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="/home/";
	o="";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me/dir/";
	o="";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me/thisisafile";
	o="thisisafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me/thisisafile.";
	o="thisisafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());

	// File extension
	i="me/afile";
	o="me/afile.ext";
	r=EDL_DefaultExtension(i,"ext");
	Assert(r==o,"EDL_DefaultExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/afile.ext";
	o="me/afile.ext";
	r=EDL_DefaultExtension(i,"txt");
	Assert(r==o,"EDL_DefaultExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/afile";
	o="me/afile.ext";
	r=EDL_ForceExtension(i,"ext");
	Assert(r==o,"EDL_ForceExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/afile.ext";
	o="me/afile.txt";
	r=EDL_ForceExtension(i,"txt");
	Assert(r==o,"EDL_ForceExtension(%s) = %s",i.c_str(),o.c_str());
	i="/home/me/isafile";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="/home/me/isafile.ext";
	o="ext";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="/home/";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/dir/";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/thisisafile";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/thisisafile.";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me/afile.extension";
	o="extension";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());

#ifdef VILE_ARCH_MICROSOFT
	// Path directory
	i="C:\\home\\me\\isafile";
	o="C:\\home\\me\\";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="C:\\home\\me\\isafile.ext";
	o="C:\\home\\me\\";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="C:\\";
	o="C:\\";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="me\\dir\\";
	o="me\\dir\\";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());
	i="me\\thisisafile";
	o="me\\";
	r=EDL_FileDirectory(i);
	Assert(r==o,"EDL_FileDirectory(%s) = %s",i.c_str(),o.c_str());

	// File basename
	i="C:\\home\\me\\isafile";
	o="isafile";
	r=EDL_Searchname(i);
	Assert(r==o,"EDL_Searchname(%s) = %s",i.c_str(),o.c_str());
	i="C:\\home\\me\\ISAfilE.eXt";
	o="isafile";
	r=EDL_Searchname(i);
	Assert(r==o,"EDL_Searchname(%s) = %s",i.c_str(),o.c_str());
	i="C:\\home\\me\\isafile";
	o="isafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="C:\\home\\me\\isafile.ext";
	o="isafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="C:\\";
	o="";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me\\dir\\";
	o="";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me\\thisisafile";
	o="thisisafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());
	i="me\\thisisafile.";
	o="thisisafile";
	r=EDL_FileName(i);
	Assert(r==o,"EDL_FileName(%s) = %s",i.c_str(),o.c_str());

	// File extension
	i="me\\afile";
	o="me\\afile.ext";
	r=EDL_DefaultExtension(i,"ext");
	Assert(r==o,"EDL_DefaultExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\afile.ext";
	o="me\\afile.ext";
	r=EDL_DefaultExtension(i,"txt");
	Assert(r==o,"EDL_DefaultExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\afile";
	o="me\\afile.ext";
	r=EDL_ForceExtension(i,"ext");
	Assert(r==o,"EDL_ForceExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\afile.ext";
	o="me\\afile.txt";
	r=EDL_ForceExtension(i,"txt");
	Assert(r==o,"EDL_ForceExtension(%s) = %s",i.c_str(),o.c_str());
	i="X:\\home\\me\\isafile";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="X:\\home\\me\\isafile.ext";
	o="ext";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="C:\\";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\dir\\";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\thisisafile";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\thisisafile.";
	o="";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
	i="me\\afile.extension";
	o="extension";
	r=EDL_FileExtension(i);
	Assert(r==o,"EDL_FileExtension(%s) = %s",i.c_str(),o.c_str());
#endif

	Header("Filesystem access");
	test=EDL_ReadableDirectory("asdjfasdfasfladsn32sssnk");
	Assert(!test,"EDL_ReadableDirectory refuses junk name");
	test=EDL_WritableDirectory("asd24r24rn4r2nkjjfasnk");
	Assert(!test,"EDL_WritableDirectory refuses junk name");
	test=EDL_ReadableFile("asdjfasdfasfladsn32sssnk");
	Assert(!test,"EDL_ReadableFile refuses junk name");
	test=EDL_WritableFile("asd24r24rn4r2nkjjfasnk");
	Assert(!test,"EDL_WritableFile refuses junk name");
	test=EDL_ReadableFile("asdjfasdfasfladsn32sssnk/dasda");
	Assert(!test,"EDL_ReadableFile refuses nested junk name");
	test=EDL_WritableFile("asd24r24rn4r2nkjjfasnk/dasdas");
	Assert(!test,"EDL_WritableFile refuses nested junk name");
	uString dir=EDL_CreateTemporary();
	Assert(dir.length(),"Can create temporary directory",dir.c_str());
	test=EDL_ReadableDirectory(dir);
	Assert(test,"EDL_ReadableDirectory accepted temporary");
	test=EDL_WritableDirectory (dir);
	Assert(test,"EDL_WritableDirectory accepted temporary");
	test=EDL_ReadableFile(dir);
	Assert(!test,"EDL_ReadableFile refused temporary directory");
	test=EDL_WritableFile(dir);
	Assert(!test,"EDL_WritableFile refused temporary directory");
	test=EDL_DeleteDirectory(dir);
	Assert(test,"Can delete empty directories");
	test=EDL_ReadableDirectory(dir);
	Assert(!test,"EDL_ReadableDirectory verified deletion");
	test=EDL_WritableDirectory (dir);
	Assert(!test,"EDL_WritableDirectory verified deletion");

	Header("Configuration files");
	uString cfgd=EDL_CreateTemporary();
	uString cfgf=cfgd+"unittest.ini";
	uString t_string;
	double t_double;
	int t_int;
	INIFile ini1(true);
	test=ini1.Get("Section","String",t_string);
	Assert(!test,"Empty inifile refuses string requests");
	test=ini1.Get("Section","Double",t_double);
	Assert(!test,"Empty inifile refuses double requests");
	test=ini1.Get("Section","Integer",t_int);
	Assert(!test,"Empty inifile refuses integer requests");
	test=ini1.Set("Section","Key","Value1");
	Assert(test,"Inifile accepts string values");
	test=ini1.Get("Section","Key",t_string);
	Assert(test,"Non-empty inifile accepts string requests");
	Assert(t_string=="Value1","Inifile returns original string");
	test=ini1.Set("Section","Key","Value1");
	Assert(test,"Inifile accepts identical string values");
	test=ini1.Get("Section","Key",t_string);
	Assert(test,"Non-empty inifile still accepts string requests");
	Assert(t_string=="Value1","Inifile returns rewritten string");
	test=ini1.Set("Section","Key","Value2");
	Assert(test,"Inifile accepts different string values");
	test=ini1.Get("Section","Key",t_string);
	Assert(test,"Non-empty inifile still accepts string requests");
	Assert(t_string=="Value2","Inifile returns overwritten string");
	test=ini1.Set("Section","NewKey","Value");
	Assert(test,"Inifile accepts writing values to other keys");
	test=ini1.Get("Section","Key",t_string);
	Assert(test,"Non-empty inifile still accepts string requests");
	Assert(t_string=="Value2","Inifile retains key when writing to others");
	test=ini1.Set("Section","String","string");
	Assert(test,"inifile accept string value");
	test=ini1.Set("Section","Double",1.23);
	Assert(test,"inifile accept double value");
	test=ini1.Set("Section","Integer",10);
	Assert(test,"inifile accepts integer values");
	test=ini1.Get("Section","String",t_string);
	Assert(test,"Inifile copy responds to valid string requests");
	Assert(t_string=="string","Copied inifile string values are correct");
	test=ini1.Get("Section","Double",t_double);
	Assert(test,"Inifile copy responds to valid double requests");
	Assert(t_double==1.23,"Copied inifile double values are correct");
	test=ini1.Get("Section","Integer",t_int);
	Assert(test,"Inifile copy responds to valid integer requests");
	Assert(t_int==10,"Copied inifile double values are correct");
	test=ini1.Get("Section","string",t_string);
	Assert(!test,"Inifiles honors case sensitivity on string requests");
	test=ini1.Get("Section","double",t_double);
	Assert(!test,"Inifiles honors case sensitivity on double requests");
	test=ini1.Get("Section","integer",t_int);
	Assert(!test,"Inifiles honors case sensitivity on integer requests");
	test=ini1.Get("section","String",t_string);
	Assert(!test,"Inifiles honors case sensitivity on string requests");
	test=ini1.Get("section","Double",t_double);
	Assert(!test,"Inifiles honors case sensitivity on double requests");
	test=ini1.Get("section","Integer",t_int);
	Assert(!test,"Inifiles honors case sensitivity on integer requests");
	Assert(ini1.WriteFile(cfgf),"Inifiles writes to disk");

	// Now open existing inifile
	INIFile ini2(false);
	Assert(ini2.ReadFile(cfgf),"Inifiles reads from disk");
	test=ini2.Get("Section","String",t_string);
	Assert(test,"Inifile copy responds to valid string requests");
	Assert(t_string=="string","Copied inifile string values are correct");
	test=ini2.Get("Section","Double",t_double);
	Assert(test,"Inifile copy responds to valid double requests");
	Assert(t_double==1.23,"Copied inifile double values are correct");
	test=ini2.Get("Section","Integer",t_int);
	Assert(test,"Inifile copy responds to valid integer requests");
	Assert(t_int==10,"Copied inifile double values are correct");
	test=ini2.Get("Section","string",t_string);
	Assert(test,"Inifiles honors case insensitivity on string requests");
	test=ini2.Get("Section","double",t_double);
	Assert(test,"Inifiles honors case insensitivity on double requests");
	test=ini2.Get("Section","integer",t_int);
	Assert(test,"Inifiles honors case insensitivity on int requests");
	test=ini2.Get("section","string",t_string);
	Assert(test,"Inifiles honors case insensitivity on string requests");
	test=ini2.Get("section","double",t_double);
	Assert(test,"Inifiles honors case insensitivity on double requests");
	test=ini2.Get("section","integer",t_int);
	Assert(test,"Inifiles honors case insensitivity on int requests");

	// Clean up
	test=EDL_DeleteDirectory(cfgd);
	Assert(test,"Can delete populated directories");
	test=EDL_ReadableFile(cfgf);
	Assert(!test,"EDL_ReadableFile verified deletion");
	test=EDL_WritableFile(cfgf);
	Assert(!test,"EDL_WritableFile verified deletion");
	test=EDL_ReadableDirectory(cfgd);
	Assert(!test,"EDL_ReadableDirectory verified deletion");
	test=EDL_WritableDirectory (cfgd);
	Assert(!test,"EDL_WritableDirectory verified deletion");


	// Sum up
	Report();
}

