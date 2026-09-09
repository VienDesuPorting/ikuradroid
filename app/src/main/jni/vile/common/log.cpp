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

#include "log.h"

#ifdef VILE_LOGGING_ENABLED

/*! \brief Rebuilds a string from variable argument list
 *	\param String Formatted string
 *	\param Arg Argument list
 *	\returns Rebuilt string
 */
uString RebuildString(uString String,va_list Arg){
	const int size=1024*8;
	static char buffer[size];
	vsnprintf(buffer,size,String.c_str(),Arg);
	buffer[size-1]=0;
	return buffer;
}

/*! \brief Logs a message
 *	\param Text Formatted text
 */
void LogBase(LOGLEVEL Level,uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);
#ifdef ANDROID
	// Map the level to an android log priority
	int prio=ANDROID_LOG_INFO;
	if(Level==LLERROR) prio=ANDROID_LOG_ERROR;
	if(Level==LLWARNING) prio=ANDROID_LOG_WARN;
#ifdef VILE_LOGGING_DEBUG
	if(Level==LLDEBUG) prio=ANDROID_LOG_DEBUG;
#endif
	__android_log_print(prio, "vile", "%s", Text.c_str());
	// Optional mirror to a log file (absolute path set at startup)
	if(Cfg::System::Logfile.length()){
		FILE *tf=fopen(Cfg::System::Logfile.c_str(),"ab");
		if(tf){
			fwrite(Text.c_str(),1,Text.length(),tf);
			fwrite("\r\n",2,1,tf);
			fclose(tf);
		}
	}
#else
    printf("%s",Text.c_str());
	if(Level>=MINLOGLEVEL){
		// Optional logging to file
		if(Cfg::System::Logfile.length()){
			FILE *tf=fopen(Cfg::System::Logfile.c_str(),"ab");
			if(tf){
				fwrite(Text.c_str(),1,Text.length(),tf);
				fwrite("\r\n",2,1,tf);
				fclose(tf);
			}
		}

		// Logg to standard output
		FILE *tf=Level==LLERROR?stderr:stdout;
		if(Cfg::System::Logcolor){
			char ansib[32];
			int FG=LOG_WHITE;
			if(Level==LLERROR)		FG=COLOR_ERROR;
			if(Level==LLWARNING)	FG=COLOR_WARNING;
			if(Level==LLMESSAGE)	FG=COLOR_MESSAGE;
			if(Level==LLDEBUG)		FG=COLOR_DEBUG;
			int ansil=sprintf(ansib,"%c[%d;%dm",0x1B,0,FG);
			fwrite(ansib,ansil,1,tf);
			fwrite(Text.c_str(),Text.length(),1,tf);
			ansil=sprintf(ansib,"%c[%d;%d;%dm ",0x1B,0,0,0);
			fwrite(ansib,ansil,1,tf);
		}
		else{
			fwrite(Text.c_str(),Text.length(),1,tf);
		}
		fwrite("\r\n",2,1,tf);

#ifdef LOGFLUSH
		fflush(tf);
#endif
	}
#endif
}

/*! \brief Logs a string without adding any formatting or newlines
 *	\param Text Formatted text
 */
void LogRaw(LOGLEVEL Level,uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	if(Level>=MINLOGLEVEL){
		// Optional logging to file
		if(Cfg::System::Logfile.length()){
			FILE *tf=fopen(Cfg::System::Logfile.c_str(),"ab");
			if(tf){
				fwrite(Text.c_str(),1,Text.length(),tf);
				fclose(tf);
			}
		}

		// Logg to standard output
		FILE *tf=Level==LLERROR?stderr:stdout;
		fwrite(Text.c_str(),Text.length(),1,tf);

		// Always flush (No newlines)
		fflush(tf);
	}
}

/*! \brief Logs a basic message
 *	\param Text Formatted text
 */
void LogMessage(uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Handle message
	LogBase(LLMESSAGE,Text);
}

/*! \brief Logs a message if the verbose flag is set
 *	\param Text Formatted text
 */
void LogVerbose(uString Text,...){
	if(Cfg::System::Verbose){
		// Gather arguments
		va_list arg;
		va_start(arg,Text);
		Text=RebuildString(Text,arg);
		va_end(arg);

		// Handle message
		LogBase(LLDEBUG,Text);
	}
}

/*! \brief Logs an error message
 *	\param Text Formatted text
 */
void LogError(uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Handle message
	LogBase(LLERROR,Text);
}

/*! \brief Logs temporary debug message
 *	\param Text Formatted text
 */
void LogWarning(uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Handle message
	LogBase(LLWARNING,Text);
}

/*! \brief Logs temporary debug message
 *	\param Text Formatted text
 */
void LogTest(uString Text,...){
	// Gather arguments
	va_list arg;
	va_start(arg,Text);
	Text=RebuildString(Text,arg);
	va_end(arg);

	// Handle message
	LogBase(LLWARNING,Text);
}

#ifdef VILE_LOGGING_DEBUG
/*! \brief Logs a message if the verbose flag is set
 *	\param Text Formatted text
 */
void LogDebug(uString Text,...){
	if(Cfg::System::Verbose){
		// Gather arguments
		va_list arg;
		va_start(arg,Text);
		Text=RebuildString(Text,arg);
		va_end(arg);

		// Handle message
		LogBase(LLDEBUG,Text);
	}
}
#endif	// Endof Debugger enabled

#endif	// Endff Disable logging

