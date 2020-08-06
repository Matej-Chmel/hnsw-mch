#ifndef MCH_STORAGE_UNIT
#define MCH_STORAGE_UNIT

#include "element.hpp"

namespace mch
{
	class StorageUnit
	{
	private:
		float* coords;
		elem_vec elements;

		void create_elements(size_t elements_len, size_t dimensions, size_t& id);

	public:
		typedef elem_vec::iterator iterator;

		StorageUnit(float* coords, size_t elements_len, size_t dimensions);
		StorageUnit(
			floats2& blocks, size_t coords_len, size_t elements_len, 
			size_t dimensions, size_t& id
		);
		~StorageUnit();

		size_t size();
		void sort_by_id();
		void sort_by_distance_to(Element* query);
		void load_from(FILE* graph_file);

		elem& operator[](size_t idx);
		iterator begin();
		iterator end();
	};

	typedef unique_ptr<StorageUnit> unit;
}

#endif
