/*! \unit log.h
 *	\brief Standard logging interface
 */
#ifndef _LOG_H_
#define _LOG_H_

// Common includes
#include <stdio.h>
#include "ustring.h"
#include "cfg.h"
#include "config.h"

#ifdef ANDROID
  #include "config.h"
  #include <jni.h>
  #include <android/log.h>
  #define LOGCAT(...) ((void)__android_log_print(ANDROID_LOG_INFO, "vile", __VA_ARGS__))
#endif
// Define terminal colors
#define LOG_BLACK           30
#define LOG_RED             31
#define LOG_GREEN           32
#define LOG_YELLOW          33
#define LOG_BLUE            34
#define LOG_MAGENTA         35
#define LOG_CYAN            36
#define LOG_WHITE           37

// Default colors
#define COLOR_ERROR			LOG_RED
#define COLOR_WARNING		LOG_BLUE
#define COLOR_MESSAGE		LOG_WHITE
#define COLOR_DEBUG			LOG_GREEN

// Set minimum logging level
#define MINLOGLEVEL			LLDEBUG

// Uncomment to flush every write (In case of crash)
#define LOGFLUSH

// Uncomment to get colors
#define LOGCOLOR

// Define levels
enum LOGLEVEL {
	LLDEBUG=0,
	LLMESSAGE,
	LLWARNING,
	LLERROR
};

#ifdef VILE_LOGGING_ENABLED
extern void LogRaw(LOGLEVEL Level,uString Text,...);
extern void LogBase(LOGLEVEL Level,uString Text,...);
extern void LogMessage(uString Text,...);
extern void LogVerbose(uString Text,...);
extern void LogWarning(uString Text,...);
extern void LogError(uString Text,...);
extern void LogTest(uString Text,...);
#else
#define LogBase(Level,Channel,Text,...)			;
#define LogRaw(Level,Channel,Text,...)			;
#define LogMessage(Text,...)					;
#define LogVerbose(Text,...)					;
#define LogWarning(Text,...)					;
#define LogError(Text,...)						;
#define LogTest(Text,...)						;
#endif

#ifdef VILE_LOGGING_DEBUG
extern void LogDebug(uString Text,...);
#else
#define LogDebug(Text,...)						;
#endif

#endif

