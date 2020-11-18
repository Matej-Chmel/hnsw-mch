#include "node.h"

namespace mch
{
	size_t Node::NEXT_ID = 0;

	Node::Node(size_t dimensions, float* coords) : id(NEXT_ID++), dimensions(dimensions), level(0), coords(coords), distance(0), query(nullptr)
	{}
	Node::~Node()
	{
		for(auto& item : this->layers)
			delete item;
	}
	void Node::init(size_t level)
	{
		size_t length = level + 1;
		this->level = level;

		this->layers.reserve(length);
		for(size_t i = 0; i < length; i++)
			this->layers.push_back(new vector<Node*>);
	}
	void Node::compute_distance_to(Node* other, bool use_pow, bool use_sqrt)
	{
		if(this->query == other)
			return;

		float result = 0.0f;
		this->query = other;

		if(use_pow)
			for(size_t i = 0; i < this->dimensions; i++)
				result += powf(this->coords[i] - other->coords[i], 2);
		else
			for(size_t i = 0; i < this->dimensions; i++)
			{
				float difference = this->coords[i] - other->coords[i];
				result += difference * difference;
			}

		if(use_sqrt)
			this->distance = sqrtf(result);
		else
			this->distance = result;
	}
	void Node::connect(vector<Node*>* nodes, size_t idx)
	{
		vector<Node*>* layer = this->neighborhood(idx);

		for(auto& item : *nodes)
		{
			layer->push_back(item);
			item->neighborhood(idx)->push_back(this);
		}
	}
	vector<Node*>* Node::neighborhood(size_t idx)
	{
		return this->layers[idx];
	}
	void Node::set_neighborhood(vector<Node*>* layer, size_t idx)
	{
		delete this->neighborhood(idx);
		this->layers[idx] = layer;
	}
	string Node::to_string()
	{
		if(!this->layers.size())
			return "";

		auto result = std::to_string(this->id) + '\n';

		for(auto& layer : this->layers)
		{
			result += '\t';

			for(auto& item : *layer)
			{
				result += std::to_string(item->id);
				result += ' ';
			}

			result += '\n';
		}

		return result;
	}
}
