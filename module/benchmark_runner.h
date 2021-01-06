#pragma once
#include "benchmark.h"

namespace mch
{
	class BenchmarkRunner
	{
		Bruteforce bruteforce;
		size_t k;
		Dataset nodes;
		Dataset queries;

	public:
		ll bruteforce_time;
		vector<Benchmark> benchmarks;

		BenchmarkRunner(size_t dimensions, size_t node_count, size_t query_count, float min_value, float max_value, size_t k);
		BenchmarkRunner(size_t dimensions, const char* node_path, const char* query_path, const char* bruteforce_path, size_t k);

		void add(Config* config, vector<size_t> ef);
		void reserve(size_t capacity);
	};
}
