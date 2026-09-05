#ifndef _RWTEST_H_
#define _RWTEST_H_

#include "utest.h"

class RWTest : public UnitTest {
	private:
		void TestObject(uString Name,RWops *Object,bool RO);
	public:
		RWTest();
};

#endif

