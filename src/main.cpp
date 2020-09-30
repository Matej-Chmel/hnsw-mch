#include <cstdio>
#include "lib/graph.hpp"
using namespace mch;
using namespace std;

#define TIME_FORMAT " %llu:%llu\n"
#define TIME(ms) ms / 1000, ms % 1000

int main()
{
    DatasetParams dataset_params = { 128, 1000, 10, 0, 184 };
    GraphParams graph_params = { 100, 20, 0.33, 40, 10, false, false, false };
    Graph graph(&dataset_params);
    BuildMeasurement measurement = graph.build(&graph_params);

    printf
    (
        "Nodes generated in" TIME_FORMAT
        "Queries generated in" TIME_FORMAT
        "Bruteforce search finished in" TIME_FORMAT
        "Graph build in" TIME_FORMAT
        "Approximate search finished in" TIME_FORMAT
        "Median accuracy is %.3f %%\n",
        TIME(graph.gen_measurement.node_ms),
        TIME(graph.gen_measurement.query_ms),
        TIME(graph.gen_measurement.bruteforce_ms),
        TIME(measurement.build_ms),
        TIME(measurement.approx_ms),
        measurement.median_accuracy
    );

    return 0;
}
