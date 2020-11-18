#include "dataset.h"
#include "file_util.h"
#include "util.h"

namespace mch
{
	constexpr size_t COUNT_MAX_LENGTH = 32;
	constexpr size_t DATASET_DESCRIPTION_MAX_LENGTH = 512;
	constexpr const char* CATEGORY_NODES = "nodes";
	constexpr const char* CATEGORY_QUERIES = "queries";

	void Dataset::bruteforce_all()
	{
		this->all_queries.reserve(this->query_count);
		this->bruteforce_results.resize(this->query_count);

		for(size_t i = 0; i < this->query_count; i++)
			this->all_queries.push_back(new Node(this->dimensions, this->query_coords + i * this->dimensions));

		perf_start();
		{
			vector<Node*> nodes;
			nodes.reserve(this->node_count);

			for(size_t i = 0; i < this->node_count; i++)
				nodes.push_back(new Node(this->dimensions, this->node_coords + i * this->dimensions));

			for(size_t i = 0; i < this->query_count; i++)
			{
				auto result = &this->bruteforce_results[i];
				Node* query = this->all_queries[i];

				for(auto& item : nodes)
					item->compute_distance_to(query, use_pow, use_sqrt);

				sort(nodes.begin(), nodes.end(), sort_cmp);

				for(size_t j = 0; j < this->k; j++)
					result->insert(nodes[j]->coords);
			}

			for(auto& item : nodes)
				delete item;
		}
		perf_stop(this->measurement.bruteforce_ms);
	}
	void Dataset::save(string filename, float* data, size_t length)
	{
		save_file(filename, data, length);
	}
	string Dataset::create_filename(const char* category)
	{
		return this->create_filename(category, strcmp(category, CATEGORY_NODES) == 0 ? &this->node_count : &this->query_count, ".bin");
	}
	Dataset::Dataset(size_t dimensions, size_t node_count, size_t query_count, size_t k, float min_value, float max_value, bool use_pow, bool use_sqrt):
		dimensions(dimensions), node_count(node_count), query_count(query_count), k(k), min_value(min_value), max_value(max_value), use_pow(use_pow), use_sqrt(use_sqrt)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<float> dist(this->min_value, this->max_value);
		size_t length;
		string filename;

		perf_start();
		{
			filename = path_dataset(this->create_filename(CATEGORY_NODES));
			this->node_coords = load_file(filename, length);

			if(!this->node_coords)
			{
				length = this->node_count * this->dimensions;
				this->node_coords = new float[length];

				for(size_t i = 0; i < length; i++)
					this->node_coords[i] = dist(gen);

				this->save(filename, this->node_coords, length);
			}
		}
		perf_stop(this->measurement.node_ms);

		perf_start();
		{
			filename = path_dataset(this->create_filename(CATEGORY_QUERIES));
			this->query_coords = load_file(filename, length);

			if(!this->query_coords)
			{
				length = this->query_count * this->dimensions;
				this->query_coords = new float[length];

				for(size_t i = 0; i < length; i++)
					this->query_coords[i] = dist(gen);

				this->save(filename, this->query_coords, length);
			}
		}
		perf_stop(this->measurement.query_ms);

		this->bruteforce_all();
	}
	Dataset::~Dataset()
	{
		for(auto item : this->all_nodes)
			delete item;
		for(auto item : this->all_queries)
			delete item;

		delete[] this->node_coords;
		delete[] this->query_coords;
	}
	Set* Dataset::create_nodes()
	{
		Set* set = new Set(nullptr, this->use_pow, this->use_sqrt);
		this->all_nodes.reserve(this->all_nodes.size() + this->node_count);

		for(size_t i = 0; i < this->node_count; i++)
		{
			Node* node = new Node(this->dimensions, this->node_coords + i * this->dimensions);
			this->all_nodes.push_back(node);
			set->push(node);
		}
		
		return set;
	}
	float Dataset::compare_to_bruteforce(vector<Set*>* approx_results)
	{
		vector<size_t> correct_counts;
		correct_counts.reserve(this->query_count);

		for(size_t i = 0; i < this->query_count; i++)
		{
			size_t correct = 0;
			auto bruteforce_result = &this->bruteforce_results[i];
			auto approx_result = approx_results->operator[](i);

			for(auto& item : *approx_result)
				if(bruteforce_result->find(item->coords) != bruteforce_result->end())
					correct++;

			correct_counts.push_back(correct);
		}

		sort(correct_counts.begin(), correct_counts.end());

		if(this->query_count % 2)
		{
			size_t half = this->query_count / 2;
			float a = float(correct_counts[half] + correct_counts[half + 1]);
			return a / 2 / this->k * 100;
		}
		else
			return float(correct_counts[this->query_count / 2]) / this->k * 100;
	}
	string Dataset::create_filename(const char* category, size_t* count, const char* extension)
	{
		char* count_buffer = new char[COUNT_MAX_LENGTH];
		char* filename = new char[FILENAME_MAX_LENGTH];

		if(count)
			snprintf(count_buffer, COUNT_MAX_LENGTH, "%zd", *count);
		else
			snprintf(count_buffer, COUNT_MAX_LENGTH, "%zd_%zd", this->node_count, this->query_count);

		snprintf
		(
			filename, FILENAME_MAX_LENGTH, 
			"%s_"

			#ifdef WIN32
			"x86"
			#else
			"x64"
			#endif

			"_%zd_%s_%.0f_%.0f%s",
			category, this->dimensions, count_buffer, this->min_value, this->max_value, extension
		);

		string result(filename);
		delete[] count_buffer;
		delete[] filename;

		return result;
	}
	string Dataset::create_description()
	{
		char* buffer = new char[DATASET_DESCRIPTION_MAX_LENGTH];

		snprintf
		(
			buffer, DATASET_DESCRIPTION_MAX_LENGTH,
			"\n\n"
			"====================\n"
			"Dataset parameters\n"
			"====================\n"
			"\n"
			"Dimensions\t\t\t%zd\n"
			"Node count\t\t\t%zd\n"
			"Query count\t\t\t%zd\n"
			"K to return\t\t\t%zd\n"
			"Min value\t\t\t%.0f\n"
			"Max value\t\t\t%.0f\n"
			"\n"
			"powf() used\t\t\t%s\n"
			"sqrtf() used\t\t\t%s\n",
			this->dimensions,
			this->node_count,
			this->query_count,
			this->k,
			this->min_value,
			this->max_value,
			bool_to_string(this->use_pow).c_str(),
			bool_to_string(this->use_sqrt).c_str()
		);

		string description(buffer);
		delete[] buffer;
		return this->create_filename("dataset", nullptr, ".bin") + description;
	}
	void Dataset::save()
	{
		this->save(path_dataset(this->create_filename(CATEGORY_NODES)), this->node_coords, this->node_count * this->dimensions);
		this->save(path_dataset(this->create_filename(CATEGORY_QUERIES)), this->query_coords, this->query_count * this->dimensions);
	}
}
