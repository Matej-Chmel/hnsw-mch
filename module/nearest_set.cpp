#include "nearest_set.h"

namespace mch
{
	NearestSet::NearestSet()
	{}
	NearestSet::NearestSet(const FurthestSet& set): distances(set.distances)
	{
		this->heapify();
	}
	NearestSet::iterator NearestSet::begin()
	{
		return this->distances.begin();
	}
	NearestSet::iterator NearestSet::end()
	{
		return this->distances.end();
	}
	void NearestSet::heapify()
	{
		make_heap(this->begin(), this->end(), nearest_cmp);
	}
	void NearestSet::insert(float* distance)
	{
		this->distances.push_back(distance);
		push_heap(this->begin(), this->end(), nearest_cmp);
	}
	float* NearestSet::pop_front()
	{
		pop_heap(this->begin(), this->end(), nearest_cmp);
		auto output = this->distances.back();
		this->distances.pop_back();
		return output;
	}
	size_t NearestSet::size()
	{
		return this->distances.size();
	}
}
