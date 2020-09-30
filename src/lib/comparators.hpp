#pragma once
#include "set_item.hpp"

namespace mch
{
	enum set_type
	{
		nearest,
		furthest
	};

	struct NearestComparator
	{
		bool operator()(const SetItem* a, const SetItem* b) const;
	};
	struct FurthestComparator
	{
		bool operator()(const SetItem* a, const SetItem* b) const;
	};

	extern NearestComparator nearest_cmp;
	extern FurthestComparator furthest_cmp;
}
