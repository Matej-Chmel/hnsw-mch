#pragma once
#include "dataset.h"

namespace mch
{
	class Bruteforce
	{
		vector<unordered_set<float*>> results;

	public:
		float compare(vector<FurthestSet>& approx_results);
		void load(const char* path, float* nodes, size_t query_count, size_t k);
		void search(Dataset& nodes, Dataset& queries, size_t k);
	};
}
