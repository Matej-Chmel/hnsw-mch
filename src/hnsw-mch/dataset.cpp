#include "dataset.h"
#include "file_util.h"
#include "progress.h"

namespace mch
{
	constexpr size_t FILENAME_MAX_LENGTH = 128;

	void Dataset::save(const char* title, float* data, size_t count, char* buffer)
	{
		snprintf(buffer, FILENAME_MAX_LENGTH, "%s_%zd_%.0f-%.0f.bin", title, count, this->min_value, this->max_value);
		save_file(string(buffer), data, count * this->dimensions);
	}
	void Dataset::bruteforce(size_t idx)
	{
		vector<BruteforceItem> items;
		float* query = this->query_coords + idx * this->dimensions;
		auto output = &this->results[idx];

		items.reserve(this->n_nodes);
		output->reserve(this->k_to_return);

		for(size_t i = 0; i < this->n_nodes; i++)
		{
			float* node = this->node_coords + i * this->dimensions;
			items.emplace_back(node, compute_distance_between(node, query, this->dimensions));
		}

		sort(items.begin(), items.end(), bruteforce_cmp());

		for(size_t i = 0; i < this->k_to_return; i++)
			output->insert(items[i].coords);
	}
	void Dataset::bruteforce_all(GenMeasurement& measurement)
	{
		PERF_CODE;
		this->results.resize(this->n_queries);

		PERF
		{
			if constexpr(SHOW_PROGRESS)
			{
				ProgressBar progress("Running bruteforce search", this->n_queries);
				for(size_t i = 0; i < this->n_queries; i++)
				{
					this->bruteforce(i);
					progress.update(1);
				}
			}
			else
				for(size_t i = 0; i < this->n_queries; i++)
					this->bruteforce(i);
		}
		DONE(measurement.bruteforce_ms);
	}
	Dataset::Dataset(GenMeasurement& measurement, size_t dimensions, size_t n_elements, size_t n_queries, size_t k_to_return, float min_value, float max_value):
		dimensions(dimensions), n_nodes(n_elements), n_queries(n_queries), k_to_return(k_to_return), min_value(min_value), max_value(max_value)
	{
		random_device rd;
		mt19937_64 gen(rd());
		uniform_real_distribution<float> dist(min_value, max_value);
		size_t node_coords_len = dimensions * n_elements;
		size_t query_coords_len = dimensions * n_queries;
		this->node_coords = new float[node_coords_len];
		this->query_coords = new float[query_coords_len];

		PERF_CODE;

		PERF
		{
			if constexpr(SHOW_PROGRESS)
			{
				ProgressBar progress("Generating nodes", n_elements);
				for(size_t i = 0; i < node_coords_len; i++)
				{
					this->node_coords[i] = dist(gen);
					progress.update(1);
				}
			}
			else
				for(size_t i = 0; i < node_coords_len; i++)
					this->node_coords[i] = dist(gen);
		}
		DONE(measurement.node_ms);

		PERF
		{
			if constexpr(SHOW_PROGRESS)
			{
				ProgressBar progress("Generating queries", n_queries);
				for(size_t i = 0; i < query_coords_len; i++)
				{
					this->query_coords[i] = dist(gen);
					progress.update(1);
				}
			}
			else
				for(size_t i = 0; i < query_coords_len; i++)
					this->query_coords[i] = dist(gen);
		}
		DONE(measurement.query_ms);

		this->bruteforce_all(measurement);
	}
	Dataset::Dataset(GenMeasurement& measurement, string nodes_filename, string queries_filename, size_t dimensions, size_t k_to_return):
		dimensions(dimensions), k_to_return(k_to_return)
	{
		PERF_CODE;
		size_t length;

		PERF
		{
			this->node_coords = load_file(nodes_filename, length);
			this->n_nodes = length / dimensions;
		}
		DONE(measurement.node_ms);
		
		PERF
		{
			this->query_coords = load_file(queries_filename, length);
			this->n_queries = length / dimensions;
		}
		DONE(measurement.query_ms);

		this->bruteforce_all(measurement);
	}
	Dataset::~Dataset()
	{
		delete[] this->node_coords;
		delete[] this->query_coords;
	}
	void Dataset::save()
	{
		char* filename = new char[FILENAME_MAX_LENGTH];
		
		this->save("nodes", this->node_coords, this->n_nodes, filename);
		this->save("queries", this->query_coords, this->n_queries, filename);

		delete[] filename;
	}
}
