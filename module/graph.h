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
		uniform_real_distribution<float> distribution;
		size_t entry_idx;
		size_t entry_level;
		Generator generator;
		vector<vector<vector<size_t>*>*> neighbors;
		vector<float*> relations;

		void approx_search(float* query, size_t ef, size_t k, vector<float*>& output);
		void connect(vector<size_t>& nodes, size_t query_idx, size_t layer_idx);
		void convert_distances_to_indexes(vector<float*>& distances, vector<size_t>& out_indexes);
		void convert_indexes_to_distances(float* query, vector<size_t>& indexes, vector<float*>& out_distances);
		size_t create_node(size_t level);
		size_t generate_level();
		float* get_distance_ptr(float* query, size_t node_idx);
		size_t get_node_idx(float* distance_ptr);
		void insert(float* query);
		vector<size_t>& neighborhood(size_t query_idx, size_t layer_idx);
		void search_layer(float* query, FurthestSet& out_entries, size_t ef, size_t layer_idx);
		void search_layer_one(float* query, size_t& out_node_idx, size_t layer_idx);
		vector<size_t> select_neighbors(float* query, FurthestSet& set, size_t m, size_t layer_idx);
		void select_neighbors(float* query, vector<size_t>& out_nodes, size_t m, size_t layer_idx);

	public:
		Graph(Config* config, Dataset* dataset, size_t seed);
		~Graph();

		void build(ProgressUpdater* updater);
		void build(size_t start_idx, size_t end_idx, ProgressUpdater* updater);
		vector<vector<float*>> search(Dataset& dataset, size_t ef, size_t k, ProgressUpdater* updater);

		#ifdef DEBUG_GRAPH
			FILE* debug_file;

			void add_neighbors_record();
			void neighbors_to_file(FILE* file);
			string neighbors_to_string();
		#endif
	};
}
