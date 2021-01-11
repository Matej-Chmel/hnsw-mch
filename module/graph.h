#pragma once
#include "config.h"
#include "dataset.h"
#include "nearest_set.h"

namespace mch
{
	class Graph
	{
		Config* config;
		Dataset* dataset;
		vector<float> distances;
		size_t entry_idx;
		size_t entry_level;
		vector<vector<vector<size_t>*>*> neighbors;
		vector<float*> relations;

		void approx_search(float* query, size_t ef, size_t k, vector<float*>& output);
		void connect(vector<size_t>& nodes, size_t query_idx, size_t layer_idx);
		size_t create_node(size_t level);
		size_t generate_level();
		float* get_distance_ptr(float* query, size_t node_idx);
		size_t get_node_idx(float* distance_ptr);
		void insert(float* query);
		vector<size_t>& neighborhood(size_t query_idx, size_t layer_idx);
		void search_layer(float* query, FurthestSet& out_entries, size_t ef, size_t layer_idx);
		void search_layer_one(float* query, size_t& out_node_idx, size_t layer_idx);
		vector<size_t> select_neighbors(vector<float*>&& distances);
		void select_neighbors(float* query, vector<size_t>& out_nodes);

	public:
		Graph(Config* config, Dataset* dataset);
		~Graph();

		void build(ProgressUpdater* updater);
		void build(size_t start_idx, size_t end_idx, ProgressUpdater* updater);
		vector<vector<float*>> search(Dataset& dataset, size_t ef, size_t k, ProgressUpdater* updater);
	};
}
