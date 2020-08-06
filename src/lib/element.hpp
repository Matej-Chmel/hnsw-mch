#ifndef MCH_ELEMENT
#define MCH_ELEMENT

#include <cstdio>
#include <memory>
#include <vector>

namespace mch
{
	using namespace std;
	
	class Element;

	typedef shared_ptr<Element> elem;
	typedef vector<elem> elem_vec;
	typedef shared_ptr<elem_vec> elem_vec_shr;
	typedef vector<elem_vec_shr> elem_vec2;

	typedef unique_ptr<vector<float>> floats_unq;
	typedef vector<floats_unq> floats2;

	class Element : public enable_shared_from_this<Element>
	{
	private:
		static const size_t END;

		float distance;
		float* coords;
		size_t dimensions;
		Element* query;
		elem_vec2 layers;

		void init_layers(size_t idx);
	
	public:
		size_t id;
		size_t level;

		struct distance_comparator
		{
			bool operator()(elem const& a, elem const& b) const;
		};
		struct id_comparator
		{
			bool operator()(elem const& a, elem const& b) const;
		};

		Element(size_t id, float* coords, size_t dimensions);

		void set_level(size_t level);

		void compute_distance_to(Element* query);
		elem_vec_shr& neighborhood(size_t layer_idx);
		void connect(elem_vec_shr elements, size_t layer_idx);

		void write_to(vector<float>& coords_block, vector<size_t>& ids_block);
		void write_to(float*& coords_block);
		void load_from(FILE* graph_file, elem_vec& all_elements);

		bool operator<(elem const& other);
	};

	inline bool Element::distance_comparator::operator()(elem const& a, elem const& b) const
	{
		if (a->distance == b->distance)
			return a.get() < b.get();
		return a->distance < b->distance;
	}
	
	inline bool Element::id_comparator::operator()(elem const& a, elem const& b) const
	{
		return a->id < b->id;
	}
}

#endif
