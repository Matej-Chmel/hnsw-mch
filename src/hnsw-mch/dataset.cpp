#include "dataset.h"
#include "progress.h"

namespace mch
{
	void Dataset::bruteforce(size_t idx)
	{
		vector<BruteforceItem> items;
		float* query = this->query_coords + idx * this->dimensions;
		auto output = &this->results[idx];

		items.reserve(this->n_elements);
		output->reserve(this->k_to_return);

		for(size_t i = 0; i < this->n_elements; i++)
		{
			float* node = this->node_coords + i * this->dimensions;
			items.emplace_back(node, compute_distance_between(node, query, this->dimensions));
		}

		sort(items.begin(), items.end(), bruteforce_cmp());

		for(size_t i = 0; i < this->k_to_return; i++)
			output->insert(items[i].coords);
	}
	Dataset::Dataset(GenMeasurement& measurement, size_t dimensions, size_t n_elements, size_t n_queries, size_t k_to_return, float min_value, float max_value):
		dimensions(dimensions), n_elements(n_elements), n_queries(n_queries), k_to_return(k_to_return)
	{
		random_device rd;
		mt19937_64 gen(rd());
		uniform_real_distribution<float> dist(min_value, max_value);
		size_t node_coords_len = dimensions * n_elements;
		size_t query_coords_len = dimensions * n_queries;
		this->node_coords = new float[node_coords_len];
		this->query_coords = new float[query_coords_len];
		this->results.resize(n_queries);

		PERF_CODE;

		PERF
		{
			ProgressBar progress("Generating nodes", n_elements);
			for(size_t i = 0; i < node_coords_len; i++)
			{
				this->node_coords[i] = dist(gen);
				progress.update(1);
			}
		}
		DONE(measurement.node_ms);

		PERF
		{
			ProgressBar progress("Generating queries", n_queries);
			for(size_t i = 0; i < query_coords_len; i++)
			{
				this->query_coords[i] = dist(gen);
				progress.update(1);
			}
		}
		DONE(measurement.query_ms);

		PERF
		{
			ProgressBar progress("Running bruteforce search", n_queries);
			for(size_t i = 0; i < n_queries; i++)
			{
				this->bruteforce(i);
				progress.update(1);
			}
		}
		DONE(measurement.bruteforce_ms);
	}
	Dataset::~Dataset()
	{
		delete[] this->node_coords;
		delete[] this->query_coords;
	}
}
