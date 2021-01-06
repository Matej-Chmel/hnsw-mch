#include "benchmark_runner.h"
#include "util.h"
using namespace mch;

void print_results(const char* title, BenchmarkRunner& runner, size_t idx, vector<size_t>& ef)
{
	auto& benchmark = runner.benchmarks[idx];

	printf
	(
		"%s\n"
		"Build time: %llu ms\n"
		"Bruteforce time: %llu ms\n\n",
		title,
		benchmark.build_time,
		runner.bruteforce_time
	);

	for(size_t i = 0; i < ef.size(); i++)
		printf
		(
			"ef: %zd\n"
			"Search time: %llu ms\n"
			"Precision: %.3f\n\n",
			ef[i],
			benchmark.search_times[i],
			benchmark.recall[i]
		);
}

int main()
{
	Config config(100, false, false, 20, 0.33, 40, false);
	Config config_heuristic(config);
	vector<size_t> ef = { 100, 300, 1000 };

	config_heuristic.use_heuristic = true;

	BenchmarkRunner runner(128, 10000, 100, 0, 184, 10);
	runner.add(&config, ef);
	runner.add(&config_heuristic, ef);

	print_results("Neighbors selected by simple sort", runner, 0, ef);
	print_results("Neighbors selected by heuristic", runner, 1, ef);

	pause_exit();
	return 0;
}
