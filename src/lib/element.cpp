#include <cmath>
#include "element.hpp"

namespace mch
{
	const size_t Element::END = -1;

	Element::Element(size_t id, float* coords, size_t dimensions):
		distance(0), coords(coords), id(id), dimensions(dimensions),
		level(END), query(nullptr)
	{}

	void Element::init_layers(size_t idx)
	{
		if (idx < this->layers.size())
			return;
		
		size_t diff = (idx + 1) - this->layers.size();
		this->layers.reserve(idx + 1);

		for(size_t i = 0; i < diff; i++)
			this->layers.push_back(make_shared<elem_vec>());
	}

	void Element::set_level(size_t level)
	{
		this->level = level;
		this->init_layers(level);
	}

	void Element::compute_distance_to(Element* query)
	{
		if (this->query == query)
			return;
		
		this->distance = 0;
		for(size_t i = 0; i < this->dimensions; i++)
			this->distance += powf(this->coords[i] - query->coords[i], 2);
		
		this->distance = sqrtf(this->distance);
		this->query = query;
	}

	elem_vec_shr& Element::neighborhood(size_t layer_idx)
	{
		this->init_layers(layer_idx);
		return this->layers[layer_idx];
	}

	void Element::connect(elem_vec_shr elements, size_t layer_idx)
	{
		elem self = this->shared_from_this();
		elem_vec_shr layer = this->neighborhood(layer_idx);

		for(elem& item: *elements)
		{
			item->layers[layer_idx]->push_back(self);
			layer->push_back(item);
		}
	}

	void Element::write_to(vector<float>& coords_block, vector<size_t>& ids_block)
	{
		coords_block.insert(coords_block.end(), this->coords, this->coords + this->dimensions);
		ids_block.push_back(this->level);

		for(elem_vec_shr& layer: this->layers)
		{
			for(elem& item: *layer)
				ids_block.push_back(item->id);
			ids_block.push_back(END);
		}
	}

	void Element::write_to(float*& coords_block)
	{
		float* ptr = this->coords;
		float* end = this->coords + this->dimensions;

		while(ptr != end)
			*coords_block++ = *ptr++;
	}

	void Element::load_from(FILE* graph_file, elem_vec& all_elements)
	{
		fread(&this->level, sizeof(size_t), 1, graph_file);
		this->init_layers(this->level);

		for(elem_vec_shr& layer: this->layers)
		{
			for(;;)
			{
				size_t id;
				fread(&id, sizeof(size_t), 1, graph_file);
				if (id == END)
					break;
				layer->push_back(all_elements[id]);
			}
		}
	}

	bool Element::operator<(elem const& other) 
	{
		return this->distance < other->distance;
	}
}
