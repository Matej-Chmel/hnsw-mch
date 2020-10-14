#include "heap_set.h"

namespace mch
{
	bool nearest::operator()(const HeapItem& a, const HeapItem& b) const
	{
		return a.distance > b.distance;
	}
	bool sort_nearest::operator()(const HeapItem& a, const HeapItem& b) const
	{
		return a.distance < b.distance;
	}
	bool furthest::operator()(const HeapItem& a, const HeapItem& b) const
	{
		return a.distance < b.distance;
	}
	bool bruteforce_cmp::operator()(const BruteforceItem& a, const BruteforceItem& b) const
	{
		return a.distance < b.distance;
	}
	BruteforceItem::BruteforceItem(float* coords, float distance): coords(coords), distance(distance)
	{}
	HeapItem::HeapItem(Node* node, float distance): node(node), distance(distance)
	{}
}
