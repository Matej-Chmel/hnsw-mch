#pragma once
#include "furthest_set.h"

namespace mch
{
	class NearestSet
	{
		vector<Node*> nodes;

	public:
		typedef vector<Node*>::iterator iterator;

		NearestSet();
		NearestSet(const FurthestSet& set);

		iterator begin();
		iterator end();
		void init(vector<Node*>& vec);
		void insert(Node* item);
		Node* pop_front();
		size_t size();
	};
}
