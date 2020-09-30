#include "set_item.hpp"

namespace mch
{
	SetItem::SetItem(Node* node, float distance): node(node), distance(distance)
	{}
	layer& SetItem::neighborhood(size_t layer_idx)
	{
		return this->node->neighborhood(layer_idx);
	}
}
