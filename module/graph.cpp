#include "distance.h"
#include "graph.h"

namespace mch
{
	void Graph::approx_search(float* query, size_t ef, size_t k, vector<float*>& output)
	{
		size_t node_idx = this->entry_idx;

		for(size_t i = this->entry_level; i >= 1; i--)
			this->search_layer_one(query, node_idx, i);

		FurthestSet found(this->distances.data() + node_idx);
		this->search_layer(query, found, ef, 0);
		found.keep_only_k_nearest(k);

		output.reserve(k);

		for(auto& distance : found)
			output.emplace_back(this->dataset->get_coord(this->get_node_idx(distance)));
	}
	void Graph::connect(vector<size_t>& nodes, size_t query_idx, size_t layer_idx)
	{
		auto& layer = this->neighborhood(query_idx, layer_idx);

		for(auto& node_idx : nodes)
		{
			layer.push_back(node_idx);
			this->neighborhood(node_idx, layer_idx).push_back(query_idx);
		}
	}
	size_t Graph::create_node(size_t level)
	{
		size_t count = level + 1;
		auto layers = this->neighbors.emplace_back(new vector<vector<size_t>*>);

		layers->reserve(count);

		for(size_t i = 0; i < count; i++)
			layers->emplace_back(new vector<size_t>);

		return this->neighbors.size() - 1;
	}
	size_t Graph::generate_level()
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dist(0, 1);

