#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include "utest.h"

class Benchmark : public UnitTest {
	private:
		static Uint32 total_time;
		static Uint32 total_runs;
		Uint32 time;
		Uint32 runs;
		Uint32 start;
	protected:
		// Only derived objects please ...
		Benchmark();

		// Benchmarking API
		void Start(uString Text,...);
		Uint32 Stop(int Runs);
		void Report();
	public:
		static void Summary();

};

#endif

