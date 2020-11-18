#pragma once
#include "node.h"

namespace mch
{
	enum class set_order
	{
		none,
		nearest,
		furthest
	};

	struct nearest_comparator
	{
		bool operator()(const Node* const a, const Node* const b) const;
	};
	struct furthest_comparator
	{
		bool operator()(const Node* const a, const Node* const b) const;
	};
	struct sort_comparator
	{
		bool operator()(const Node* const a, const Node* const b) const;
	};

	extern nearest_comparator nearest_cmp;
	extern furthest_comparator furthest_cmp;
	extern sort_comparator sort_cmp;

	class Set
	{
	public:
		typedef vector<Node*>::iterator iterator;

		Node* query;
		vector<Node*>* nodes;
		bool use_pow;
		bool use_sqrt;
		bool owns_nodes;
		set_order order;

		Set(Node* query, bool use_pow, bool use_sqrt, set_order order = set_order::none);
		Set(Node* query, Node* node, bool use_pow, bool use_sqrt, set_order order = set_order::none);
		Set(const Set& other, set_order order = set_order::none);
		Set(Node* query, vector<Node*>* nodes, bool use_pow, bool use_sqrt);
		~Set();

		void heapify();
		void ensure_order(set_order order);
		vector<Node*>* transfer_nodes();
		void insert(Node* item);
		void push(Node* item);
		Node* front();
		Node* pop_front();
		void remove_front();
		size_t size();
		void keep_only_the_nearest();
		void keep_only_k_nearest(size_t k);

		Node* const& operator[](const size_t idx);
		iterator begin();
		iterator end();
	};
}
