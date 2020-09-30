#include <algorithm>
#include "set.hpp"

namespace mch
{
	template<set_type type>
	Set<type>::Set() 
	{}
	template<set_type type>
	Set<type>::Set(SetItem* item)
	{
		this->items.push_back(item);
	}
	template<set_type type>
	template<set_type other_type>
	Set<type>::Set(const Set<other_type>& other): items(other.begin(), other.end())
	{
		if constexpr(type != other_type)
		{
			if constexpr(type == nearest)
				make_heap(this->items.begin(), this->items.end(), nearest_cmp);
			else make_heap(this->items.begin(), this->items.end(), furthest_cmp);
		}
	}
	template<set_type type>
	size_t Set<type>::size()
	{
		return this->items.size();
	}
	template<set_type type>
	void Set<type>::insert(SetItem* item)
	{
		this->items.push_back(item);
		if constexpr(type == nearest)
			push_heap(this->items.begin(), this->items.end(), nearest_cmp);
		else push_heap(this->items.begin(), this->items.end(), furthest_cmp);
	}
	template<set_type type>
	SetItem* Set<type>::front()
	{
		return this->items.front();
	}
	template<set_type type>
	SetItem* Set<type>::pop_front()
	{
		if constexpr(type == nearest)
			pop_heap(this->items.begin(), this->items.end(), nearest_cmp);
		else pop_heap(this->items.begin(), this->items.end(), furthest_cmp);
		SetItem* front = this->items.back();
		this->items.pop_back();
		return front;
	}
	template<set_type type>
	SetItem* Set<type>::nearest()
	{
		if constexpr(type == nearest)
			return this->front();
		return *min_element(this->items.begin(), this->items.end(), furthest_cmp);
	}
	template<set_type type>
	layer Set<type>::k_nearest(size_t k_to_return)
	{
		sort_heap(this->items.begin(), this->items.end(), furthest_cmp);
		layer output;
		output.reserve(k_to_return);
		Set<type>::iterator end = this->end();
		for(auto iter = this->end() - k_to_return; iter != end; iter++)
			output.push_back((*iter)->node);
		return output;
	}
	template<set_type type>
	unordered_set<Node*> Set<type>::create_set()
	{
		unordered_set<Node*> set;
		set.reserve(this->size());
		for(SetItem*& item: *this)
			set.insert(item->node);
		return set;
	}
	template<set_type type>
	typename Set<type>::iterator Set<type>::begin()
	{
		return this->items.begin();
	}
	template<set_type type>
	typename Set<type>::iterator Set<type>::end()
	{
		return this->items.end();
	}
}
