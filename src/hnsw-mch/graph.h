#pragma once
#include "dataset.h"

namespace mch
{
	class Graph
	{
	public:
		size_t ef;
		size_t m;
		size_t mmax;
		float ml;
		bool use_heuristic;
		bool extend_candidates;
		bool keep_pruned;
		GraphMeasurement measurement;
		Dataset* dataset;
		Set* nodes;
		Node* entry;

		Graph(Dataset* dataset, size_t ef, size_t m, size_t mmax, float ml, bool use_heuristic, bool extend_candidates, bool keep_pruned);
		Graph(const Graph& other);
		~Graph();

		size_t generate_level();
		void insert(Node* query);
		void search_layer(Node* query, Set* out_entries, size_t ef, size_t layer_idx);
		Set* select_neighbors(Node* query, Set* candidates, size_t m, size_t layer_idx);
		Set* approx_search(Node* query);

		void build();
		void search_all();

		string to_string();
		void print();

		string create_description();
		string create_measurement_description();
		void print_measurement_description();
		void save_measurement_description();
	};
}
