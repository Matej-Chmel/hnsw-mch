#include "bruteforce.h"
#include "file_util.h"

namespace mch
{
	void Bruteforce::knn_search(Dataset& queries, size_t query_idx, size_t k, vector<BruteforceItem*>& items)
	{
		auto result = &this->results[query_idx];
		float* query = queries.get_coord(query_idx);

		for(auto& item : items)
			item->compute_distance_to(query, queries.dimensions);

		sort(items.begin(), items.end(), sort_cmp);

		for(size_t j = 0; j < k; j++)
			result->insert(items[j]->coords);
	}
	void Bruteforce::search(Dataset& nodes, Dataset& queries, size_t k, ProgressUpdater* updater)
	{
		this->results.resize(queries.count);

		vector<BruteforceItem*> items;
		items.reserve(nodes.count);

		if(updater == nullptr)
		{
			for(size_t i = 0; i < nodes.count; i++)
				items.push_back(new BruteforceItem(nodes.get_coord(i)));

			for(size_t i = 0; i < queries.count; i++)
				this->knn_search(queries, i, k, items);

			for(auto& item : items)
				delete item;
		}
		else
		{
			updater->start("Creating bruteforce nodes", nodes.count);

			for(size_t i = 0; i < nodes.count; i++)
			{
				items.push_back(new BruteforceItem(nodes.get_coord(i)));
				updater->update();
			}

			updater->close();
			updater->start("Bruteforce processing queries", queries.count);

			for(size_t i = 0; i < queries.count; i++)
			{
				this->knn_search(queries, i, k, items);
				updater->update();
			}

			updater->close();
			updater->start("Deleting bruteforce nodes", nodes.count);

			for(auto& item : items)
			{
				delete item;
				updater->update();
			}

			updater->close();
		}
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
	size_t Bruteforce::load(const char* path, float* nodes, size_t query_count)
	{
		size_t length;
		int* positions = load_file_data<int>(path, length);
		size_t k = length / query_count;

		this->results.resize(query_count);

		for(size_t i = 0; i < query_count; i++)
		{
			auto result = &this->results[i];
			size_t end_index = (i + 1) * k;

			for(size_t j = i * k; j < end_index; j++)
				result->insert(nodes + positions[j]);
		}

		delete[] positions;
		return k;
	}
}
