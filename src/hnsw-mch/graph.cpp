#include "file_util.h"
#include "graph.h"
#include "progress_bar.h"
#include "util.h"

namespace mch
{
	constexpr size_t GRAPH_DESCRIPTION_MAX_LENGTH = 512;
	constexpr size_t MEASUREMENT_DESCRIPTION_MAX_LENGTH = 512;

	Graph::Graph(Dataset* dataset, size_t ef, size_t m, size_t mmax, float ml, bool use_heuristic, bool extend_candidates, bool keep_pruned):
		ef(ef), m(m), mmax(mmax), ml(ml), use_heuristic(use_heuristic), extend_candidates(extend_candidates), keep_pruned(keep_pruned),
		measurement(), dataset(dataset), nodes(dataset->create_nodes()), entry(nullptr)
	{}
	Graph::Graph(const Graph& other):
		ef(other.ef), m(other.m), mmax(other.mmax), ml(other.ml),
		use_heuristic(other.use_heuristic), extend_candidates(other.extend_candidates), keep_pruned(other.keep_pruned),
		measurement(), dataset(other.dataset), nodes(other.dataset->create_nodes()), entry(nullptr)
	{}
	Graph::~Graph()
	{
		delete this->nodes;
	}
	size_t Graph::generate_level()
	{
		static random_device rd;
		static mt19937 gen(rd());
		static uniform_real_distribution<float> dist(0.0f, 1.0f);

		return size_t(floorf(-logf(dist(gen)) * this->ml));
	}
	void Graph::insert(Node* query)
	{
		Set* found = new Set(query, this->entry, this->dataset->use_pow, this->dataset->use_sqrt, set_order::furthest);
		query->init(this->generate_level());

		for(size_t i = this->entry->level; i >= query->level + 1; i--)
		{
			this->search_layer(query, found, 1, i);
			found->keep_only_the_nearest();
		}

		for(size_t i = min(this->entry->level, query->level);; i--)
		{
			this->search_layer(query, found, this->ef, i);
			Set* neighbors = this->select_neighbors(query, found, this->m, i);
			query->connect(neighbors->nodes, i);

			for(auto& item : *neighbors)
			{
				auto e_conn = new Set(query, item->neighborhood(i), this->dataset->use_pow, this->dataset->use_sqrt);

				if(e_conn->size() > this->mmax)
				{
					Set* e_new_conn = this->select_neighbors(query, e_conn, this->mmax, i);
					item->set_neighborhood(e_new_conn->transfer_nodes(), i);
					delete e_new_conn;
				}

				delete e_conn;
			}

			if(i == 0)
				break;
		}

		if(query->level > this->entry->level)
			this->entry = query;

		delete found;
	}
	void Graph::search_layer(Node* query, Set* out_entries, size_t ef, size_t layer_idx)
	{
		unordered_set<Node*> visited;
		Set* candidates = new Set(*out_entries, set_order::nearest);
		out_entries->ensure_order(set_order::furthest);

		visited.reserve(out_entries->size());

		for(auto* item : *out_entries)
			visited.insert(item);

		while(candidates->size() > 0)
		{
			auto nearest_candidate = candidates->pop_front();
			auto furthest_found = out_entries->front();

			if(nearest_candidate->distance > furthest_found->distance)
				break;

			for(auto& item : *nearest_candidate->neighborhood(layer_idx))
				if(visited.insert(item).second)
				{
					item->compute_distance_to(query, this->dataset->use_pow, this->dataset->use_sqrt);
					furthest_found = out_entries->front();

					if(item->distance < furthest_found->distance || out_entries->size() < this->ef)
					{
						candidates->insert(item);
						out_entries->insert(item);

						if(out_entries->size() > this->ef)
							out_entries->remove_front();
					}
				}
		}

		delete candidates;
	}
	Set* Graph::select_neighbors(Node* query, Set* candidates, size_t m, size_t layer_idx)
	{
		if(this->use_heuristic)
		{
			Set* result = new Set(query, this->dataset->use_pow, this->dataset->use_sqrt);
			Set* found;
			Set* discarded = new Set(query, this->dataset->use_pow, this->dataset->use_sqrt, set_order::nearest);
			
			if(this->extend_candidates)
			{
				unordered_set<Node*> visited;
				found = new Set(query, this->dataset->use_pow, this->dataset->use_sqrt, set_order::nearest);
				
				for(auto& element : *candidates)
					for(auto& adj : *element->neighborhood(layer_idx))
						visited.insert(adj);

				for(const auto& item : visited)
					found->insert(item);
			}
			else
				found = new Set(*candidates, set_order::nearest);

			while(found->size() > 0 && result->size() < this->m)
			{
				auto item = found->pop_front();

				if(result->size() == 0 || item->distance < result->front()->distance)
					result->insert(item);
				else
					discarded->insert(item);
			}

			if(this->keep_pruned)
			{
				while(discarded->size() > 0 && result->size() < this->m)
					result->insert(discarded->pop_front());
			}

			delete discarded;
			delete found;
			return result;
		}
		else
		{
			Set* result = new Set(*candidates);
			result->keep_only_k_nearest(m);
			return result;
		}
	}
	Set* Graph::approx_search(Node* query)
	{
		Set* found = new Set(query, this->entry, this->dataset->use_pow, this->dataset->use_sqrt, set_order::furthest);

		for(size_t i = this->entry->level; i >= 1; i--)
		{
			this->search_layer(query, found, 1, i);
			found->keep_only_the_nearest();
		}

		this->search_layer(query, found, this->ef, 0);
		found->keep_only_k_nearest(this->dataset->k);

		return found;
	}
	void Graph::build()
	{
		perf_start();
		{
			if constexpr(SHOW_PROGRESS)
			{
				ProgressBar progress("Inserting nodes...", float(this->dataset->node_count));
				this->entry = this->nodes->operator[](0);
				this->entry->init(this->generate_level());
				progress.update();

				for(size_t i = 1; i < this->dataset->node_count; i++)
				{
					this->insert(this->nodes->operator[](i));
					progress.update();
				}
			}
			else
			{
				this->entry = this->nodes->operator[](0);
				this->entry->init(this->generate_level());

				for(size_t i = 1; i < this->dataset->node_count; i++)
					this->insert(this->nodes->operator[](i));
			}
		}
		perf_stop(this->measurement.build_ms);
	}
	void Graph::search_all()
	{
		vector<Set*> results;

		perf_start();
		{
			results.reserve(this->dataset->query_count);

			for(auto& query : this->dataset->all_queries)
				results.push_back(this->approx_search(query));
		}
		perf_stop(this->measurement.approx_ms);

		this->measurement.median_accuracy = this->dataset->compare_to_bruteforce(&results);

		for(auto& item : results)
			delete item;
	}
	string Graph::to_string()
	{
		string result = "";

		for(auto& item : *this->nodes)
			result += item->to_string();

		return result;
	}
	void Graph::print()
	{
		printf("%s\n", this->to_string().c_str());
	}
	string Graph::create_description()
	{
		char* buffer = new char[GRAPH_DESCRIPTION_MAX_LENGTH];

		snprintf
		(
			buffer, GRAPH_DESCRIPTION_MAX_LENGTH,
			"===================\n"
			"Graph parameters\n"
			"===================\n"
			"\n"
			"ef\t\t\t\t%zd\n"
			"m\t\t\t\t%zd\n"
			"mmax\t\t\t\t%zd\n"
			"ml\t\t\t\t%.3f\n"
			"\n"
			"Heuristic used\t\t\t%s\n"
			"Candidates extended\t\t%s\n"
			"Pruned kept\t\t\t%s\n",
			this->ef,
			this->m,
			this->mmax,
			this->ml,
			bool_to_string(this->use_heuristic).c_str(),
			bool_to_string(this->extend_candidates).c_str(),
			bool_to_string(this->keep_pruned).c_str()
		);

		string description(buffer);
		delete[] buffer;

		return description;
	}
	string Graph::create_measurement_description()
	{
		char* buffer = new char[MEASUREMENT_DESCRIPTION_MAX_LENGTH];

		snprintf
		(
			buffer, MEASUREMENT_DESCRIPTION_MAX_LENGTH,
			"\n"
			"Build time\t\t\t" TIME_FORMAT
			"Bruteforce search time\t\t" TIME_FORMAT
			"Approximate search time\t\t" TIME_FORMAT
			"Median accuracy\t\t\t%.3f %%\n",
			TIME_VALUE(this->measurement.build_ms),
			TIME_VALUE(this->dataset->measurement.bruteforce_ms),
			TIME_VALUE(this->measurement.approx_ms),
			this->measurement.median_accuracy
		);

		string description(buffer);
		delete[] buffer;

		return this->dataset->create_description() + '\n' + this->create_description() + description;
	}
	void Graph::print_measurement_description()
	{
		printf("%s\n", this->create_measurement_description().c_str());
	}
	void Graph::save_measurement_description()
	{
		char* buffer = new char[FILENAME_MAX_LENGTH];

		snprintf
		(
			buffer, FILENAME_MAX_LENGTH,
			"_%d_%d_graph_%zd_%zd_%zd_%zd_%.3f_%d_%d_%d.txt",
			this->dataset->use_pow,
			this->dataset->use_sqrt,
			this->dataset->k,
			this->ef,
			this->m,
			this->mmax,
			this->ml,
			this->use_heuristic,
			this->extend_candidates,
			this->keep_pruned
		);

		save_file(path_measurement(this->dataset->create_filename(add_config_to_filename("measurement").c_str(), nullptr, "") + string(buffer)), this->create_measurement_description());
		delete[] buffer;
	}
}
