#ifndef MCH_GRAPH
#define MCH_GRAPH

#include "util/random.hpp"
#include "search_results.hpp"
#include "set.hpp"
#include "storage_unit.hpp"

namespace mch
{
	class Graph
	{
	private:
		static const size_t COORDS_READ_LEN;
		static const size_t IDS_READ_LEN;

		bool use_heuristic;
		bool ext_cand;
		bool keep_pruned;

		size_t dimensions;
		size_t m;
		size_t mmax;
		size_t ef;
		float ml;
		RndDouble level_rand;
		unit elements;

		size_t next_id;
		elem entry;

		unit read_coords(FILE* coords_file, size_t& id);
		set_shr search_layer(Element* query, set_shr out_entry_points, size_t ef_to_return, size_t layer_idx);
		elem_vec_shr select_neighbors_simple(set_shr candidates, size_t m_to_return);
		elem_vec_shr select_neighbors_heuristic(set_shr candidates, size_t m_to_return, size_t layer_idx);
		elem_vec_shr search(Element* query, size_t k_to_return);
		void insert(elem query);
		elem_vec_shr bruteforce_search(Element* query, size_t k_to_return);

	public:
		Graph(
			bool use_heuristic, bool ext_cand, bool keep_pruned,
			size_t dimensions, size_t m, size_t mmax, size_t ef, float ml,
			FILE* coords_file
		);
		Graph(FILE* graph_file);

		void write_to(FILE* out_file);
		unit read_queries(FILE* coords_file);
		results search(unit& queries, size_t k_to_return);
		results bruteforce_search(unit& queries, size_t k_to_return);
	};
}

#endif
