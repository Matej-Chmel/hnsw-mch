#include "node.h"

namespace mch
{
	size_t Node::NEXT_ID = 0;

	Node::Node(float* coords): id(NEXT_ID++), coords(coords)
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
	string Node::to_string()
	{
		if(!this->layers.size())
			return "";

		auto result = std::to_string(this->id) + '\n';

		for(auto& layer : this->layers)
		{
			result += '\t';

			for(auto& item : layer)
			{
				result += std::to_string(item->id);
				result += ' ';
			}

			result += '\n';
		}

		return result;
	}
	float compute_distance_between(float* coords, float* query, size_t& dimensions)
	{
		float result = 0;
		for(size_t i = 0; i < dimensions; i++)
			result += powf(coords[i] - query[i], 2);
		return sqrtf(result);
	}
}
