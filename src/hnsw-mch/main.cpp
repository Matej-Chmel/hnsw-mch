#include "graph.h"
#include "file_util.h"
using namespace mch;
using namespace std;

constexpr size_t NODE_COUNT_START = 10000;
constexpr size_t NODE_COUNT_STEP = 1000;
constexpr size_t NODE_COUNT_END = 10000;

static void build_and_search(size_t node_count, bool use_pow, bool use_sqrt)
{
	Dataset dataset(128, node_count, node_count / 10, node_count / 100, 0, 184, use_pow, use_sqrt);
	Graph graph(&dataset, 100, 20, 40, 0.33f, false, false, false);

	graph.build();
	graph.search_all();
	graph.save_measurement_description();
}

int main()
{
	for(size_t node_count = NODE_COUNT_START; node_count <= NODE_COUNT_END; node_count += NODE_COUNT_STEP)
	{
		build_and_search(node_count, false, false);
		//build_and_search(node_count, true, true);
	}
	
	fputs("Press any key to continue.", stdout);
	(void) getchar();

	return 0;
}
