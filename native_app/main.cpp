#include "benchmark.h"
#include "timer.h"
#include "util.h"
using namespace mch;

int main()
{
	Config config(100, false, false, 20, 0.33, 40, false);
	Config config_heuristic(config);
	config_heuristic.use_heuristic = true;

	Bruteforce bruteforce;
	Dataset nodes(128, 1000, 0, 184);
	Dataset queries(128, 10, 0, 184);
	Timer timer;

	timer.start();
	bruteforce.search(nodes, queries, 10);
	printf("Bruteforce time: %llu ms.\n\n", timer.stop());

	Benchmark benchmark(&config);
	Benchmark benchmark_heuristic(&config_heuristic);

	benchmark.run(bruteforce, nodes, queries, { 100, 300 }, 10);
	benchmark_heuristic.run(bruteforce, nodes, queries, { 100, 300 }, 10);

	pause_exit();
	return 0;
}
