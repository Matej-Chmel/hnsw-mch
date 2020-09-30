#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include "graph.hpp"
using namespace std::chrono;

namespace mch
{
	Graph::Graph(DatasetParams* data): data(data), params(nullptr), entry(nullptr), entry_level(0)
	{
		this->nodes.reserve(this->data->n_elements);

		random_device rd;
		mt19937_64 gen(rd());
		uniform_real_distribution<float> dist(this->data->min, this->data->max);

		auto gen_start = system_clock::now();
		{
			size_t id = 0;
			this->node_coords = new float[this->data->dimensions * this->data->n_elements];
			for(size_t i = 0; i < this->data->n_elements; i++)
			{
				this->nodes.push_back(new Node(id++, this->node_coords + i * this->data->dimensions));
				float* end = this->node_coords + (i + 1) * this->data->dimensions;
				for(float* ptr = this->node_coords; ptr != end; ptr++)
					*ptr = dist(gen);
			}
		}
		auto gen_end = system_clock::now();
		this->gen_measurement.node_ms = duration_cast<milliseconds>(gen_end - gen_start).count();

		gen_start = system_clock::now();
		{
			size_t n_coords = this->data->dimensions * this->data->n_queries;
			this->query_coords = new float[n_coords];
			float* end = this->query_coords + n_coords;
			for(float* ptr = this->query_coords; ptr != end; ptr++)
				*ptr = dist(gen);
		}
		gen_end = system_clock::now();
		this->gen_measurement.query_ms = duration_cast<milliseconds>(gen_end - gen_start).count();
	}
	Graph::~Graph()
	{
		delete[] this->node_coords;
		delete[] this->query_coords;
		for(SetItem*& item: this->set_items)
			delete item;
	}
	BuildMeasurement Graph::build(GraphParams* params)
	{
		this->params = params;
		if (this->params->use_heuristic)
			this->select_neighbors = &Graph::select_neighbors_heuristic;
		else this->select_neighbors = &Graph::select_neighbors_simple;

		BuildMeasurement measurement;

		auto start = system_clock::now();
		{
			this->set_entry(this->nodes.front(), this->generate_level());
				for(size_t i = 1; i < this->data->n_elements; i++)
					this->insert(this->nodes[i]);
		}
		auto end = system_clock::now();
		measurement.build_ms = duration_cast<milliseconds>(end - start).count();

		vector<size_t> target;
		target.reserve(this->data->n_queries);
		vector<layer> approx_results = this->approx_search(measurement);
		for(size_t i = 0; i < this->data->n_queries; i++)
		{
			size_t correct = 0;
			layer query_result = approx_results[i];
			unordered_set<Node*> bruteforce_result = this->bruteforce_results[i];
			unordered_set<Node*>::iterator bruteforce_end = bruteforce_result.end();

			for(Node*& node: query_result)
				if (bruteforce_result.find(node) != bruteforce_end)
					correct++;
			
			target[i] = correct;
		}

		sort(target.begin(), target.end());
		if (this->data->n_queries % 2)
			measurement.median_accuracy = target[this->data->n_queries / 2 + 1];
		else
		{
			size_t index = this->data->n_queries / 2;
			measurement.median_accuracy = (target[index] + target[index + 1]) / 2;
		}

		return measurement;
	}
	void Graph::bruteforce()
	{
		auto start = system_clock::now();
		{
			this->bruteforce_results.resize(this->data->n_queries);
			vector<unordered_set<Node*>>::iterator iter = this->bruteforce_results.begin();

			float* query_end = this->query_coords + this->data->dimensions * this->data->n_queries;
			for(float* query = this->query_coords; query != query_end; query += this->data->dimensions, iter++)
			{
				Set<nearest> set;
				for(Node*& item: this->nodes)
					set.insert(this->new_set_item(item, query));
				
				unordered_set<Node*> neighbors = *iter;
				for(size_t i = 0; i < this->params->k_to_return; i++)
					neighbors.insert(set.pop_front()->node);
			}
		}
		auto end = system_clock::now();
		this->gen_measurement.bruteforce_ms = duration_cast<milliseconds>(end - start).count();
	}
	vector<layer> Graph::approx_search(BuildMeasurement& measurement) 
	{
		auto start = system_clock::now();
		vector<layer> output;
		output.reserve(this->data->n_queries);
		{
			float* query_end = this->query_coords + this->data->dimensions * this->data->n_queries;
			for(float* query = this->query_coords; query != query_end; query += this->data->dimensions)
				output.push_back(this->approx_search_one(query));
		}
		auto end = system_clock::now();
		measurement.approx_ms = duration_cast<milliseconds>(end - start).count();
		return output;
	}
	void Graph::insert(Node* node)
	{
		SetItem* nearest_item = this->new_set_item(this->entry, node->coords);
		size_t node_level = this->generate_level();
		node->init(node_level);

		for(size_t i = this->entry_level; i > node_level; i--)
			nearest_item = this->search_layer(node->coords, Set<furthest>(nearest_item), 1, i).nearest();
		
		Set<furthest> found(nearest_item);
		for(size_t i = min(this->entry_level, node_level);; i--)
		{
			found = this->search_layer(node->coords, found, this->params->ef, i);
			layer neighbors = (this->*select_neighbors)(node, found, this->params->m, i);
			node->connect(neighbors, i);

			for(Node*& adj: neighbors)
			{
				layer conn = adj->neighborhood(i);
				if (conn.size() > this->params->mmax)
				{
					Set set = this->set_from_layer(conn, node->coords);
					adj->neighborhood(i) = (this->*select_neighbors)(adj, set, this->params->mmax, i);
				}
			}

			if (i == 0)
				break;
		}

		if (node_level > this->entry_level)
			this->set_entry(node, node_level);
	}
	Set<furthest> Graph::search_layer(float* query, Set<furthest> found, size_t ef, size_t layer_idx)
	{
		unordered_set<Node*> visited;
		visited.reserve(found.size());
		for(SetItem*& item: found)
			visited.insert(item->node);

		Set<nearest> candidates(found);

		while(candidates.size() > 0)
		{
			SetItem* nearest_cand = candidates.pop_front();
			SetItem* furthest_found = found.front();

			if (nearest_cand->distance > furthest_found->distance)
				break;
			
			layer neighbors = nearest_cand->neighborhood(layer_idx);
			for(Node*& item: neighbors)
			{
				if (visited.insert(item).second)
				{
					SetItem* adj = this->new_set_item(item, query);
					if (adj->distance < found.front()->distance || found.size() < this->params->ef)
					{
						candidates.insert(adj);
						found.insert(adj);
						if (found.size() > this->params->ef)
							found.pop_front();
					}
				}
			}
		}

		return found;
	}
	layer Graph::select_neighbors_simple(Node* query, Set<furthest>& candidates, size_t m_to_return, size_t)
	{
		return candidates.k_nearest(m_to_return);
	}
	layer Graph::select_neighbors_heuristic(Node* query, Set<furthest>& candidates, size_t m_to_return, size_t layer_idx)
	{
		Set<nearest> result;
		Set<nearest> found(candidates);

		if (this->params->extend_candidates)
		{
			unordered_set<Node*> helper = found.create_set();
			unordered_set<Node*>::iterator helper_end = helper.end();

			for(SetItem*& item: candidates)
			{
				layer neighbors = item->neighborhood(layer_idx);
				for(Node*& adj: neighbors)
					if (helper.find(adj) == helper_end)
						found.insert(this->new_set_item(adj, query->coords));
			}
		}

		if (this->params->keep_pruned)
		{
			while(found.size() > 0 && result.size() < this->params->m)
			{
				SetItem* nearest_item = found.pop_front();
				if (!result.size() || nearest_item->distance < result.nearest()->distance)
					result.insert(nearest_item);
			}
		}
		else
		{
			Set<nearest> discarded;

			while(found.size() > 0 && result.size() < this->params->m)
			{
				SetItem* nearest_item = found.pop_front();
				if (!result.size() || nearest_item->distance < result.nearest()->distance)
					result.insert(nearest_item);
				else discarded.insert(nearest_item);
			}

			while(discarded.size() > 0 && result.size() < this->params->m)
				result.insert(discarded.pop_front());
		}

		return result.k_nearest(result.size());
	}
	layer Graph::approx_search_one(float* query)
	{
		SetItem* nearest_item = this->new_set_item(this->entry, query);
		for(size_t i = this->entry_level; i > 0; i--)
			nearest_item = this->search_layer(query, Set<nearest>(nearest_item), 1, i).nearest();
		return this->search_layer(query, Set<nearest>(nearest_item), this->params->ef, 0).k_nearest(this->params->k_to_return);
	}
	void Graph::set_entry(Node* query, size_t level) 
	{
		this->entry = query;
		this->entry_level = level;
	}
	size_t Graph::generate_level()
	{
		static random_device rd;
		static mt19937_64 gen(rd());
		static uniform_real_distribution<float> dist(0.0f, 1.0f);
		return floor(-log(dist(gen)) * this->params->ml);
	}
	SetItem* Graph::new_set_item(Node* node, float* query)
	{
		float distance = 0.0f;
		float* end = query + this->data->dimensions;
		for(float* node_coord = node->coords; query != end; node_coord++, query++)
			distance += powf(*node_coord - *query, 2);
		SetItem* item = new SetItem(node, sqrtf(distance));
		this->set_items.push_back(item);
		return item;
	}
	Set<furthest> Graph::set_from_layer(layer& nodes, float* query)
	{
		Set<furthest> set;
		set.items.reserve(nodes.size());
		for(Node*& node: nodes)
			set.insert(this->new_set_item(node, query));
		return set;
	}
	template<set_type type>
	bool Graph::insert_unique_into(Set<type>& item_set, Node* node, float* query, unordered_set<Node*>& node_set)
	{
		if (!node_set.insert(node).second)
			return false;
		item_set.insert(this->new_set_item(node, query));
		return true;
	}
}
