#pragma once
#include "imports.h"

namespace mch
{
	typedef long long ll;
	typedef system_clock::time_point tp;

	struct ApproxMeasurement
	{
		size_t ef;
		ll time_ms;
		float median_accuracy;

		ApproxMeasurement(size_t ef, ll time_ms, float median_accuracy): ef(ef), time_ms(time_ms), median_accuracy(median_accuracy) {}
	};

	struct DatasetMeasurement
	{
		ll node_ms;
		ll query_ms;
		ll bruteforce_ms;
	};

	struct GraphMeasurement
	{
		ll build_ms;
		vector<ApproxMeasurement> results;
	};

	extern tp __start__;

	void perf_start();
	void perf_stop(ll& dest);

	#define TIME_VALUE(ms) ms / 1000, ms % 1000
	#define TIME_FORMAT "%llu.%03llu"
	#define TIME_FORMATN TIME_FORMAT "\n"
}
