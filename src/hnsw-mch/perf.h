#pragma once
#include "imports.h"

namespace mch
{
	typedef long long ll;
	typedef system_clock::time_point tp;

	struct DatasetMeasurement
	{
		ll node_ms;
		ll query_ms;
		ll bruteforce_ms;
	};

	struct GraphMeasurement
	{
		ll build_ms;
		ll approx_ms;
		float median_accuracy;
	};

	extern tp __start__;

	void perf_start();
	void perf_stop(ll& dest);

	#define TIME_VALUE(ms) ms / 1000, ms % 1000
	#define TIME_FORMAT "%llu.%03llu\n"
}
