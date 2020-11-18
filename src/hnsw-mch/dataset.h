#pragma once
#include "set.h"
#include "perf.h"

namespace mch
{
	class Dataset
	{
		void bruteforce_all();
		void save(string filename, float* data, size_t length);
		string create_filename(const char* category);

	public:
		DatasetMeasurement measurement;
		size_t dimensions;
		size_t node_count;
		size_t query_count;
		size_t k;
		float min_value;
		float max_value;
		float* node_coords;
		float* query_coords;
		bool use_pow;
		bool use_sqrt;
		vector<Node*> all_queries;
		vector<Node*> all_nodes;
		vector<unordered_set<float*>> bruteforce_results;

		Dataset(size_t dimensions, size_t node_count, size_t query_count, size_t k, float min_value, float max_value, bool use_pow, bool use_sqrt);
		~Dataset();

		Set* create_nodes();
		float compare_to_bruteforce(vector<Set*>* approx_results);

		string create_filename(const char* category, size_t* count, const char* extension);
		string create_description();
		void save();
	};
}
