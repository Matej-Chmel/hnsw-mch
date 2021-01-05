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
		vector<size_t> correct_counts;
		size_t k = this->results[0].size();
		size_t query_count = approx_results.size();

		correct_counts.reserve(query_count);

		for(size_t i = 0; i < query_count; i++)
		{
			size_t correct = 0;
			auto bruteforce_result = &this->results[i];
			auto& approx_result = approx_results[i];

			for(auto& item : approx_result)
				if(bruteforce_result->find(item->coords) != bruteforce_result->end())
					correct++;

			correct_counts.push_back(correct);
		}

		sort(correct_counts.begin(), correct_counts.end());

		if(query_count % 2)
		{
			size_t half = query_count / 2;
			float a = float(correct_counts[half] + correct_counts[half + 1]);
			return a / 2 / k * 100;
		}
		else
			return float(correct_counts[query_count / 2]) / k * 100;
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
