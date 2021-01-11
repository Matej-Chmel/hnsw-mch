#include "furthest_set.h"

namespace mch
{
	FurthestSet::FurthestSet()
	{}
	FurthestSet::FurthestSet(float* distance)
	{
		this->distances.push_back(distance);
	}
	FurthestSet::iterator FurthestSet::begin()
	{
		return this->distances.begin();
	}
	FurthestSet::iterator FurthestSet::end()
	{
		return this->distances.end();
	}
	float* FurthestSet::front()
	{
		return this->distances.front();
	}
	vector<float*> FurthestSet::get_distances_copy()
	{
		return vector<float*>(this->distances);
	}
	void FurthestSet::keep_only_k_nearest(size_t k)
	{
		make_heap(this->begin(), this->end(), furthest_cmp);
		sort_heap(this->begin(), this->end(), furthest_cmp);

		if(this->distances.size() < k)
			k = this->distances.size();

		this->distances.erase(this->begin() + k, this->end());
	}
	void FurthestSet::insert(float* distance)
	{
		this->distances.push_back(distance);
		push_heap(this->begin(), this->end(), furthest_cmp);
	}
	void FurthestSet::remove_front()
	{
		pop_heap(this->begin(), this->end(), furthest_cmp);
		this->distances.pop_back();
	}
	size_t FurthestSet::size()
	{
		return this->distances.size();
	}
}
