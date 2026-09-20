/*! \class CriticalPoint
 *	\brief Critical Point implementation
 *
 *  This implementation is just to test backwards compability of EngineWill.
 *
 *  The compability seems pretty horrible at first glance.
 */

#ifndef _CRITICAL_H_
#define _CRITICAL_H_

#include "../will.h"

// Dialog chrome geometry shared by CriticalTextview and the text
// selection layout: the winbase0 frame is 616x152 and docks above
// the 28px bottom menubar
static const int CP_WINDOW_W=616;
static const int CP_WINDOW_H=152;
static const int CP_MENUBAR_H=28;
// Uniform in-window row grid: row i occupies [30+24i,50+24i] from
// the window top, X spans 12..602, max 6 rows - the same slots
// the message text prints on, whether a name is shown or not
static const int CP_ROW_TOP=30;     // rows start 30px below the window top
static const int CP_ROW_PITCH=24;   // 24px row pitch
static const int CP_ROW_XPAD=12;    // rows start 12px right of the window
static const int CP_ROW_W=590;      // text rows span 12..602 inside the window

class CriticalPoint : public EngineWill {
	private:
	public:
		CriticalPoint(uString Path);

		// Overrides
		virtual const uString NativeID();
		virtual const uString NativeName();
		virtual void LayoutTextSelection(Stringlist *items);
};

#endif


