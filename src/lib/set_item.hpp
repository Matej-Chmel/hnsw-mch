#pragma once
#include "node.hpp"

namespace mch
{
	class SetItem
	{
	public:
		Node* node;
		float distance;

		// Constructed in Graph.
		SetItem(Node* node, float distance);

		layer& neighborhood(size_t layer_idx);
	};
}
