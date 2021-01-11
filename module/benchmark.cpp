#include "benchmark.h"
#include "timer.h"

namespace mch
{
	Benchmark::Benchmark(Config* config): build_time(0), config(config)
	{}
	void Benchmark::run(Bruteforce& bruteforce, Dataset& nodes, Dataset& queries, vector<size_t>& ef, size_t k, ProgressUpdater* updater)
	{
		Graph graph(this->config, &nodes);
		Timer timer;

		timer.start();
		graph.build(updater);
		this->build_time = timer.stop();

		this->recall.reserve(ef.size());
		this->search_times.reserve(ef.size());

		for(auto& _ef : ef)
		{
			timer.start();
			auto results = graph.search(queries, _ef, k, updater);
			this->search_times.push_back(timer.stop());
			this->recall.push_back(bruteforce.compare(results));
		}
	}
}
