#include "comparators.h"

namespace mch
{
	NearestComparator nearest_cmp;
	FurthestComparator furthest_cmp;
	BruteforceComparator sort_cmp;

	bool NearestComparator::operator()(const Node* const a, const Node* const b) const
	{
		return a->distance > b->distance;
	}
	bool FurthestComparator::operator()(const Node* const a, const Node* const b) const
	{
		return a->distance < b->distance;
	}
	bool BruteforceComparator::operator()(const BruteforceItem* const a, const BruteforceItem* const b) const
	{
		return a->distance < b->distance;
	}
}
