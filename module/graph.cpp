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
			if(node_idx == query_idx)
				continue;

			layer.push_back(node_idx);
			this->neighborhood(node_idx, layer_idx).push_back(query_idx);
		}
	}
	void Graph::convert_distances_to_indexes(vector<float*>& distances, vector<size_t>& out_indexes)
	{
		out_indexes.reserve(distances.size());

		for(auto& distance : distances)
			out_indexes.emplace_back(this->get_node_idx(distance));
	}
	void Graph::convert_indexes_to_distances(float* query, vector<size_t>& indexes, vector<float*>& out_distances)
	{
		out_distances.reserve(indexes.size());

		for(auto& item_idx : indexes)
			out_distances.emplace_back(this->get_distance_ptr(query, item_idx));
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
		return size_t(floorf(-logf(this->distribution(this->generator)) * this->config->ml));
	}
	float* Graph::get_distance_ptr(float* query, size_t node_idx)
	{
		if(this->relations[node_idx] != query)
		{
			this->relations[node_idx] = query;
			this->distances[node_idx] = distance_between(this->dataset->get_coord(node_idx), query, this->dataset->dimensions);
		}

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
			auto neighbors = this->select_neighbors(query, found, this->config->m, i);
			this->connect(neighbors, query_idx, i);

			for(auto& item_idx : neighbors)
			{
				auto& e_conn = this->neighborhood(item_idx, i);

				if(e_conn.size() > this->config->mmax)
					this->select_neighbors(query, e_conn, this->config->mmax, i);
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
		auto layers = this->neighbors[query_idx];

		try
		{
			return *layers->at(layer_idx);
		}
		catch(const out_of_range& e)
		{
			size_t count = (layer_idx + 1) - layers->size();

			for(size_t i = 0; i < count; i++)
				layers->emplace_back(new vector<size_t>);

			return*layers->operator[](layer_idx);
		}

		// return *(*this->neighbors[query_idx])[layer_idx];
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
	vector<size_t> Graph::select_neighbors(float* query, FurthestSet& set, size_t m, size_t layer_idx)
	{
		vector<size_t> nodes;

		if(this->config->use_heuristic)
		{
			NearestSet found(set);
			NearestSet discarded;

			if(this->config->extend_candidates)
			{
				unordered_set<size_t> visited;

				for(auto& item_distance : found)
					for(auto& item_idx : this->neighborhood(this->get_node_idx(item_distance), layer_idx))
						visited.insert(item_idx);

				for(const auto& item_idx : visited)
					found.insert(this->get_distance_ptr(query, item_idx));
			}

			nodes.reserve(m);

			if(found.size() > 0 && nodes.size() < m)
			{
				auto item_distance = found.pop_front();
				auto current_distance = *item_distance;
				nodes.push_back(this->get_node_idx(item_distance));

				while(found.size() > 0 && nodes.size() < m)
				{
					item_distance = found.pop_front();

					if(*item_distance < current_distance)
					{
						current_distance = *item_distance;
						nodes.push_back(this->get_node_idx(item_distance));
					}
					else if(this->config->keep_pruned)
						discarded.insert(item_distance);
				}
			}

			if(this->config->keep_pruned)
			{
				while(discarded.size() > 0 && nodes.size() < m)
					nodes.push_back(this->get_node_idx(discarded.pop_front()));
			}
		}
		else
		{
			auto distances = set.get_distances_copy();
			sort_heap(distances.begin(), distances.end(), furthest_cmp);
			this->convert_distances_to_indexes(distances, nodes);
			nodes.resize(m);
		}

		return nodes;
	}
	void Graph::select_neighbors(float* query, vector<size_t>& out_nodes, size_t m, size_t layer_idx)
	{
		if(this->config->use_heuristic)
		{
			NearestSet found;
			NearestSet discarded;

			if(this->config->extend_candidates)
			{
				unordered_set<size_t> visited;

				for(auto& item_idx : out_nodes)
					for(auto& adj : this->neighborhood(item_idx, layer_idx))
						visited.insert(adj);

				for(const auto& item_idx : visited)
					found.insert(this->get_distance_ptr(query, item_idx));
			}
			else
				for(const auto& item_idx : out_nodes)
					found.insert(this->get_distance_ptr(query, item_idx));

			out_nodes.clear();
			out_nodes.reserve(m);

			if(found.size() > 0 && out_nodes.size() < m)
			{
				auto item_distance = found.pop_front();
				auto current_distance = *item_distance;
				out_nodes.push_back(this->get_node_idx(item_distance));

				while(found.size() > 0 && out_nodes.size() < m)
				{
					item_distance = found.pop_front();

					if(current_distance < *item_distance)
					{
						current_distance = *item_distance;
						out_nodes.push_back(this->get_node_idx(item_distance));
					}
					else if(this->config->keep_pruned)
						discarded.insert(item_distance);
				}
			}

			if(this->config->keep_pruned)
			{
				while(discarded.size() > 0 && out_nodes.size() < m)
					out_nodes.push_back(this->get_node_idx(discarded.pop_front()));
			}
		}
		else
		{
			vector<float*> distances;
			this->convert_indexes_to_distances(query, out_nodes, distances);
			sort(distances.begin(), distances.end(), furthest_cmp);
			out_nodes.clear();
			this->convert_distances_to_indexes(distances, out_nodes);
			out_nodes.resize(m);
		}
	}
	Graph::Graph(Config* config, Dataset* dataset, size_t seed):
		config(config), dataset(dataset), distribution(0, 1), entry_idx(0), entry_level(0), generator(seed)
	{
		#ifdef DEBUG_GRAPH
			this->debug_file = fopen("..\\native_app\\debug_files\\a.txt", "w+");
		#endif
	}
	Graph::~Graph()
	{
		#ifdef DEBUG_GRAPH
			this->add_neighbors_record();
			fclose(this->debug_file);
		#endif

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

	#ifdef DEBUG_GRAPH

	void Graph::add_neighbors_record()
	{
		this->neighbors_to_file(this->debug_file);
	}
	void Graph::neighbors_to_file(FILE* file)
	{
		auto neighbors_string = this->neighbors_to_string();
		fwrite(neighbors_string.c_str(), sizeof(char), neighbors_string.length(), file);
	}
	string Graph::neighbors_to_string()
	{
		size_t node_count = this->neighbors.size();
		string result = "";
		
		for(size_t i = 0; i < node_count; i++)
		{
			auto& layers = this->neighbors[i];
			result += to_string(i);

			for(auto& layer : *layers)
			{
				result += "\n\t";

				if(layer->size() == 0)
				{
					result += "empty";
					continue;
				}

				auto sorted_layer = *layer;
				size_t last_index = sorted_layer.size() - 1;
				sort(sorted_layer.begin(), sorted_layer.end());

				for(size_t i = 0; i < last_index; i++)
					result += to_string(sorted_layer[i]) + ' ';
				result += to_string(sorted_layer[last_index]);
			}

			result += "\n\n";
		}

		return result;
	}

	#endif
}
