#include "node.hpp"

namespace mch
{
	Node::Node(size_t id, float* coords): id(id), coords(coords)
	{}
	void Node::init(size_t level)
	{
		size_t n_layers = level + 1;
		this->layers.clear();
		this->layers.reserve(n_layers);

		for(size_t i = 0; i < n_layers; i++)
			this->layers.emplace_back();
	}
	void Node::connect(layer& nodes, size_t layer_idx) 
	{
		vector<Node*> layer = this->layers[layer_idx];
		layer.reserve(layer.size() + nodes.size());

		for(Node*& node: nodes)
		{
			layer.push_back(node);
			node->neighborhood(layer_idx).push_back(this);
		}
	}
	size_t Node::level() 
	{
		return this->layers.size() - 1;
	}
	layer& Node::neighborhood(size_t layer_idx) 
	{
		return this->layers[layer_idx];
	}
}