		return size_t(floorf(-logf(dist(gen)) * this->config->ml));
	}
	float* Graph::get_distance_ptr(float* query, size_t node_idx)
	{
		if(this->relations[node_idx] == query)
			return this->distances.data() + node_idx;

		this->relations[node_idx] = query;
		this->distances[node_idx] = distance_between(this->dataset->get_coord(node_idx), query, this->dataset->dimensions);
		return this->distances.data() + node_idx;
	}
	size_t Graph::get_node_idx(float* distance_ptr)
	{
		return distance_ptr - this->distances.data();
	}
	void Graph::insert(float* query)
	{
		size_t node_idx = this->entry_idx;
		size_t query_level = this->generate_level();
		size_t query_idx = this->create_node(query_level);

		for(size_t i = this->entry_level; i >= query_level + 1; i--)
			this->search_layer_one(query, node_idx, i);

		FurthestSet found(this->get_distance_ptr(query, node_idx));

		for(size_t i = min(this->entry_level, query_level);; i--)
		{
			this->search_layer(query, found, this->config->ef, i);
			auto neighbors = this->select_neighbors(found.get_distances_copy());
			this->connect(neighbors, query_idx, i);

			for(auto& item_idx : neighbors)
			{
				auto& e_conn = this->neighborhood(item_idx, i);

				if(e_conn.size() > this->config->mmax)
					this->select_neighbors(query, e_conn);
			}

			if(i == 0)
				break;
		}

		if(query_level > this->entry_level)
		{
			this->entry_idx = query_idx;
			this->entry_level = query_level;
		}
	}
	vector<size_t>& Graph::neighborhood(size_t query_idx, size_t layer_idx)
	{
		return *(*this->neighbors[query_idx])[layer_idx];
	}
	void Graph::search_layer(float* query, FurthestSet& out_entries, size_t ef, size_t layer_idx)
	{
		NearestSet candidates(out_entries);
		unordered_set<size_t> visited;

		visited.reserve(out_entries.size());

		for(auto& item : out_entries)
			visited.insert(this->get_node_idx(item));

		while(candidates.size() > 0)
		{
			auto nearest_candidate = candidates.pop_front();
			auto furthest_found = out_entries.front();

			if(*nearest_candidate > *furthest_found)
				break;

			for(auto& item_idx : this->neighborhood(this->get_node_idx(nearest_candidate), layer_idx))
				if(visited.insert(item_idx).second)
				{
					furthest_found = out_entries.front();
					auto item_distance = this->get_distance_ptr(query, item_idx);

					if(*item_distance < *furthest_found || out_entries.size() < ef)
					{
						candidates.insert(item_distance);
						out_entries.insert(item_distance);

						if(out_entries.size() > ef)
							out_entries.remove_front();
					}
				}
		}
	}
	void Graph::search_layer_one(float* query, size_t& out_node_idx, size_t layer_idx)
	{
		NearestSet candidates;
		float current_distance = *this->get_distance_ptr(query, out_node_idx);
		unordered_set<size_t> visited;

		visited.insert(out_node_idx);

		{
			auto& neighbors = this->neighborhood(out_node_idx, layer_idx);
			for(auto& item_idx : neighbors)
			{
				visited.insert(item_idx);
				auto item_distance = this->get_distance_ptr(query, item_idx);

				if(*item_distance < current_distance)
				{
					candidates.insert(item_distance);
					out_node_idx = item_idx;
					current_distance = *item_distance;
				}
			}
		}

		while(candidates.size() > 0)
		{
			auto nearest_candidate = candidates.pop_front();

			if(*nearest_candidate > current_distance)
				break;

			auto& neighbors = this->neighborhood(this->get_node_idx(nearest_candidate), layer_idx);
			for(auto& item_idx : neighbors)
			{
				if(visited.insert(item_idx).second)
				{
					auto item_distance = this->get_distance_ptr(query, item_idx);

					if(*item_distance < current_distance)
					{
						candidates.insert(item_distance);
						out_node_idx = item_idx;
						current_distance = *item_distance;
					}
				}
			}
		}
	}
	vector<size_t> Graph::select_neighbors(vector<float*>&& distances)
	{
		if(distances.size() > this->config->m)
		{
			sort_heap(distances.begin(), distances.end(), furthest_cmp);
			distances.resize(this->config->m);
		}

		vector<size_t> neighbors;
		neighbors.reserve(distances.size());

		for(auto& distance : distances)
			neighbors.emplace_back(this->get_node_idx(distance));

		return neighbors;
	}
	void Graph::select_neighbors(float* query, vector<size_t>& out_nodes)
	{
		if(out_nodes.size() > this->config->mmax)
		{
			vector<float*> distances;
			distances.reserve(out_nodes.size());

			for(auto& item_idx : out_nodes)
				distances.emplace_back(this->get_distance_ptr(query, item_idx));

			sort(distances.begin(), distances.end(), furthest_cmp);
			distances.resize(this->config->mmax);

			out_nodes.clear();

			for(auto& distance : distances)
				out_nodes.emplace_back(this->get_node_idx(distance));
		}
	}
	Graph::Graph(Config* config, Dataset* dataset): config(config), dataset(dataset), entry_idx(0), entry_level(0)
	{}
	Graph::~Graph()
	{
		for(auto& layers : this->neighbors)
		{
			for(auto& layer : *layers)
				delete layer;
			delete layers;
		}
	}
	void Graph::build(ProgressUpdater* updater)
	{
		this->build(0, this->dataset->count, updater);
	}
	void Graph::build(size_t start_idx, size_t end_idx, ProgressUpdater* updater)
	{
		size_t count = end_idx - start_idx;

		this->distances.resize(count);
		this->neighbors.reserve(count);
		this->relations.resize(count);

		if(updater != nullptr)
			updater->start("Inserting nodes", count);

		if(start_idx == 0)
		{
			this->entry_level = this->generate_level();
			this->create_node(this->entry_level);
			start_idx++;

			if(updater != nullptr)
				updater->update();
		}

		if(updater == nullptr)
		{
			for(size_t i = start_idx; i < end_idx; i++)
				this->insert(this->dataset->get_coord(i));
		}
		else
		{
			for(size_t i = start_idx; i < end_idx; i++)
			{
				this->insert(this->dataset->get_coord(i));
				updater->update();
			}

			updater->close();
		}
	}
	vector<vector<float*>> Graph::search(Dataset& dataset, size_t ef, size_t k, ProgressUpdater* updater)
	{
		vector<vector<float*>> results;
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
}
