#pragma once
#include "imports.h"

namespace mch
{
	class Node
	{
	public:
		static size_t NEXT_ID;

		size_t id;
		vector<vector<Node*>> layers;
		float* coords;

		Node(float* coords);
		void init(size_t level);

		float compute_distance_to(float* query, size_t& dimensions);
		void connect(vector<Node*>& items, size_t layer_idx);
		size_t level();
		vector<Node*>* neighborhood(size_t layer_idx);
		void set_neighborhood(vector<Node*>&& neighbors, size_t layer_idx);

		string to_string();
	};

	float compute_distance_between(float* coords, float* query, size_t& dimensions);
}
