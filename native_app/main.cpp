#include "benchmark_runner.h"
#include "progress_bar.h"
#include "util.h"
using namespace mch;

//#define PROFILE
//#define SIFT

constexpr size_t dimensions = 128;
constexpr size_t ef_construction = 100;
constexpr size_t k = 100;
constexpr size_t m = 20;
constexpr size_t max_filename_length = 256;
constexpr float max_value = 184;
constexpr float min_value = 0;
constexpr float ml = 0.33;
constexpr size_t mmax = m * 2;
constexpr size_t nodes = 100000;
constexpr size_t queries = 1000;

string get_dataset_path(string filename)
{
	char buffer[max_filename_length];
	snprintf(buffer, max_filename_length, "..\\datasets\\%s.bin", filename.c_str());
	return buffer;
}
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
	Config config(ef_construction, false, false, m, ml, mmax, false);
	vector<size_t> ef = { 25, 50, 100, 300, 500, 1000 };

	#ifdef PROFILE
		ProgressUpdater* updater = nullptr;
	#else
		ProgressUpdater* updater = new ProgressUpdater(ProgressBar::get_instance().bind());
	#endif

	#ifdef SIFT
		BenchmarkRunner runner
		(
			dimensions, get_dataset_path("sift1M").c_str(), get_dataset_path("siftQ1M").c_str(),
			get_dataset_path("knnQA1M").c_str(), updater
		);
	#else
		BenchmarkRunner runner(dimensions, nodes, queries, min_value, max_value, k, updater);
	#endif

	runner.add(&config, ef);
	print_results("Neighbors selected by simple sort", runner, 0, ef);
	
	#ifndef PROFILE
		delete updater;
		pause_exit();
	#endif

	return 0;
}
