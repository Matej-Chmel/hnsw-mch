#pragma once
#include "dataset.h"

namespace mch
{
	class Bruteforce
	{
		vector<unordered_set<float*>> results;

		void knn_search(Dataset& queries, size_t query_idx, size_t k, vector<BruteforceItem*>& items);

	public:
		float compare(vector<FurthestSet>& approx_results);
		size_t load(const char* path, float* nodes, size_t query_count);
		void search(Dataset& nodes, Dataset& queries, size_t k, ProgressUpdater* updater);
	};
}
