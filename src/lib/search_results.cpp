#include "search_results.hpp"

namespace mch
{
	SearchResults::SearchResults(size_t num_of_queries, size_t dimensions)
		: dimensions(dimensions), num_of_elements(0)
	{
		this->results.reserve(num_of_queries);
	}

	size_t SearchResults::size()
	{
		return this->num_of_elements;
	}

	void SearchResults::push(elem_vec_shr result)
	{
		this->num_of_elements += result->size();
		this->results.push_back(result);
	}

	void SearchResults::write_to(FILE* out_file) 
	{
		size_t num_of_queries = this->results.size();
		size_t coords_len = this->num_of_elements * this->dimensions;

		size_t* result_lens = new size_t[num_of_queries];
		float* coords = new float[coords_len];

		size_t* rs_ptr = result_lens;
		float* c_ptr = coords;

		fwrite(&num_of_queries, sizeof(size_t), 1, out_file);

		for(elem_vec_shr& result: this->results)
		{
			*rs_ptr++ = result->size();
			for(elem& item: *result)
				item->write_to(c_ptr);
		}

		fwrite(result_lens, sizeof(size_t), num_of_queries, out_file);
		fwrite(coords, sizeof(float), coords_len, out_file);

		delete[] result_lens;
		delete[] coords;
	}
}
