#include "graph.h"
#include "file_util.h"
#include "progress_bar.h"
using namespace mch;
using namespace std;

constexpr size_t NODE_COUNT_START = 30000;
constexpr size_t NODE_COUNT_STEP = 1000;
constexpr size_t NODE_COUNT_END = 30000;
constexpr size_t APPROX_EF_START = 10;
constexpr size_t APPROX_EF_STEP = 10;
constexpr size_t APPROX_EF_END = 200;

static void build_and_search(size_t node_count, bool use_pow, bool use_sqrt)
{
	Dataset dataset(128, node_count, node_count / 10, node_count / 100, 0, 184, use_pow, use_sqrt);
	Graph graph(&dataset, 100, 20, 40, 0.33f, false, false, false);
	
	graph.build();
	
	if constexpr(SHOW_PROGRESS)
	{
		ProgressBar progress("Testing approximate search...", float(APPROX_EF_END) / float(APPROX_EF_STEP));

		for(size_t ef = APPROX_EF_START; ef <= APPROX_EF_END; ef += APPROX_EF_STEP)
		{
			graph.search_all(ef);
			progress.update();
		}
	}
	else
		for(size_t ef = APPROX_EF_START; ef <= APPROX_EF_END; ef += APPROX_EF_STEP)
			graph.search_all(ef);

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
