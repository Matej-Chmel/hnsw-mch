#pragma once
#include "imports.h"

namespace mch
{
	class Node
	{
		vector<vector<Node*>> layers;
		// vector<Node*>

	public:
		float* coords;

		Node(float* coords);
		void init(size_t level);

		float compute_distance_to(float* query, size_t& dimensions);
		void connect(vector<Node*>& items, size_t layer_idx);
		size_t level();
		vector<Node*>* neighborhood(size_t layer_idx);
		void set_neighborhood(vector<Node*>&& neighbors, size_t layer_idx);
	};

	float compute_distance_between(float* coords, float* query, size_t& dimensions);
}
