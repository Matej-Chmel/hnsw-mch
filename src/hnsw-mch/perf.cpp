#include "perf.h"

namespace mch
{
	tp __start__ = system_clock::now();

	void perf_start()
	{
		__start__ = system_clock::now();
	}
	void perf_stop(ll& dest)
	{
		dest = duration_cast<milliseconds>(system_clock::now() - __start__).count();
	}
}
