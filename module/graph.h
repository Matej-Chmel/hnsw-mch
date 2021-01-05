#pragma once
#include "config.h"
#include "dataset.h"

namespace mch
{
	class Graph
	{
	public:
		Config* config;
		size_t dimensions;
		Node* entry;
		size_t entry_level;
		vector<Node> nodes;

		Graph(Config* config);

		size_t generate_level();
		void insert(float* query);
		void search_layer_one(float* query, Node*& out_entry, size_t layer_idx);
		void search_layer(float* query, FurthestSet& out_entries, size_t ef, size_t layer_idx);
		void select_neighbors(float* query, vector<Node*>& out_candidates, size_t m, size_t layer_idx);
		void approx_search(float* query, size_t ef, size_t k, FurthestSet& output);

		void build(Dataset& dataset);
		vector<FurthestSet> search_all(Dataset& dataset, size_t ef, size_t k);

		string to_string();
		void print();
	};
}
