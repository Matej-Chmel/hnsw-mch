#include "bruteforce_item.h"
#include "distance.h"

namespace mch
{
	BruteforceItem::BruteforceItem(float* coords): coords(coords), distance(0)
	{}
	void BruteforceItem::compute_distance_to(float* query, size_t dimensions)
	{
		this->distance = distance_between(this->coords, query, dimensions);
	}
}
