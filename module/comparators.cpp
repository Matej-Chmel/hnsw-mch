#include "comparators.h"

namespace mch
{
	NearestComparator nearest_cmp;
	FurthestComparator furthest_cmp;
	BruteforceComparator sort_cmp;

	bool NearestComparator::operator()(const float* const a, const float* const b) const
	{
		return *a > *b;
	}
	bool FurthestComparator::operator()(const float* const a, const float* const b) const
	{
		return *a < *b;
	}
	bool BruteforceComparator::operator()(const BruteforceItem* const a, const BruteforceItem* const b) const
	{
		return a->distance < b->distance;
	}
}
