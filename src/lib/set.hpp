#pragma once
#include <unordered_set>
#include "comparators.hpp"

namespace mch
{
	template <set_type type>
	class Set
	{
	public:
		typedef vector<SetItem*>::iterator iterator;
		vector<SetItem*> items;

		Set();
		Set(SetItem* item);
		template<set_type other_type>
		Set(const Set<other_type>& other);

		size_t size();
		// bool insert_unique(SetItem* item) in Graph
		void insert(SetItem* item);
		SetItem* front();
		SetItem* pop_front();
		SetItem* nearest();
		layer k_nearest(size_t k_to_return);

		unordered_set<Node*> create_set();
		iterator begin();
		iterator end();
	};
}
