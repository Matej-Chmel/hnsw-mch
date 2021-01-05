#include "nearest_set.h"

namespace mch
{
	NearestSet::NearestSet()
	{}
	NearestSet::NearestSet(const FurthestSet& set): nodes(set.nodes)
	{
		make_heap(this->begin(), this->end(), nearest_cmp);
	}
	NearestSet::iterator NearestSet::begin()
	{
		return this->nodes.begin();
	}
	NearestSet::iterator NearestSet::end()
	{
		return this->nodes.end();
	}
	void NearestSet::init(vector<Node*>& vec)
	{
		this->nodes.swap(vec);
		make_heap(this->begin(), this->end(), nearest_cmp);
	}
	void NearestSet::insert(Node* item)
	{
		this->nodes.push_back(item);
		push_heap(this->nodes.begin(), this->nodes.end(), nearest_cmp);
	}
	Node* NearestSet::pop_front()
	{
		pop_heap(this->nodes.begin(), this->nodes.end(), nearest_cmp);
		auto output = this->nodes.back();
		this->nodes.pop_back();
		return output;
	}
	size_t NearestSet::size()
	{
		return this->nodes.size();
	}
}
