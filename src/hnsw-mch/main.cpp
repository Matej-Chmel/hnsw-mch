#include "console.h"
#include "graph.h"
#include "file_util.h"
using namespace mch;
using namespace std;

constexpr bool SAVING_DATASET = false;

int main()
{
	GenMeasurement gen;

	if constexpr(SAVING_DATASET)
	{
		Dataset dataset(gen, 128, 250, 25, 10, 0, 184);
		dataset.save();
	}
	else
	{
		Dataset dataset(gen, "nodes_250_0-184.bin", "queries_25_0-184.bin", 128, 10);

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
	}

	return completed();
}
