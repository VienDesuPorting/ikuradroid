#ifndef _UTEST_H_
#define _UTEST_H_

#include "../common/log.h"
#include "../common/edl_fs.h"
#include "../common/edl_gfx.h"
#include "../common/dstack.h"
#include "../common/dvector.h"
#include "../common/darray.h"
#include "../common/stringlist.h"
#include "../common/inifile.h"
#include "../res/archives/abase.h"

#define UNIT_BUFFER_SIZE			1024

class UnitTest {
	private:
		// Internal data
		static Uint32 total_passed;
		static Uint32 total_failed;
		static Uint32 total_time;
		char buffer[UNIT_BUFFER_SIZE];
		int passed,failed;
		Uint32 start,round;
	protected:
		// Only derived objects please ...
		UnitTest();

		// Standard unittest API
		void Header(uString Text,...);
		void Assert(bool Test,uString Text,...);
		void Report();
		void Reset();

		// Format helper
		uString RebuildString(uString String,va_list Arg);
	public:
		static void Summary();
};

#endif

