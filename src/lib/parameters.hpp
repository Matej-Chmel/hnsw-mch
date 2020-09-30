#pragma once
#include <cstddef>

namespace mch
{
	struct DatasetParams
	{
		size_t dimensions;
		size_t n_elements;
		size_t n_queries;
		float min;
		float max;
	};

	struct GraphParams
	{
		size_t ef;
		size_t m;
		float ml;
		size_t mmax;
		size_t k_to_return;
		bool use_heuristic;
		bool extend_candidates;
		bool keep_pruned;
	};
}
