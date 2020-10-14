#include "node.h"

namespace mch
{
	Node::Node(float* coords): coords(coords)
	{}
	void Node::init(size_t level)
	{
		this->layers.clear();
		this->layers.resize(level + 1);
	}
	float Node::compute_distance_to(float* query, size_t& dimensions)
	{
		return compute_distance_between(this->coords, query, dimensions);
	}
	void Node::connect(vector<Node*>& items, size_t layer_idx)
	{
		auto layer = this->neighborhood(layer_idx);
		layer->reserve(layer->size() + items.size());
		for(auto item : items)
		{
			layer->push_back(item);
			item->neighborhood(layer_idx)->push_back(this);
		}
	}
	size_t Node::level()
	{
		return this->layers.size() - 1;
	}
	vector<Node*>* Node::neighborhood(size_t layer_idx)
	{
		return &this->layers[layer_idx];
	}
	void Node::set_neighborhood(vector<Node*>&& neighbors, size_t layer_idx)
	{
		this->layers[layer_idx] = move(neighbors);
	}
	float compute_distance_between(float* coords, float* query, size_t& dimensions)
	{
		float result = 0;
		for(size_t i = 0; i < dimensions; i++)
			result += powf(coords[i] - query[i], 2);
		return sqrtf(result);
	}
}
