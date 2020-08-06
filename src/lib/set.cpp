#include <iterator>
#include "set.hpp"

namespace mch
{
	Set::Set(Element* query) : query(query)
	{}

	Set::Set(Element* query, elem item) : query(query)
	{
		this->insert(item);
	}

	Set::Set(Element* query, elem_vec_shr& items) : query(query), elements(items->begin(), items->end())
	{}

	Set::Set(const Set& other) : query(other.query), elements(other.elements)
	{}

	size_t Set::size()
	{
		return this->elements.size();
	}

	Element* Set::get_query() 
	{
		return this->query;
	}

	bool Set::insert(elem item)
	{
		item->compute_distance_to(this->query);
		return this->elements.insert(item).second;
	}

	elem Set::nearest()
	{
		return *this->elements.begin();
	}

	elem Set::pop_nearest()
	{
		auto iter = this->elements.begin();
		elem item = *iter;
		this->elements.erase(iter);
		return item;
	}

	void Set::remove_nearest()
	{
		this->elements.erase(this->elements.begin());
	}

	elem Set::furthest()
	{
		return *this->elements.rbegin();
	}

	elem Set::pop_furthest()
	{
		auto iter = --this->elements.end();
		elem item = *iter;
		this->elements.erase(iter);
		return item;
	}

	void Set::remove_furthest()
	{
		this->elements.erase(--this->elements.end());
	}

	bool Set::every_is_further_than(elem const& item)
	{
		if (this->size() == 0)
			return true;
		return *item < this->nearest();
	}

	elem_vec_shr Set::slice(size_t length) 
	{
		length = this->elements.size() < length ? this->elements.size() : length;
		auto end = this->elements.begin();
		advance(end, length);

		return make_shared<elem_vec>(this->elements.begin(), end);
	}

	Set::iterator Set::begin() 
	{
		return this->elements.begin();
	}

	Set::iterator Set::end() 
	{
		return this->elements.end();
	}
}
