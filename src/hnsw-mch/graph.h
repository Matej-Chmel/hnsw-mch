#pragma once
#include "dataset.h"
#include "progress.h"

namespace mch
{
	class GraphParams
	{
	public:
		reflect(size_t, ef);
		reflect(size_t, m);
		reflect(float, ml);
		reflect(size_t, mmax);
		reflect(bool, use_heuristic);
		reflect(bool, extend_candidates);
		reflect(bool, keep_pruned);

		GraphParams(size_t ef, size_t m, float ml, size_t mmax, bool use_heuristic, bool extend_candidates, bool keep_pruned);

		void set(string key, void* value);
	};

	class BuildMeasurement
	{
	public:
		char* name;
		long long build_ms;
		long long approx_ms;
		float median_accuracy;

		BuildMeasurement(char* name);
		BuildMeasurement(const BuildMeasurement& other) = delete;
		BuildMeasurement(BuildMeasurement&& moved);
		~BuildMeasurement();
	};

	class Graph
	{
		Dataset* data;
		Node* entry;
		size_t entry_level;
		vector<Node> nodes;
		vector<Node*>(Graph::*select_neighbors)(Node* query, HeapSet<nearest> candidates, size_t m_to_return, size_t layer_idx);

		void set_entry(Node* query);
		size_t generate_level();
		void insert(Node* query);
		HeapItem search_one(float* query, HeapItem entry, size_t layer_idx);
		void search_layer(float* query, HeapSet<furthest>& found, size_t layer_idx);
		vector<Node*> select_neighbors_simple(Node* query, HeapSet<nearest> candidates, size_t m_to_return, size_t layer_idx);
		vector<Node*> select_neighbors_heuristic(Node* query, HeapSet<nearest> candidates, size_t m_to_return, size_t layer_idx);
		vector<float*> approx_search(float* query);
		void update_params();

	public:
		GraphParams params;

		Graph(Dataset* data, GraphParams params);

		BuildMeasurement build(char* name, ExclusiveBar& progress);
	};
}
