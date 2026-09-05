/*! \class INIFile
 *  \brief Simple configuration file interface
 *  \todo Make INIFile format and parsers UNICODE compliant
 */
#ifndef _INIFILE_H_
#define _INIFILE_H_

#include "log.h"
#include "edl_fs.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string>

class INIFile {
	private:
		bool casesensitive;
		char *inibuffer;
		int inisize;
		int ssection(uString S,char *B,unsigned int L);
		int skey(uString S,uString K,char *B,unsigned int L);
		int skey(uString K,char *B,unsigned int L);
	public:
		INIFile(bool CaseSensitive=true);
		~INIFile();
		bool Get(uString Section,uString Key,uString &Value);
		bool Get(uString Section,uString Key,double &Value);
		bool Get(uString Section,uString Key,int &Value);
		bool Get(uString Key,uString &Value);
		bool Get(uString Key,double &Value);
		bool Get(uString Key,int &Value);
		bool Set(uString Section,uString Key,uString Value);
		bool Set(uString Section,uString Key,double Value);
		bool Set(uString Section,uString Key,int Value);
		bool ReadFile(uString Path);
		bool WriteFile(uString Path);
};

#endif

