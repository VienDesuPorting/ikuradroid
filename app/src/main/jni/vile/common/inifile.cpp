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

#include "inifile.h"

INIFile::INIFile(bool CaseSensitive){
	inisize=0;
	inibuffer=0;
	casesensitive=CaseSensitive;
}

INIFile::~INIFile(){
	if(inibuffer){
		delete [] inibuffer;
	}
}

bool INIFile::WriteFile(uString Path){
	bool retval=false;
	if(Path.length() && inibuffer && inisize){
		FILE *inifile=0;
		if(Path.c_str()){
			inifile=fopen(Path.c_str(),"wb");
		}
		if(inifile){
			retval=(fwrite(inibuffer,1,inisize,inifile)>0);
			fclose(inifile);
		}
	}
	return retval;
}

bool INIFile::ReadFile(uString Path){
	bool retval=false;
	FILE *inifile=0;
	if(EDL_ReadableFile(Path)){
		inifile=fopen(Path.c_str(),"rb");
	}
	if(inifile){
		fseek(inifile,0,SEEK_END);
		int size=ftell(inifile);
		fseek(inifile,0,SEEK_SET);
		if(size>0){
			char *buffer=new char[size];
			if((int)fread(buffer,1,size,inifile)>0){
				// Data successfully read
				if(inibuffer){
					delete [] inibuffer;
				}
				inibuffer=buffer;
				inisize=size;
				retval=true;
			}
			else{
				delete [] buffer;
			}
		}
		fclose(inifile);
	}
	return retval;
}

/*! \brief Searches for a section
 *  \param S Section to search for
 *  \param B Buffer to search
 *  \param L Length of buffer
 *  \return Index of section or -1 if failed
 */
int INIFile::ssection(uString S,char *B,unsigned int L){
	for(unsigned int i=0;i<L;i++){
		if(B[i]=='['){
			i++;
			unsigned int j;
			if(casesensitive){
				for(j=0;(j+i)<L && S[j]==B[j+i];j++);
			}
			else{
				for(j=0;(j+i)<L && S[j].to_lower()==(char)tolower(B[j+i]);j++);
			}
			if(j==S.length() && B[i+j]==']'){
				int k=i+j+1;
				while(B[k]=='\r' || B[k]=='\n'){
					k++;
				}
				return k;
			}
		}
	}
	return -1;
}

/*! \brief Searches for first occurrance of a key
 *  \param K Key to search for
 *  \param B Buffer to search
 *  \param L Length of buffer
 *  \return Index of key or -1 if failed
 */
int INIFile::skey(uString K,char *B,unsigned int L){
	for(unsigned int i=0;i<L;i++){
		char b=B[i];
		char k=K.c_str()[0];
		if(!casesensitive){
			b=tolower(b);
			k=tolower(k);
		}
		if(b==k && (i==0 || B[i-1]=='\r' || B[i-1]=='\n')){
			unsigned int j;
			if(casesensitive){
				for(j=0;(j+i)<L && K[j]==B[j+i];j++);
			}
			else{
				for(j=0;(j+i)<L && K[j].to_lower()==(char)tolower(B[j+i]);j++);
			}
			if(j==K.length() && B[i+j]=='='){
				return i+j+1;
			}
		}
	}
	return -1;
}

/*! \brief Searches for a key within a section
 *  \param S Section to search in
 *  \param K Key to search for
 *  \param B Buffer to search
 *  \param L Length of buffer
 *  \return Index of key or -1 if failed
 */
int INIFile::skey(uString S,uString K,char *B,unsigned int L){
	unsigned int start=ssection(S,B,L);
	for(unsigned int i=start;i>=0 && i<L;i++){
		char b=B[i];
		char k=K.c_str()[0];
		if(!casesensitive){
			b=tolower(b);
			k=tolower(k);
		}
		if(b==k && (i==start || B[i-1]=='\r' || B[i-1]=='\n')){
			unsigned int j;
			if(casesensitive){
				for(j=0;(j+i)<L && K[j]==B[j+i];j++);
			}
			else{
				for(j=0;(j+i)<L && K[j].to_lower()==(char)tolower(B[j+i]);j++);
			}
			if(j==K.length() && B[i+j]=='='){
				return i+j+1;
			}
		}
		else if(B[i]=='['){
			return -1;
		}
	}
	return -1;
}

bool INIFile::Get(uString Key,uString &Value){
	bool retval=false;
	if(inibuffer && inisize){
		int start=skey(Key,inibuffer,inisize);
		if(start>0 && start<inisize){
			retval=true;
			Value="";
			int end=start;
			while(end<inisize && 
					inibuffer[end] &&
					inibuffer[end]!='\r' &&
					inibuffer[end]!='\n'){
				Value+=inibuffer[end++];
			}
		}
	}
	return retval;
}

