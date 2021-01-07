#pragma once
#include "bruteforce.h"
#include "graph.h"

namespace mch
{
	class Benchmark
	{
	public:
		ll build_time;
		Config* config;
		vector<float> recall;
		vector<ll> search_times;

		Benchmark(Config* config);

		void run(Bruteforce& bruteforce, Dataset& nodes, Dataset& queries, vector<size_t>& ef, size_t k, ProgressUpdater* updater);
	};
}
