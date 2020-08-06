#ifndef MCH_SEARCH_RESULTS
#define MCH_SEARCH_RESULTS

#include "element.hpp"

namespace mch
{
	class SearchResults
	{
	private:
		elem_vec2 results;
		size_t num_of_elements;
		size_t dimensions;

	public:
		SearchResults(size_t num_of_queries, size_t dimensions);

		size_t size();
		void push(elem_vec_shr result);
		void write_to(FILE* out_file);
	};

	typedef unique_ptr<SearchResults> results;
}

#endif
