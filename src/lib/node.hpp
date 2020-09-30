#pragma once
#include <vector>

namespace mch
{
	using namespace std;
	class Node;
	typedef vector<Node*> layer;

	class Node
	{
	public:
		size_t id;
		float* coords;
		vector<layer> layers;

		Node(size_t id, float* coords);
		void init(size_t level);
		
		void connect(layer& nodes, size_t layer_idx);
		size_t level();
		layer& neighborhood(size_t layer_idx);
	};
}
