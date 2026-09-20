/* src/common/config.h.  Generated from config.h.in by configure.  */
/* src/common/config.h.in.  Generated from configure.ac by autoheader.  */

/* Compilation date */
#define COMPILATION_DATE "Fri Feb 20 16:13:40 KRAT 2015"

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Handles timezone and days */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Updates blocks of ram */
#define HAVE_MEMSET 1

/* Define to 1 if stdbool.h conforms to C99. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#define HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1



/* Define to 1 if the system has the type `_Bool'. */
#define HAVE__BOOL 1

/* Name of package */
#define PACKAGE "IkuraDroid"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "ikuradroid@vk.com"

/* Name of current line */
#define PACKAGE_LINE "Locutus"

/* Define to the full name of this package. */
#define PACKAGE_NAME "IkuraDroid"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "IkuraDroid 2.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ikuradroid"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.4"

/* SCM Revision */
#define SCM_VERSION "1735b0d"

/* Define to the type of arg 1 for `select'. */
#define SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define SELECT_TYPE_ARG5 (struct timeval *)

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#define VERSION "2.0.4"

/* Compile code for a BSD based target */
/* #undef VILE_ARCH_FREEBSD */

/* Compile code for a linuxbased target */
//#define VILE_ARCH_LINUX 1

/* Compile code for wintendo */
//#define VILE_ARCH_MICROSOFT 1

#ifndef ANDROID
#ifndef VILE_ARCH_MICROSOFT
#define HAVE_WORDEXP_H 1
#endif
#endif

/* Use internal SDL_gfx code (Not library) */
/* #undef VILE_BUILD_INTERNALGFX */

/* Use internal SDL_image code (Not library) */
/* #undef VILE_BUILD_INTERNALIMAGE */

/* Use internal SDL_ttf code (Not library) */
/* #undef VILE_BUILD_INTERNALTTF */

/* Build a native msys makefile */
/* #undef VILE_BUILD_MSYS */

/* Enabling SDL 1.3 support */
/* #undef VILE_BUILD_SDLHG */

/* Build standalone executable */
/* #undef VILE_BUILD_STATIC */

/* Enabling UNICODE support */
//#define VILE_BUILD_UNICODE 1

/* Build with win32 console */
/* #undef VILE_BUILD_W32CONSOLE */

/* Building with Electric Fence enabled */
/* #undef VILE_FEATURE_EFENCE */

/* Building with ffmpeg */
#define VILE_FEATURE_FFMPEG 1

/* Building with fluidsynth */
//#define VILE_FEATURE_FLUIDSYNTH 1

/* Building with hq scalers enabled */
//#define VILE_FEATURE_SCALER 1

/* Building with unittests enabled */
/* #undef VILE_FEATURE_UNITTEST */

/* Build with debug information */
/* #undef VILE_LOGGING_DEBUG */

/* Build with logging information */
#define VILE_LOGGING_ENABLED 1

/* Default configuration file */
#define VILE_PATH_CFG ""

/* Current working directory */
#define VILE_PATH_CWD ""

/* Default path to load games from */
#define VILE_PATH_GAME ""

/* Default resource file */
#define VILE_PATH_RES "vilevn.pck"

/* Default path for savegames */
#define VILE_PATH_SAVE ""

/* Building with crowd support */
#define VILE_SUPPORT_CROWD 1

/* Building with cware support */
#define VILE_SUPPORT_CWARE 1

/* Building with ikura support */
#define VILE_SUPPORT_IKURA 1

/* Building with jast support */
#define VILE_SUPPORT_JAST 1

/* Building with True love support */
#define VILE_SUPPORT_TLOVE 1

/* Building with will support */
#define VILE_SUPPORT_WILL 1

/* Building with windy support */
#define VILE_SUPPORT_WINDY 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */
