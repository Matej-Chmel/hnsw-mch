#pragma once
#include "furthest_set.h"

namespace mch
{
	class NearestSet
	{
	public:
		typedef vector<float*>::iterator iterator;

		vector<float*> distances;

		NearestSet();
		NearestSet(const FurthestSet& set);

		iterator begin();
		iterator end();
		void heapify();
		void insert(float* distance);
		float* pop_front();
		size_t size();
	};
}
