#include "furthest_set.h"

namespace mch
{
	FurthestSet::FurthestSet()
	{}
	FurthestSet::FurthestSet(Node* node)
	{
		this->nodes.push_back(node);
	}
	FurthestSet::FurthestSet(vector<Node*>& nodes): nodes(nodes)
	{}
	FurthestSet::FurthestSet(const FurthestSet& other) : nodes(other.nodes)
	{}
	FurthestSet::iterator FurthestSet::begin()
	{
		return this->nodes.begin();
	}
	vector<Node*> FurthestSet::copy_nodes()
	{
		return vector<Node*>(this->nodes);
	}
	FurthestSet::iterator FurthestSet::end()
	{
		return this->nodes.end();
	}
	Node* FurthestSet::front()
	{
		return this->nodes.front();
	}
	void FurthestSet::keep_only_k_nearest(size_t k)
	{
		make_heap(this->nodes.begin(), this->nodes.end(), furthest_cmp);
		sort_heap(this->nodes.begin(), this->nodes.end(), furthest_cmp);

		if(this->nodes.size() < k)
			k = this->nodes.size();

		this->nodes.erase(this->nodes.begin() + k, this->nodes.end());
	}
	void FurthestSet::insert(Node* item)
	{
		this->nodes.push_back(item);
		push_heap(this->nodes.begin(), this->nodes.end(), furthest_cmp);
	}
	void FurthestSet::remove_front()
	{
		pop_heap(this->nodes.begin(), this->nodes.end(), furthest_cmp);
		this->nodes.pop_back();
	}
	size_t FurthestSet::size()
	{
		return this->nodes.size();
	}
}