bool INIFile::Get(uString Key,double &Value){
	uString value;
	bool retval=Get(Key,value);
	if(retval){
		char *end=(char*)value.c_str()+value.length();
		char *test=end;
		Value=strtod(value.c_str(),&end);
		retval=(end==test);
	}
	return retval;
}

bool INIFile::Get(uString Key,int &Value){
	double value;
	bool retval=Get(Key,value);
	if(retval){
		Value=value;
	}
	return retval;
}


bool INIFile::Get(uString Section,uString Key,uString &Value){
	bool retval=false;
	if(inibuffer && inisize){
		int start=skey(Section,Key,inibuffer,inisize);
		if(start>0 && start<inisize){
			retval=true;
			Value="";
			int end=start;
			while(end<inisize && 
					inibuffer[end] &&
					inibuffer[end]!='\r' &&
					inibuffer[end]!='\n'){
				Value+=inibuffer[end++];
			}
		}
	}
	return retval;
}

bool INIFile::Get(uString Section,uString Key,double &Value){
	uString value;
	bool retval=Get(Section,Key,value);
	if(retval){
		char *end=(char*)value.c_str()+value.length();
		char *test=end;
		Value=strtod(value.c_str(),&end);
		retval=(end==test);
	}
	return retval;
}

bool INIFile::Get(uString Section,uString Key,int &Value){
	double value;
	bool retval=Get(Section,Key,value);
	if(retval){
		Value=value;
	}
	return retval;
}

bool INIFile::Set(uString Section,uString Key,int Value){
	char b[255];
	sprintf(b,"%d",Value);
	return Set(Section,Key,b);
}

bool INIFile::Set(uString Section,uString Key,double Value){
	char b[255];
	sprintf(b,"%f",Value);
	return Set(Section,Key,b);
}

bool INIFile::Set(uString Section,uString Key,uString Value){
	bool retval=false;
	if(inibuffer && inisize){
		int start=skey(Section,Key,inibuffer,inisize);
		if(start>0 && start<inisize){
			int end=start;
			while(end<inisize && 
					inibuffer[end] &&
					inibuffer[end]!='\r' &&
					inibuffer[end]!='\n'){
				end++;
			}
			int oldlen=end-start;
			int newlen=Value.length();
			if(oldlen==newlen){
				// Inject new value into existing buffer
				retval=true;
				for(int i=0;i<newlen;i++){
					inibuffer[start+i]=Value[i].c_str()[0];
				}
			}
			else{
				// Recreate a new buffer
				int newsize=inisize+newlen-oldlen;
				char *newbuffer=new char[newsize];
				for(int i=0;i<start;i++){
					newbuffer[i]=inibuffer[i];
				}
				for(int i=0;i<newlen;i++){
					newbuffer[start++]=Value[i].c_str()[0];
				}
				int remainder=inisize-end;
				for(int i=0;i<remainder;i++){
					newbuffer[start+i]=inibuffer[end+i];
				}
				delete [] inibuffer;
				inibuffer=newbuffer;
				inisize=newsize;
				retval=true;
			}
		}
		else{
			start=ssection(Section,inibuffer,inisize);
			int end=start;
			uString data;
			if(start){
				// Insert new key
				data=Key+uString("=")+Value+uString("\r\n");
			}
			else{
				// Insert section with key
				data=uString("[")+Section+uString("]\r\n");
				data+=Key+uString("=")+Value+uString("\r\n");
			}
			int newsize=inisize+data.length();
			char *newbuffer=new char[newsize];
			for(int i=0;i<start;i++){
				newbuffer[i]=inibuffer[i];
			}
			for(unsigned int i=0;i<data.length();i++){
				newbuffer[start++]=data[i].c_str()[0];
			}
			int remainder=inisize-end;
			for(int i=0;i<remainder;i++){
				newbuffer[start+i]=inibuffer[end+i];
			}
			delete [] inibuffer;
			inibuffer=newbuffer;
			inisize=newsize;
			retval=true;
		}
	}
	else{
		// Create a new buffer
		uString data=uString("[")+Section+uString("]\r\n");
		data+=Key+uString("=")+Value+uString("\r\n");
		inisize=data.length();
		inibuffer=new char[inisize];
		for(int i=0;i<inisize;i++){
			inibuffer[i]=data[i].c_str()[0];
		}
		retval=true;
	}
	return retval;
}


