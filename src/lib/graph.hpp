#pragma once
#include "measurements.hpp"
#include "parameters.hpp"
#include "set.hpp"

namespace mch
{
	class Graph
	{
	public:
		DatasetParams* data;
		GraphParams* params;
		GenMeasurement gen_measurement;
		float* node_coords;
		float* query_coords;
		layer nodes;
		vector<SetItem*> set_items;
		vector<unordered_set<Node*>> bruteforce_results;
		Node* entry;
		size_t entry_level;
		layer(Graph::*select_neighbors)(Node*, Set<furthest>&, size_t, size_t);

		Graph(DatasetParams* data);
		~Graph();

		BuildMeasurement build(GraphParams* params);
		void bruteforce();
		vector<layer> approx_search(BuildMeasurement& measurement);

		void insert(Node* node);
		Set<furthest> search_layer(float* query, Set<furthest> found, size_t ef, size_t layer_idx);
		layer select_neighbors_simple(Node* query, Set<furthest>& candidates, size_t m_to_return, size_t);
		layer select_neighbors_heuristic(Node* query, Set<furthest>& candidates, size_t m_to_return, size_t layer_idx);
		layer approx_search_one(float* query);

		void set_entry(Node* query, size_t level);
		size_t generate_level();
		SetItem* new_set_item(Node* node, float* query);
		Set<furthest> set_from_layer(layer& nodes, float* query);
		template<set_type type>
		bool insert_unique_into(Set<type>& item_set, Node* node, float* query, unordered_set<Node*>& node_set);
	};
}
