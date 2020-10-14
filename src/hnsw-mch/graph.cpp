#include "graph.h"
#include "progress.h"

namespace mch
{
	GraphParams::GraphParams(size_t ef, size_t m, float ml, size_t mmax, bool use_heuristic, bool extend_candidates, bool keep_pruned):
		ef(ef), m(m), ml(ml), mmax(mmax), use_heuristic(use_heuristic), extend_candidates(extend_candidates), keep_pruned(keep_pruned)
	{}
	void GraphParams::set(string key, void* value)
	{
		if(key == r_ef)
			this->ef = *(size_t*)value;
		else if(key == r_m)
			this->m = *(size_t*)value;
		else if(key == r_ml)
			this->ml = *(float*)value;
		else if(key == r_mmax)
			this->mmax = *(size_t*)value;
		else if(key == r_use_heuristic)
			this->use_heuristic = *(bool*)value;
		else if(key == r_extend_candidates)
			this->extend_candidates = *(bool*)value;
		else
			this->keep_pruned = *(bool*)value;
	}
	BuildMeasurement::BuildMeasurement(char* name):
		name(name), build_ms(0), approx_ms(0), median_accuracy(0)
	{}
	BuildMeasurement::BuildMeasurement(BuildMeasurement&& moved):
		name(moved.name), build_ms(moved.build_ms), approx_ms(moved.approx_ms), median_accuracy(moved.median_accuracy)
	{
		moved.name = nullptr;
	}
	BuildMeasurement::~BuildMeasurement()
	{
		delete[] this->name;
	}
	void Graph::set_entry(Node* query)
	{
		this->entry = query;
		this->entry_level = query->level();
	}
	size_t Graph::generate_level()
	{
		static random_device rd;
		static mt19937_64 gen(rd());
		static uniform_real_distribution<float> dist(0, 1);

		return size_t(floorf(-logf(dist(gen)) * this->params.ml));
	}
	void Graph::insert(Node* query)
	{
		size_t query_level = this->generate_level();
		query->init(query_level);

		HeapItem point = { this->entry, this->entry->compute_distance_to(query->coords, this->data->dimensions) };
		for(size_t i = this->entry_level; i > query_level; i--)
			point = this->search_one(query->coords, point, i);

		HeapSet<furthest> found(point);
		for(size_t i = min(this->entry_level, query_level);; i--)
		{
			this->search_layer(query->coords, found, i);
			auto neighbors = (this->*select_neighbors)(query, found, this->params.m, i);
			query->connect(neighbors, i);

			for(auto item : neighbors)
			{
				auto e_conn = query->neighborhood(i);
				if(e_conn->size() > this->params.mmax)
					query->set_neighborhood
					(
						(this->*select_neighbors)(query, HeapSet<nearest>(*e_conn, query->coords, this->data->dimensions), this->params.mmax, i), i
					);
			}

			if(!i)
				break;
		}

		if(query_level > this->entry_level)
			this->set_entry(query);
	}
	HeapItem Graph::search_one(float* query, HeapItem entry, size_t layer_idx)
	{
		unordered_set<Node*> visited;
		visited.insert(entry.node);
		HeapSet<nearest> candidates(entry);

		while(candidates.size())
		{
			auto nearest_cand = candidates.pop_front();

			if(nearest_cand.distance > entry.distance)
				break;

			auto neighbors = nearest_cand.node->neighborhood(layer_idx);
			for(auto item : *neighbors)
			{
				if(visited.insert(item).second)
				{
					float item_distance = item->compute_distance_to(query, this->data->dimensions);

					if(item_distance < entry.distance)
					{
						candidates.insert(item, item_distance);
						entry.node = item;
						entry.distance = item_distance;
					}
				}
			}
		}

		return entry;
	}
	void Graph::search_layer(float* query, HeapSet<furthest>& found, size_t layer_idx)
	{
		unordered_set<Node*> visited;

		visited.reserve(found.size());
		for(auto item : found)
			visited.insert(item.node);

		HeapSet<nearest> candidates(found);

		while(candidates.size())
		{
			auto nearest_cand = candidates.pop_front();
			auto furthest_found = found.front();

			if(nearest_cand.distance > furthest_found.distance)
				break;

			auto neighbors = nearest_cand.node->neighborhood(layer_idx);
			for(auto item : *neighbors)
			{
				if(visited.insert(item).second)
				{
					float item_distance = item->compute_distance_to(query, this->data->dimensions);
					auto furthest_found = found.front();

					if(item_distance < furthest_found.distance || found.size() < this->params.ef)
					{
						candidates.insert(item, item_distance);
						found.insert(item, item_distance);

						if(found.size() > this->params.ef)
							found.remove_front();
					}
				}
			}
		}
	}
	vector<Node*> Graph::select_neighbors_simple(Node* query, HeapSet<nearest> candidates, size_t m_to_return, size_t layer_idx)
	{
		return candidates.k_sorted_nodes(this->data->k_to_return);
	}
	vector<Node*> Graph::select_neighbors_heuristic(Node* query, HeapSet<nearest> candidates, size_t m_to_return, size_t layer_idx)
	{
		HeapSet<nearest> result;
		HeapSet<nearest> working_queue;

		if(this->params.extend_candidates)
		{
			unordered_set<Node*> visited;

			visited.reserve(candidates.size());
			for(auto item : candidates)
				visited.insert(item.node);

			for(auto item : candidates)
			{
				auto neighbors = item.node->neighborhood(layer_idx);
				for(auto adjacent : *neighbors)
					visited.insert(adjacent);
			}

			for(const auto item : visited)
				working_queue.insert(item, query->coords, this->data->dimensions);
		}
		else working_queue.init(candidates);

		if(this->params.keep_pruned)
		{
			HeapSet<nearest> discarded;

			while(working_queue.size() && result.size() < this->params.m)
			{
				auto nearest_item = working_queue.pop_front();

				if(nearest_item.distance < result.front().distance)
					result.insert(nearest_item);
				else discarded.insert(nearest_item);
			}

			while(discarded.size() && result.size() < this->params.m)
				result.insert(discarded.pop_front());
		}
		else
		{
			while(working_queue.size() && result.size() < this->params.m)
			{
				auto nearest_item = working_queue.pop_front();

				if(nearest_item.distance < result.front().distance)
					result.insert(nearest_item);
			}
		}

		return result.to_vector();
	}
	vector<float*> Graph::approx_search(float* query)
	{
		HeapItem point = {this->entry, this->entry->compute_distance_to(query, this->data->dimensions)};

		for(size_t i = this->entry_level; i > 0; i--)
			point = this->search_one(query, point, i);

		HeapSet<furthest> found(point);
		this->search_layer(query, found, 0);
		return found.k_sorted_coords(this->data->k_to_return);
	}
	void Graph::update_params()
	{
		if(this->params.use_heuristic)
			this->select_neighbors = &Graph::select_neighbors_heuristic;
		else this->select_neighbors = &Graph::select_neighbors_simple;
	}
	Graph::Graph(Dataset* data, GraphParams params):
		data(data), entry(nullptr), entry_level(0), params(params), select_neighbors(nullptr)
	{
		this->nodes.reserve(data->n_elements);

		for(size_t i = 0; i < data->n_elements; i++)
			this->nodes.emplace_back(data->node_coords + i * data->dimensions);
	}
	BuildMeasurement Graph::build(char* name, ExclusiveBar& progress)
	{
		BuildMeasurement output(name);
		vector<vector<float*>> results;
		vector<size_t> target;

		results.reserve(this->data->n_queries);
		target.reserve(this->data->n_queries);
		this->update_params();

		PERF_CODE;

		PERF
		{
			this->nodes.front().init(this->generate_level());
			this->set_entry(&this->nodes.front());
			
			for(size_t i = 1; i < this->data->n_elements; i++)
			{
				this->insert(&this->nodes[i]);
				progress.update(1);
			}
		}
		DONE(output.build_ms);

		PERF
		{
			for(size_t i = 0; i < this->data->n_queries; i++)
			{
				results.push_back(this->approx_search(this->data->query_coords + i * this->data->dimensions));
				progress.update(1);
			}
		}
		DONE(output.approx_ms);

		for(size_t i = 0; i < this->data->n_queries; i++)
		{
			size_t correct = 0;
			auto approx = results[i];
			auto bruteforce = &this->data->results[i];
			auto end = bruteforce->end();

			for(auto item : approx)
				if(bruteforce->find(item) != end)
					correct++;

			target.emplace_back(correct);
		}

		sort(target.begin(), target.end());

		size_t length = target.size();
		size_t half = length / 2;
		if(length % 2)
			output.median_accuracy = float(target[half + 1]) / float(this->data->k_to_return) * 100.f;
		else output.median_accuracy = float(target[half] + target[half + 1]) / 2.f / float(this->data->k_to_return) * 100.f;

		return move(output);
	}
}
