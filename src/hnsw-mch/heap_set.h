#pragma once
#include "node.h"

namespace mch
{
	struct BruteforceItem
	{
		float* coords;
		float distance;

		BruteforceItem(float* coords, float distance);
	};

	struct HeapItem
	{
		Node* node;
		float distance;

		HeapItem(Node* node, float distance);
	};

	struct nearest
	{
		bool operator()(const HeapItem& a, const HeapItem& b) const;
	};
	struct sort_nearest
	{
		bool operator()(const HeapItem& a, const HeapItem& b) const;
	};
	struct furthest
	{
		bool operator()(const HeapItem& a, const HeapItem& b) const;
	};
	struct bruteforce_cmp
	{
		bool operator()(const BruteforceItem& a, const BruteforceItem& b) const;
	};

	template<typename comparator>
	class HeapSet
	{
		comparator cmp;

	public:
		typedef vector<HeapItem>::iterator iterator;

		vector<HeapItem> items;

		HeapSet();
		HeapSet(HeapItem entry);
		HeapSet(vector<Node*>& neighbors, float* query, size_t& dimensions);
		template<typename other_cmp> HeapSet(const HeapSet<other_cmp>& other);
		void init(const HeapSet<comparator>& other);

		size_t size();
		void insert(HeapItem& item);
		void insert(HeapItem&& item);
		void insert(Node* node, float* query, size_t& dimensions);
		void insert(Node* node, float distance);

		HeapItem& front();
		HeapItem pop_front();
		void remove_front();
		vector<float*> k_sorted_coords(size_t& k);
		vector<Node*> k_sorted_nodes(size_t& k);
		vector<Node*> to_vector();

		iterator begin();
		iterator end();
	};

	template<typename comparator>
	inline HeapSet<comparator>::HeapSet()
	{}

	template<typename comparator>
	inline
	HeapSet<comparator>::HeapSet(HeapItem entry)
	{
		this->items.push_back(move(entry));
	}
	template<typename comparator>
	inline
	HeapSet<comparator>::HeapSet(vector<Node*>& neighbors, float* query, size_t& dimensions)
	{
		this->items.reserve(neighbors.size());
		for(auto item : neighbors)
			this->items.emplace_back(item, item->compute_distance_to(query, dimensions));
		make_heap(this->items.begin(), this->items.end(), this->cmp);
	}
	template<typename comparator>
	inline void HeapSet<comparator>::init(const HeapSet<comparator>& other)
	{
		this->items.reserve(other.items.size());

		for(auto item : other.items)
			this->items.emplace_back(item.node, item.distance);
	}
	template<typename comparator>
	inline size_t HeapSet<comparator>::size()
	{
		return this->items.size();
	}
	template<typename comparator>
	inline
	void HeapSet<comparator>::insert(HeapItem& item)
	{
		this->insert(item.node, item.distance);
	}
	template<typename comparator>
	inline
		void HeapSet<comparator>::insert(HeapItem&& item)
	{
		this->items.push_back(move(item));
		push_heap(this->items.begin(), this->items.end(), this->cmp);
	}
	template<typename comparator>
	inline
	void HeapSet<comparator>::insert(Node* node, float* query, size_t& dimensions)
	{
		this->insert(node, node->compute_distance_to(query, dimensions));
	}
	template<typename comparator>
	inline void HeapSet<comparator>::insert(Node* node, float distance)
	{
		this->items.emplace_back(node, distance);
		push_heap(this->items.begin(), this->items.end(), this->cmp);
	}
	template<typename comparator>
	inline
	HeapItem& HeapSet<comparator>::front()
	{
		return this->items.front();
	}
	template<typename comparator>
	inline
	HeapItem HeapSet<comparator>::pop_front()
	{
		pop_heap(this->items.begin(), this->items.end(), this->cmp);
		auto output = this->items.back();
		this->items.pop_back();
		return output;
	}
	template<typename comparator>
	inline void HeapSet<comparator>::remove_front()
	{
		pop_heap(this->items.begin(), this->items.end(), this->cmp);
		this->items.pop_back();
	}
	template<typename comparator>
	inline
	vector<float*> HeapSet<comparator>::k_sorted_coords(size_t& k)
	{
		if constexpr(is_same<comparator, nearest>::value)
			sort_heap(this->items.begin(), this->items.end(), this->cmp);
		else sort(this->items.begin(), this->items.end(), sort_nearest());

		vector<float*> output;
		output.reserve(k);
		for(const auto item : this->items)
			output.emplace_back(item.node->coords);
		return move(output);
	}
	template<typename comparator>
	inline
	vector<Node*> HeapSet<comparator>::k_sorted_nodes(size_t& k)
	{
		sort_heap(this->items.begin(), this->items.end(), this->cmp);
		vector<Node*> output;
		output.reserve(k);
		for(const auto item : this->items)
			output.emplace_back(item.node);
		return move(output);
	}
	template<typename comparator>
	inline
	vector<Node*> HeapSet<comparator>::to_vector()
	{
		vector<Node*> output;
		output.reserve(this->items.size());

		for(auto item : this->items)
			output.emplace_back(item.node);

		return move(output);
	}
	template<typename comparator>
	inline typename
	HeapSet<comparator>::iterator HeapSet<comparator>::begin()
	{
		return this->items.begin();
	}
	template<typename comparator>
	inline typename
	HeapSet<comparator>::iterator HeapSet<comparator>::end()
	{
		return this->items.end();
	}
	template<typename comparator>
	template<typename other_cmp>
	inline
	HeapSet<comparator>::HeapSet(const HeapSet<other_cmp>& other)
	{
		this->items.reserve(other.items.size());

		for(auto item : other.items)
			this->items.emplace_back(item.node, item.distance);

		if constexpr(!is_same<comparator, other_cmp>::value)
			make_heap(this->items.begin(), this->items.end(), this->cmp);
	}
}
