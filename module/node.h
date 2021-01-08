#pragma once
#include "mch.h"

namespace mch
{
	class Node
	{
	public:
		static size_t NEXT_ID;

		size_t id;
		float* coords;
		float distance;
		float* query;
		vector<vector<Node*>*>* layers;

		Node(float* coords, size_t level);
		~Node();

		void compute_distance_to(float* query, size_t dimensions);
		void connect(vector<Node*>& nodes, size_t idx);
		vector<Node*>& neighborhood(size_t idx);

		string to_string();
	};
}
