#include <algorithm>
#include "util/progress.hpp"
#include "storage_unit.hpp"

namespace mch
{
	StorageUnit::StorageUnit(float* coords, size_t elements_len, size_t dimensions):
		coords(coords)
	{
		size_t id = 0;
		this->create_elements(elements_len, dimensions, id);
	}

	StorageUnit::StorageUnit(floats2& blocks, size_t coords_len, size_t elements_len, size_t dimensions, size_t& id)
	{
		this->coords = new float[coords_len];
		float* ptr = this->coords;

		ProgressBar progress("Copying coordinates...", blocks.size());
		for(floats_unq& block: blocks)
		{
			copy(block->begin(), block->end(), ptr);
			ptr += block->size();
			progress.update(1);
		}

		this->create_elements(elements_len, dimensions, id);
	}

	void StorageUnit::create_elements(size_t elements_len, size_t dimensions, size_t& id)
	{
		float* ptr = this->coords;
		this->elements.reserve(elements_len);

		ProgressBar progress("Creating elements...", elements_len);
		for(size_t i = 0; i < elements_len; i++)
		{
			this->elements.push_back(make_shared<Element>(id++, ptr, dimensions));
			ptr += dimensions;
			progress.update(1);
		}
	}

	StorageUnit::~StorageUnit()
	{
		delete[] this->coords;
	}

	size_t StorageUnit::size()
	{
		return this->elements.size();
	}

	void StorageUnit::sort_by_id() 
	{
		sort(this->elements.begin(), this->elements.end(), Element::id_comparator());
	}

	void StorageUnit::sort_by_distance_to(Element* query) 
	{
		for_each(this->elements.begin(), this->elements.end(),
			[&](elem& item)
			{
				item->compute_distance_to(query);
			}
		);
		sort(this->elements.begin(), this->elements.end(), Element::distance_comparator());
	}

	void StorageUnit::load_from(FILE* graph_file) 
	{
		for(elem& item: this->elements)
			item->load_from(graph_file, this->elements);
	}

	elem& StorageUnit::operator[](size_t idx) 
	{
		return this->elements[idx];
	}

	StorageUnit::iterator StorageUnit::begin()
	{
		return this->elements.begin();
	}

	StorageUnit::iterator StorageUnit::end()
	{
		return this->elements.end();
	}
}
