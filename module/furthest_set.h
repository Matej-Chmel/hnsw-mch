#pragma once
#include "comparators.h"
#include "mch.h"

namespace mch
{
	class FurthestSet
	{
	public:
		typedef vector<float*>::iterator iterator;

		vector<float*> distances;

		FurthestSet();
		FurthestSet(float* distance);

		iterator begin();
		iterator end();
		float* front();
		vector<float*> get_distances_copy();
		void keep_only_k_nearest(size_t k);
		void insert(float* distance);
		void remove_front();
		size_t size();
	};
}
