#include "distance.h"
#include "node.h"

namespace mch
{
	size_t Node::NEXT_ID = 0;

	Node::Node(float* coords, size_t level): id(NEXT_ID++), coords(coords), distance(0), query(nullptr)
	{
		this->layers.resize(level + 1);
	}
	void Node::compute_distance_to(float* query, size_t dimensions)
	{
		if(this->query == query)
			return;

		this->query = query;
		this->distance = distance_between(this->coords, this->query, dimensions);
	}
	void Node::connect(vector<Node*>& nodes, size_t idx)
	{
		auto& layer = this->neighborhood(idx);

		for(auto& item : nodes)
		{
			layer.push_back(item);
			item->neighborhood(idx).push_back(this);
		}
	}
	vector<Node*>& Node::neighborhood(size_t idx)
	{
		return this->layers[idx];
	}
	string Node::to_string()
	{
		if(!this->layers.size())
			return "";

		auto result = std::to_string(this->id) + '\n';

		for(auto& layer : this->layers)
		{
			result += '\t';

			vector<size_t> ids;
			ids.reserve(layer.size());

			for(auto& item : layer)
				ids.push_back(item->id);

			sort(ids.begin(), ids.end());

			for(auto& item : ids)
				result += std::to_string(item) + ' ';

			result += '\n';
		}

		return result;
	}
}
