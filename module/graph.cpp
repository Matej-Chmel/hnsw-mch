#include "graph.h"

namespace mch
{
	Graph::Graph(Config* config): config(config), dimensions(0), entry(nullptr), entry_level(0)
	{}
	size_t Graph::generate_level()
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dist(0, 1);

		return size_t(floorf(-logf(dist(gen)) * this->config->ml));
	}
	void Graph::insert(float* query_coords)
	{
		Node* point = this->entry;
		size_t query_level = this->generate_level();
		Node* query = &this->nodes.emplace_back(query_coords, query_level);

		point->compute_distance_to(query_coords, this->dimensions);

		for(size_t i = this->entry_level; i >= query_level + 1; i--)
			this->search_layer_one(query_coords, point, i);

		FurthestSet found(point);

		for(size_t i = min(this->entry_level, query_level);; i--)
		{
			this->search_layer(query_coords, found, this->config->ef, i);
			auto neighbors = found.copy_nodes();
			this->select_neighbors(query_coords, neighbors, this->config->m, i);
			query->connect(neighbors, i);

			for(auto& item : neighbors)
			{
				auto& e_conn = item->neighborhood(i);

				if(e_conn.size() > this->config->mmax)
				{
					for(auto& adj : e_conn)
						adj->compute_distance_to(query_coords, this->dimensions);
					this->select_neighbors(query_coords, e_conn, this->config->mmax, i);
				}
			}

			if(i == 0)
				break;
		}

		if(query_level > this->entry_level)
		{
			this->entry = query;
			this->entry_level = query_level;
		}
	}
	void Graph::search_layer_one(float* query, Node*& out_entry, size_t layer_idx)
	{
		NearestSet candidates;
		unordered_set<Node*> visited;

		visited.insert(out_entry);

		{
			auto& neighbors = out_entry->neighborhood(layer_idx);
			for(auto& item : neighbors)
			{
				visited.insert(item);
				item->compute_distance_to(query, this->dimensions);

				if(item->distance < out_entry->distance)
				{
					candidates.insert(item);
					out_entry = item;
				}
			}
		}

		while(candidates.size() > 0)
		{
			auto nearest_candidate = candidates.pop_front();

			if(nearest_candidate->distance > out_entry->distance)
				break;

			auto& neighbors = nearest_candidate->neighborhood(layer_idx);
			for(auto& item : neighbors)
			{
				if(visited.insert(item).second)
				{
					item->compute_distance_to(query, this->dimensions);

					if(item->distance < out_entry->distance)
					{
						candidates.insert(item);
						out_entry = item;
					}
				}
			}
		}
	}
	void Graph::search_layer(float* query, FurthestSet& out_entries, size_t ef, size_t layer_idx)
	{
		NearestSet candidates(out_entries);
		unordered_set<Node*> visited;

		visited.reserve(out_entries.size());

		for(auto& item : out_entries)
			visited.insert(item);

		while(candidates.size() > 0)
		{
			auto nearest_candidate = candidates.pop_front();
			auto furthest_found = out_entries.front();

			if(nearest_candidate->distance > furthest_found->distance)
				break;

			for(auto& item : nearest_candidate->neighborhood(layer_idx))
				if(visited.insert(item).second)
				{
					item->compute_distance_to(query, this->dimensions);
					furthest_found = out_entries.front();

					if(item->distance < furthest_found->distance || out_entries.size() < ef)
					{
						candidates.insert(item);
						out_entries.insert(item);

						if(out_entries.size() > ef)
							out_entries.remove_front();
					}
				}
		}
	}
	void Graph::select_neighbors(float* query, vector<Node*>& out_candidates, size_t m, size_t layer_idx)
	{
		if(this->config->use_heuristic)
		{
			NearestSet found;
			NearestSet discarded;
			
			if(this->config->extend_candidates)
			{
				unordered_set<Node*> visited;

				for(auto& element : out_candidates)
					for(auto& adj : element->neighborhood(layer_idx))
						visited.insert(adj);

				for(const auto& item : visited)
					found.insert(item);
			}
			else
				found.init(out_candidates);

			if(found.size() > 0 && out_candidates.size() < m)
			{
				auto item = found.pop_front();
				auto distance = item->distance;
				out_candidates.push_back(item);

				while(found.size() > 0 && out_candidates.size() < m)
				{
					item = found.pop_front();

					if(item->distance < distance)
					{
						auto distance = item->distance;
						out_candidates.push_back(item);
					}
					else if(this->config->keep_pruned)
						discarded.insert(item);
				}
			}

			if(this->config->keep_pruned)
			{
				while(discarded.size() > 0 && out_candidates.size() < m)
					out_candidates.push_back(discarded.pop_front());
			}
		}
		else
		{
			if(out_candidates.size() > m)
			{
				sort(out_candidates.begin(), out_candidates.end(), furthest_cmp);
				out_candidates.resize(m);
			}
		}
	}
	void Graph::approx_search(float* query, size_t ef, size_t k, FurthestSet& output)
	{
		Node* point = this->entry;

		for(size_t i = this->entry_level; i >= 1; i--)
			this->search_layer_one(query, point, i);

		output.insert(point);
		this->search_layer(query, output, ef, 0);
		output.keep_only_k_nearest(k);
	}
	void Graph::build(Dataset& dataset, ProgressUpdater* updater)
	{
		this->dimensions = dataset.dimensions;
		this->nodes.reserve(dataset.count);
		this->entry_level = this->generate_level();
		this->entry = &this->nodes.emplace_back(dataset.get_coord(0), this->entry_level);

		if(updater == nullptr)
		{
			for(size_t i = 1; i < dataset.count; i++)
				this->insert(dataset.get_coord(i));
		}
		else
		{
			updater->start("Inserting nodes", dataset.count);
			updater->update();

			for(size_t i = 1; i < dataset.count; i++)
			{
				this->insert(dataset.get_coord(i));
				updater->update();
			}

			updater->close();
		}
	}
	vector<FurthestSet> Graph::search_all(Dataset& dataset, size_t ef, size_t k, ProgressUpdater* updater)
	{
		vector<FurthestSet> results;
		results.resize(dataset.count);

		if(updater == nullptr)
		{
			for(size_t i = 0; i < dataset.count; i++)
				this->approx_search(dataset.get_coord(i), ef, k, results[i]);
		}
		else
		{
			updater->start("Graph is processing queries", dataset.count);

			for(size_t i = 0; i < dataset.count; i++)
			{
				this->approx_search(dataset.get_coord(i), ef, k, results[i]);
				updater->update();
			}

			updater->close();
		}

		return results;
	}
	string Graph::to_string()
	{
		string result = "";

		for(auto& item : this->nodes)
			result += item.to_string();

		return result;
	}
	void Graph::print()
	{
		printf("%s\n", this->to_string().c_str());
	}
}
