#pragma once
#include "heap_set.h"

namespace mch
{
	struct GenMeasurement
	{
		long long node_ms;
		long long query_ms;
		long long bruteforce_ms;
	};

	class Dataset
	{
		void save(const char* title, float* data, size_t count, char* buffer);
		void bruteforce(size_t idx);
		void bruteforce_all(GenMeasurement& measurement);

	public:
		size_t dimensions;
		size_t n_nodes;
		size_t n_queries;
		size_t k_to_return;
		float min_value;
		float max_value;
		float* node_coords;
		float* query_coords;
		vector<unordered_set<float*>> results;

		Dataset(GenMeasurement& measurement, size_t dimensions, size_t n_elements, size_t n_queries, size_t k_to_return, float min_value, float max_value);
		Dataset(GenMeasurement& measurement, string nodes_filename, string queries_filename, size_t dimensions, size_t k_to_return);
		~Dataset();

		void save();
	};
}
