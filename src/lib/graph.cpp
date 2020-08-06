#include <cmath>
#include "util/progress.hpp"
#include "graph.hpp"

namespace mch
{
	const size_t Graph::COORDS_READ_LEN = 16384;
	const size_t Graph::IDS_READ_LEN = 4096;

	Graph::Graph(
		bool use_heuristic, bool ext_cand, bool keep_pruned,
		size_t dimensions, size_t m, size_t mmax, size_t ef, float ml,
		FILE* coords_file
	) : use_heuristic(use_heuristic), ext_cand(ext_cand), keep_pruned(keep_pruned),
		dimensions(dimensions), m(m), mmax(mmax), ef(ef), ml(ml),
		next_id(0)
	{
		this->elements = this->read_coords(coords_file, this->next_id);

		ProgressBar progress("Inserting elements...", this->elements->size());
		for(elem& item: *this->elements)
		{
			this->insert(item);
			progress.update(1);
		}
	}

	Graph::Graph(FILE* graph_file)
	{
		bool flags[3];
		size_t props[6];

		fread(flags, sizeof(bool), 3, graph_file);
		fread(props, sizeof(size_t), 6, graph_file);
		fread(&this->ml, sizeof(float), 1, graph_file);
		
		this->use_heuristic = flags[0];
		this->ext_cand = flags[1];
		this->keep_pruned = flags[2];

		this->dimensions = props[0];
		this->m = props[1];
		this->mmax = props[2];
		this->ef = props[3];
		this->next_id = props[4];

		size_t coords_len = this->next_id * this->dimensions;
		float* coords = new float[coords_len];

		fread(coords, sizeof(float), coords_len, graph_file);
		this->elements = make_unique<StorageUnit>(coords, this->next_id, this->dimensions);
		this->elements->load_from(graph_file);
		this->entry = (*this->elements)[props[5]];

		// coords are now owned by this->elements
	}

	unit Graph::read_coords(FILE* coords_file, size_t& id)
	{
		floats2 blocks;
		size_t coords_len = 0;

		IndefiniteBar progress("Reading coordinates...", COORDS_READ_LEN);
		for(size_t i = 0; ; i++)
		{
			blocks.push_back(make_unique<vector<float>>(COORDS_READ_LEN));
			size_t read = fread(blocks[i]->data(), sizeof(float), COORDS_READ_LEN, coords_file);
			coords_len += read;
			progress.update(read);

			if (read != COORDS_READ_LEN)
			{
				if (read == 0)
					blocks.pop_back();
				else blocks[i]->erase(blocks[i]->begin() + read, blocks[i]->end());

				break;
			}
		}
		progress.complete();

		return make_unique<StorageUnit>(blocks, coords_len, coords_len / this->dimensions, this->dimensions, id);
	}

	set_shr Graph::search_layer(Element* query, set_shr out_entry_points, size_t ef_to_return, size_t layer_idx)
	{
		Set visited(*out_entry_points);
		Set candidates(*out_entry_points);

		while(candidates.size() > 0)
		{
			elem nearest_cand = candidates.pop_nearest();
			elem furthest_found = out_entry_points->furthest();

			if (*furthest_found < nearest_cand)
				break;	// all elements in found are evaluated

			elem_vec_shr neighbors = nearest_cand->neighborhood(layer_idx);
			for(elem& item: *neighbors)
			{
				if (!visited.insert(item))
					continue;
				
				furthest_found = out_entry_points->furthest();
				
				if (*item < furthest_found || out_entry_points->size() < ef_to_return)
				{
					candidates.insert(item);
					out_entry_points->insert(item);
					if (out_entry_points->size() > ef_to_return)
						out_entry_points->remove_furthest();
				}
			}
		}

		return out_entry_points;
	}

	elem_vec_shr Graph::select_neighbors_simple(set_shr candidates, size_t m_to_return)
	{
		return candidates->slice(m_to_return);
	}

	elem_vec_shr Graph::select_neighbors_heuristic(set_shr candidates, size_t m_to_return, size_t layer_idx)
	{
		Set result(candidates->get_query());
		Set discarded(candidates->get_query());
		Set cand_queue(*candidates);

		if (this->ext_cand)
			for(elem const& item: *candidates)
			{
				elem_vec_shr neighbors = item->neighborhood(layer_idx);
				for(elem& adj: *neighbors)
					cand_queue.insert(adj);
			}
		
		while(cand_queue.size() > 0 && result.size() < m_to_return)
		{
			elem nearest = cand_queue.pop_nearest();
			if (result.every_is_further_than(nearest))
				result.insert(nearest);
			else discarded.insert(nearest);
		}

		if (this->keep_pruned)
			while(discarded.size() > 0 && result.size() < m_to_return)
				result.insert(discarded.pop_nearest());
		
		return result.slice(result.size());
	}

