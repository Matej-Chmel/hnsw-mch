#pragma once
#include "comparators.h"

namespace mch
{
	class FurthestSet
	{
	public:
		typedef vector<Node*>::iterator iterator;

		vector<Node*> nodes;

		FurthestSet();
		FurthestSet(Node* node);
		FurthestSet(vector<Node*>& nodes);
		FurthestSet(const FurthestSet& other);

		iterator begin();
		vector<Node*> copy_nodes();
		iterator end();
		Node* front();
		void keep_only_k_nearest(size_t k);
		void insert(Node* item);
		void remove_front();
		size_t size();
	};
}
