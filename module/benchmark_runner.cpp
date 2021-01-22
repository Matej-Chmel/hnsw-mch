#include "benchmark_runner.h"
#include "timer.h"

namespace mch
{
	size_t BenchmarkRunner::create_seed(size_t* seed_ptr)
	{
		if(seed_ptr == nullptr)
		{
			random_device rd;
			return rd();
		}

		return *seed_ptr;
	}
	BenchmarkRunner::BenchmarkRunner
	(
		size_t dimensions, size_t node_count, size_t query_count,
		float min_value, float max_value, size_t k,
		size_t* seed, ProgressUpdater* updater
	):
		seed(this->create_seed(seed)), k(k), nodes(dimensions, node_count, min_value, max_value, this->seed, updater),
		queries(dimensions, query_count, min_value, max_value, this->seed, updater), updater(updater)
    {
		Timer timer;

		timer.start();
		this->bruteforce.search(this->nodes, this->queries, this->k, this->updater);
		this->bruteforce_time = timer.stop();
    }
	BenchmarkRunner::BenchmarkRunner
	(
		size_t dimensions,
		const char* node_path, const char* query_path, const char* bruteforce_path,
		size_t* seed, ProgressUpdater* updater
	):
		nodes(dimensions, node_path), queries(dimensions, query_path),
		seed(this->create_seed(seed)), updater(updater), bruteforce_time(0)
	{
		this->k = this->bruteforce.load(bruteforce_path, this->nodes, this->queries.count);
	}
    void BenchmarkRunner::add(Config* config, vector<size_t> ef)
    {
		auto& benchmark = this->benchmarks.emplace_back(config);
		benchmark.run(this->bruteforce, this->nodes, this->queries, ef, this->k, this->seed, this->updater);
    }
	size_t BenchmarkRunner::get_query_count()
	{
		return this->queries.count;
	}
	void BenchmarkRunner::reserve(size_t capacity)
	{
		this->benchmarks.reserve(capacity);
	}
}
