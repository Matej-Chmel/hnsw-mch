#include "comparators.hpp"

namespace mch
{
	NearestComparator nearest_cmp;
	FurthestComparator furthest_cmp;

	bool NearestComparator::operator()(const SetItem* a, const SetItem* b) const
	{
		return a->distance > b->distance;
	}
	bool FurthestComparator::operator()(const SetItem* a, const SetItem* b) const
	{
		return a->distance < b->distance;
	}
}
