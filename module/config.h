#pragma once
#include "mch.h"

namespace mch
{
	class Config
	{
	public:
		size_t ef;
		bool extend_candidates;
		bool keep_pruned;
		size_t m;
		float ml;
		size_t mmax;
		bool use_heuristic;

		Config(size_t ef, bool extend_candidates, bool keep_pruned, size_t m, float ml, size_t mmax, bool use_heuristic);

		string to_string();
	};
}
