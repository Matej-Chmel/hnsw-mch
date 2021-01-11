#pragma once
#include "bruteforce_item.h"

namespace mch
{
	struct NearestComparator
	{
		bool operator()(const float* const a, const float* const b) const;
	};
	struct FurthestComparator
	{
		bool operator()(const float* const a, const float* const b) const;
	};
	struct BruteforceComparator
	{
		bool operator()(const BruteforceItem* const a, const BruteforceItem* const b) const;
	};

	extern NearestComparator nearest_cmp;
	extern FurthestComparator furthest_cmp;
	extern BruteforceComparator sort_cmp;
}
