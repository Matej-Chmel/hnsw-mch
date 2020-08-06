#ifndef MCH_SET
#define MCH_SET

#include <memory>
#include <set>
#include <vector>
#include "element.hpp"

namespace mch
{
	class Set
	{
	private:
		Element* query;
		set<elem, Element::distance_comparator> elements;
	
	public:
		typedef set<elem, Element::distance_comparator>::iterator iterator;

		Set(Element* query);
		Set(Element* query, elem item);
		Set(Element* query, elem_vec_shr& items);
		Set(const Set& other);

		size_t size();
		Element* get_query();
		bool insert(elem item);
		elem nearest();
		elem pop_nearest();
		void remove_nearest();
		elem furthest();
		elem pop_furthest();
		void remove_furthest();
		bool every_is_further_than(elem const& item);
		elem_vec_shr slice(size_t length);

		iterator begin();
		iterator end();
	};

	typedef shared_ptr<Set> set_shr;
}

#endif
