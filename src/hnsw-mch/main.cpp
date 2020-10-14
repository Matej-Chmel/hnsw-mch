#include "console.h"
#include "graph.h"
#include <iostream>
using namespace mch;
using namespace std;

constexpr size_t BUILDERS_LEN = 3;

template<typename T>
static
void builder
(
	Dataset* dataset, GraphParams* params,
	string param_key, const char* format, T start, T end, T step,
	vector<BuildMeasurement>* output, ExclusiveBar* progress
){
	Graph graph(dataset, *params);
	size_t n_builds = ((end - start) / step) + 1;
	char* fmt = new char[7];
	snprintf(fmt, 7, "%%s %s", format);

	output->reserve(n_builds);
	progress->init(((dataset->n_elements - 1) + dataset->n_queries) * n_builds);

	for(T value = start; value <= end; value += step)
	{
		char* name = new char[30];
		snprintf(name, 30, fmt, param_key.c_str(), value);

		graph.params.set(param_key, &value);
		output->push_back(graph.build(name, *progress));
	}

	delete[] fmt;
}

int main()
{
	GenMeasurement gen;
	Dataset dataset(gen, 128, 250, 10, 10, 0, 184);
	GraphParams params(100, 20, 0.33f, 40, false, false, false);

	printf
	(
		"Nodes generated in" TIME_FORMAT
		"Queries generated in" TIME_FORMAT
		"Bruteforce search finished in" TIME_FORMAT,
		TIME(gen.node_ms),
		TIME(gen.query_ms),
		TIME(gen.bruteforce_ms)
	);

	vector<vector<BuildMeasurement>> output;
	vector<thread> builders;

	output.resize(BUILDERS_LEN);
	builders.reserve(BUILDERS_LEN);

	ConcurrentProgress progress("Building", BUILDERS_LEN);
	
	{
		builders.push_back
		(
			thread
			(
				builder<bool>,
				&dataset, &params,
				GraphParams::r_use_heuristic, "%d", false, false, 1,
				&output[0], &progress.bars[0]
			)
		);

		builders.push_back
		(
			thread
			(
				builder<bool>,
				&dataset, &params,
				GraphParams::r_extend_candidates, "%d", false, false, 1,
				&output[1], &progress.bars[1]
			)
		);

		builders.push_back
		(
			thread
			(
				builder<bool>,
				&dataset, &params,
				GraphParams::r_keep_pruned, "%d", false, false, 1,
				&output[2], &progress.bars[2]
			)
		);
	}
	progress.run();

	for(size_t i = 0; i < BUILDERS_LEN; i++)
	{
		builders[i].join();
		auto result = &output[i];

		for(auto& item : *result)
		{
			printf
			(
				"%s\n"
				"Graph build in" TIME_FORMAT
				"Approximate search finished in" TIME_FORMAT
				"Median accuracy is %.3f %%\n\n",
				item.name,
				TIME(item.build_ms),
				TIME(item.approx_ms),
				item.median_accuracy
			);
		}
	}

	return completed();
}
