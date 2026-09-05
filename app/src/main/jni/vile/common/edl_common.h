/*! \unit Common Extended Directmedia Layer
 *
 *  EDL is a ViLE-specific extension of SDL. EDL_Common handles common tasks
 *  such as text manipulation, random number generation and time handling. It
 *  also defines basic macros for math, windows and setting structures.
 */

#ifndef _EDL_COMMON_H_
#define _EDL_COMMON_H_

#ifdef VILE_ARCH_MICROSOFT
#include <windows.h>
#endif
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include "log.h"
#include "cfg.h"

// Select ONE hash function
#define VILE_HASH_CCITT16
//#define VILE_HASH_CRC16
//#define VILE_HASH_CRC32

// Basic macros
#define EDL_ODD(A)				((A)%2)
#define EDL_EVEN(A)				(!EDL_ODD(A))
#define EDL_ABS(A)				(A<0?A*-1:A)
#define EDL_MAX(A,B)			(B>A?B:A)
#define EDL_MIN(A,B)			(B<A?B:A)
#define EDL_LIMIT(A,MIN,MAX)	(EDL_MAX(EDL_MIN(A,MAX),MIN))

// Simple macros to configure rects and colors
#define EDL_SETRECT(R,X,Y,W,H)	({R.x=X; R.y=Y; R.w=W; R.h=H;})
#define EDL_SETCOLOR(C,R,G,B)	({C.r=R; C.g=G; C.b=B;})

// Window functions for softening blends
#define EDL_HAMMING(INDEX,SIZE)	(0.54-(0.46*cos((2*3.14*INDEX)/(SIZE-1))))
#define EDL_HANNING(A,B)		(0.5+0.5*cos((A*M_PI*2)/B))
#define EDL_WELCH(A,B)			(1-pow((A-(B/2))/(B/2),2))


// ----- Misc utilities -----
extern int EDL_Random(int Min,int Max);
extern bool EDL_Random();
extern time_t EDL_UnixTime();
extern SDL_Rect EDL_AddRect(SDL_Rect R1,SDL_Rect R2);


// ----- String utilities -----
extern uString EDL_Format(uString Text,...);
extern uString EDL_Lower(uString Text);
extern uString EDL_Upper(uString Text);
extern uString EDL_Trim(uString Text,bool Quotes,bool Spaces);
extern uString EDL_DateString(const time_t UnixTime);
extern uString EDL_TimeString(const time_t UnixTime);
extern uString EDL_IntString(int Integer);
extern uString EDL_DoubleString(double Double);


// ----- Hash utilities -----
extern Uint32 EDL_HashUpdate(Uint32 Hash,Uint8 Byte);
extern Uint32 EDL_HashUpdate(Uint32 Hash,void *Pointer);
extern Uint32 EDL_HashString(uString Text);
extern Uint32 EDL_HashBuffer(Uint8 *Buffer,Uint32 Length);


#endif

