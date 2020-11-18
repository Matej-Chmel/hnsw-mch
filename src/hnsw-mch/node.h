#pragma once
#include "imports.h"

namespace mch
{
	class Node
	{
	public:
		static size_t NEXT_ID;

		size_t id;
		size_t dimensions;
		size_t level;
		float* coords;
		float distance;
		Node* query;
		vector<vector<Node*>*> layers;

		Node(size_t dimensions, float* coords);
		~Node();

		void init(size_t level);
		void compute_distance_to(Node* other, bool use_pow, bool use_sqrt);
		void connect(vector<Node*>* nodes, size_t idx);
		vector<Node*>* neighborhood(size_t idx);
		void set_neighborhood(vector<Node*>* layer, size_t idx);

		string to_string();
	};
}
