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
		ProgressUpdater* updater;

	public:
		ll bruteforce_time;
		vector<Benchmark> benchmarks;

		BenchmarkRunner
		(
			size_t dimensions, size_t node_count, size_t query_count,
			float min_value, float max_value, size_t k, ProgressUpdater* updater
		);
		BenchmarkRunner
		(
			size_t dimensions,
			const char* node_path, const char* query_path, const char* bruteforce_path,
			ProgressUpdater* updater
		);

		void add(Config* config, vector<size_t> ef);
		size_t get_query_count();
		void reserve(size_t capacity);
	};
}
