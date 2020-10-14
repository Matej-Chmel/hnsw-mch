#include "console.h"
#include "graph.h"
using namespace mch;
using namespace std;

int main()
{
	GenMeasurement gen;
	Dataset dataset(gen, 128, 250, 10, 10, 0, 184);

	GraphParams params(100, 20, 0.33f, 40, false, false, false);
	Graph graph(&dataset, params);

	auto build = graph.build();

	printf
	(
		"Nodes generated in" TIME_FORMAT
		"Queries generated in" TIME_FORMAT
		"Bruteforce search finished in" TIME_FORMAT
		"Graph build in" TIME_FORMAT
		"Approximate search finished in" TIME_FORMAT
		"Median accuracy is %.3f %%\n\n",
		TIME(gen.node_ms),
		TIME(gen.query_ms),
		TIME(gen.bruteforce_ms),
		TIME(build.build_ms),
		TIME(build.approx_ms),
		build.median_accuracy
	);

	return completed();
}