	elem_vec_shr Graph::search(Element* query, size_t k_to_return)
	{
		elem entry_point = this->entry;

		for(size_t layer_idx = entry_point->level; layer_idx > 0; layer_idx--)
			entry_point = this->search_layer(query, make_shared<Set>(query, entry_point), size_t(1), layer_idx)->nearest();
		
		return this->search_layer(query, make_shared<Set>(query, entry_point), this->ef, 0)->slice(k_to_return);
	}

	void Graph::insert(elem query)
	{
		if (!this->entry)
		{
			this->entry = query;
			this->entry->set_level(-log(this->level_rand()) * this->ml);
			return;
		}

		Element* query_raw = query.get();
		elem entry_point = this->entry;
		query->set_level(-log(this->level_rand()) * this->ml);
		
		for(size_t layer_idx = this->entry->level; layer_idx > query->level; layer_idx--)
			entry_point = this->search_layer(query_raw, make_shared<Set>(query_raw, entry_point), size_t(1), size_t(layer_idx))->nearest();
		
		set_shr entry_points = make_shared<Set>(query_raw, entry_point);
		for(size_t layer_idx = min(this->entry->level, query->level); ; layer_idx--)
		{
			elem_vec_shr neighbors;

			if (this->use_heuristic)
				neighbors = this->select_neighbors_heuristic(
					this->search_layer(query_raw, entry_points, this->ef, layer_idx),
					this->m, layer_idx
				);
			else neighbors = this->select_neighbors_simple(
				this->search_layer(query_raw, entry_points, this->ef, layer_idx),
				this->m
			);

			query->connect(neighbors, layer_idx);

			for(elem& item: *neighbors)
			{
				elem_vec_shr e_conn = item->neighborhood(layer_idx);

				if (e_conn->size() > this->mmax)
				{
					if (this->use_heuristic)
						item->neighborhood(layer_idx) = this->select_neighbors_heuristic(
							make_shared<Set>(item.get(), e_conn), this->mmax, layer_idx
						);
					else item->neighborhood(layer_idx) = this->select_neighbors_simple(
						make_shared<Set>(item.get(), e_conn), this->mmax
					);
				}
			}

			// ep = W because out_entry_points are passed by reference

			if (layer_idx == 0)
				break;
		}

		if (query->level > this->entry->level)
			this->entry = query;
	}

	elem_vec_shr Graph::bruteforce_search(Element* query, size_t k_to_return)
	{
		k_to_return = this->elements->size() < k_to_return ? this->elements->size() : k_to_return;
		auto end = this->elements->begin() + k_to_return;

		this->elements->sort_by_distance_to(query);
		return make_shared<elem_vec>(this->elements->begin(), end);
	}

	void Graph::write_to(FILE* out_file)
	{
		bool flags[3] = { this->use_heuristic, this->ext_cand, this->keep_pruned };
		size_t props[6] = { this->dimensions, this->m, this->mmax, this->ef, 0 , 0};
		size_t id = 0;
		vector<float> coords;
		vector<size_t> ids;

		coords.reserve(this->elements->size() * this->dimensions);
		ids.reserve(this->elements->size() * this->mmax);

		this->elements->sort_by_id();

		props[4] = id;	// number of elements
		props[5] = this->entry->id;

		fwrite(flags, sizeof(bool), 3, out_file);
		fwrite(props, sizeof(size_t), 6, out_file);
		fwrite(&this->ml, sizeof(float), 1, out_file);

		for(elem& item: *this->elements)
			item->write_to(coords, ids);

		fwrite(coords.data(), sizeof(float), coords.size(), out_file);
		fwrite(ids.data(), sizeof(size_t), ids.size(), out_file);
	}

	unit Graph::read_queries(FILE* coords_file)
	{
		size_t id = 0;
		return this->read_coords(coords_file, id);
	}

	results Graph::search(unit& queries, size_t k_to_return)
	{
		results all_results = make_unique<SearchResults>(queries->size(), this->dimensions);

		for(elem& query: *queries)
			all_results->push(this->search(query.get(), k_to_return));

		return all_results;
	}

	results Graph::bruteforce_search(unit& queries, size_t k_to_return)
	{
		results all_results = make_unique<SearchResults>(queries->size(), this->dimensions);

		for(elem& query: *queries)
			all_results->push(this->bruteforce_search(query.get(), k_to_return));
		
		return all_results;
	}
}
