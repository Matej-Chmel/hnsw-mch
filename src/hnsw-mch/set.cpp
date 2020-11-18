#include "set.h"

namespace mch
{
	nearest_comparator nearest_cmp;
	furthest_comparator furthest_cmp;
	sort_comparator sort_cmp;

	bool nearest_comparator::operator()(const Node* const a, const Node* const b) const
	{
		return a->distance > b->distance;
	}
	bool furthest_comparator::operator()(const Node* const a, const Node* const b) const
	{
		return a->distance < b->distance;
	}
	bool sort_comparator::operator()(const Node* const a, const Node* const b) const
	{
		return a->distance < b->distance;
	}
	Set::Set(Node* query, bool use_pow, bool use_sqrt, set_order order) : query(query), owns_nodes(true), order(order), use_pow(use_pow), use_sqrt(use_sqrt)
	{
		this->nodes = new vector<Node*>;
	}
	Set::Set(Node* query, Node* node, bool use_pow, bool use_sqrt, set_order order) : query(query), owns_nodes(true), order(order)
	{
		this->nodes = new vector<Node*>;
		node->compute_distance_to(query, this->use_pow, this->use_sqrt);
		this->nodes->push_back(node);
	}
	Set::Set(const Set& other, set_order order) : query(other.query), owns_nodes(true), use_pow(other.use_pow), use_sqrt(other.use_sqrt)
	{
		this->nodes = new vector<Node*>;
		*this->nodes = *other.nodes;

		if(order == set_order::none)
			this->order = other.order;
		else
		{
			this->order = order;

			if(order == other.order)
				return;

			this->heapify();
		}
	}
	Set::Set(Node* query, vector<Node*>* nodes, bool use_pow, bool use_sqrt):
		query(query), owns_nodes(false), nodes(nodes), use_pow(use_pow), use_sqrt(use_sqrt), order(set_order::none)
	{}
	Set::~Set()
	{
		if(this->owns_nodes)
			delete this->nodes;
	}
	void Set::heapify()
	{
		if(this->order == set_order::nearest)
			make_heap(this->nodes->begin(), this->nodes->end(), nearest_cmp);
		else
			make_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);
	}
	void Set::ensure_order(set_order order)
	{
		if(this->order == order)
			return;

		this->order = order;
		this->heapify();
	}
	vector<Node*>* Set::transfer_nodes()
	{
		this->owns_nodes = false;
		return this->nodes;
	}
	void Set::insert(Node* item)
	{
		item->compute_distance_to(this->query, this->use_pow, this->use_sqrt);
		this->nodes->push_back(item);

		if (this->order == set_order::nearest)
			push_heap(this->nodes->begin(), this->nodes->end(), nearest_cmp);
		else
			push_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);
	}
	void Set::push(Node* item)
	{
		this->nodes->push_back(item);
	}
	Node* Set::front()
	{
		return this->nodes->front();
	}
	Node* Set::pop_front()
	{
		if(this->order == set_order::nearest)
			pop_heap(this->nodes->begin(), this->nodes->end(), nearest_cmp);
		else
			pop_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);

		auto output = this->nodes->back();
		this->nodes->pop_back();
		return output;
	}
	void Set::remove_front()
	{
		if(this->order == set_order::nearest)
			pop_heap(this->nodes->begin(), this->nodes->end(), nearest_cmp);
		else
			pop_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);

		this->nodes->pop_back();
	}
	size_t Set::size()
	{
		return this->nodes->size();
	}
	void Set::keep_only_the_nearest()
	{
		make_heap(this->nodes->begin(), this->nodes->end(), nearest_cmp);
		this->nodes->erase(this->nodes->begin() + 1, this->nodes->end());
	}
	void Set::keep_only_k_nearest(size_t k)
	{
		if(this->order != set_order::nearest)
			make_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);

		sort_heap(this->nodes->begin(), this->nodes->end(), furthest_cmp);

		if(this->nodes->size() < k)
			k = this->nodes->size();

		this->nodes->erase(this->nodes->begin() + k, this->nodes->end());
	}
	Node* const& Set::operator[](const size_t idx)
	{
		return this->nodes->operator[](idx);
	}
	Set::iterator Set::begin()
	{
		return this->nodes->begin();
	}
	Set::iterator Set::end()
	{
		return this->nodes->end();
	}
}
