#include "bruteforce.h"
#include "file_util.h"

namespace mch
{
	void Bruteforce::search(Dataset& nodes, Dataset& queries, size_t k)
	{
		this->results.resize(queries.count);

		vector<BruteforceItem*> items;
		items.reserve(nodes.count);

		for(size_t i = 0; i < nodes.count; i++)
			items.push_back(new BruteforceItem(nodes.get_coord(i)));

		for(size_t i = 0; i < queries.count; i++)
		{
			auto result = &this->results[i];
			float* query = queries.get_coord(i);

			for(auto& item : items)
				item->compute_distance_to(query, nodes.dimensions);

			sort(items.begin(), items.end(), sort_cmp);

			for(size_t j = 0; j < k; j++)
				result->insert(items[j]->coords);
		}

		for(auto& item : items)
			delete item;
	}
	float Bruteforce::compare(vector<FurthestSet>& approx_results)
	{
		float correct = 0;
		size_t k = this->results[0].size();
		size_t query_count = approx_results.size();

		for(size_t i = 0; i < query_count; i++)
		{
			auto bruteforce_result = &this->results[i];
			auto& approx_result = approx_results[i];

			for(auto& item : approx_result)
				if(bruteforce_result->find(item->coords) != bruteforce_result->end())
					correct++;
		}

		return correct / (query_count * k);
	}
	void Bruteforce::load(const char* path, float* nodes, size_t query_count, size_t k)
	{
		size_t bruteforce_length;
		int* bruteforce_positions = load_file_data<int>(path, bruteforce_length);

		this->results.resize(query_count);

		for(size_t i = 0; i < query_count; i++)
		{
			auto result = &this->results[i];
			size_t end_index = (i + 1) * k;

			for(size_t j = i * k; j < end_index; j++)
				result->insert(nodes + bruteforce_positions[j]);
		}

		delete[] bruteforce_positions;
	}
}
