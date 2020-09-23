from src.lib import Any, array, bin_unpack, Callable, dataclass, Dict, Element, ElementSet, eval, List, log, random

@dataclass
class Graph:
	dimensions: int = None
	m: int = None
	mmax: int = None
	ef: int = None
	ml: float = None
	use_heuristic: bool = False
	ext_cand: bool = False
	keep_pruned: bool = False
	elements: List[Element] = None
	entry: Element = None
	__select_neighbors: Callable = None

	def __complete_load__(self):
		self.__select_neighbors = self.__select_neighbors_heuristic if self.use_heuristic else self.__select_neighbors_simple
		self.elements = []

	@classmethod
	def from_metadata(cls, metadata: Dict[str, Any]):
		graph = Graph()

		for key in metadata:
			if hasattr(graph, key):
				setattr(graph, key, metadata[key])
		
		graph.__complete_load__()
		return graph

	@classmethod
	def from_coords(cls, metadata: Dict[str, Any], path):
		graph = Graph.from_metadata(metadata)

		fmt = 'f' * graph.dimensions
		size = graph.dimensions * 4

		with open(path, 'rb') as file:
			coord_bytes = file.read(size)
			graph.start(bin_unpack(fmt, coord_bytes))
			coord_bytes = file.read(size)
			while coord_bytes:
				graph.insert(bin_unpack(fmt, coord_bytes))
				coord_bytes = file.read(size)

		return graph

	@classmethod
	def load_from(cls, path):
		with open(path, mode='r', encoding='utf-8') as file:
			metadata: Dict = eval(file.read())
		
		repr_elements = metadata.pop('elements')
		repr_entry = metadata.pop('entry')
		graph = Graph.from_metadata(metadata)
		graph.__complete_load__()

		graph.elements = []
		graph.entry = Element.partially_load_from(repr_entry)
		
		for model in repr_elements:
			graph.elements.append(Element.partially_load_from(model))
		
		graph.elements.sort(key=lambda item: item.id)
		graph.entry.__complete_load__(graph.elements)
		for item in graph.elements:
			item.__complete_load__(graph.elements)
		
		return graph

	def __generate_level(self):
		return int(-log(random()) * self.ml)

	def start(self, coords: List[float]):
		self.elements.append(Element(0, self.__generate_level(), coords))
		self.entry = self.elements[0]

	def insert(self, coords: List[float]):
		print(f'Inserting element: {len(self.elements)}')
		query = Element(len(self.elements), self.__generate_level(), coords)
		self.elements.append(query)
		lead_point = self.entry

		for layer_idx in range(self.entry.level, query.level, -1):
			lead_point = self.__search_layer(query, ElementSet(query, [lead_point]), 1, layer_idx).nearest()
		
		for layer_idx in range(min(self.entry.level, query.level), -1, -1):
			neighbors = self.__select_neighbors(
				self.__search_layer(query, ElementSet(query, [lead_point]), self.ef, layer_idx),
				self.m, layer_idx
			)

			query.connect(neighbors, layer_idx)

			for item in neighbors:
				e_conn = item.neighborhood(layer_idx)
				if len(e_conn) > self.mmax:
					item.set_neighborhood(self.__select_neighbors(ElementSet(item, e_conn), self.mmax, layer_idx), layer_idx)

		if query.level > self.entry.level:
			self.entry = query

	def __search_layer(self, query: Element, entry_points: ElementSet, ef_to_return: int, layer_idx: int) -> ElementSet:
		visited = entry_points.copy()
		candidates = entry_points.copy()

		while len(candidates):
			nearest_cand = candidates.pop_nearest()
			furthest_found = entry_points.furthest()

			if furthest_found < nearest_cand:
				break	# all elements in found are evaluated

			for item in nearest_cand.neighborhood(layer_idx):
				if not visited.insert(item):
					continue
				
				furthest_found = entry_points.furthest()

				if item < furthest_found or len(entry_points) < ef_to_return:
					candidates.insert(item)
					entry_points.insert(item)
					if len(entry_points) > ef_to_return:
						entry_points.pop_furthest()
		
		return entry_points

	def __select_neighbors_simple(self, candidates: ElementSet, m_to_return: int, _: int) -> List[Element]:
		return candidates[:m_to_return]

	def __select_neighbors_heuristic(self, candidates: ElementSet, m_to_return: int, layer_idx: int) -> List[Element]:
		result = ElementSet(candidates.query)
		discarded = ElementSet(candidates.query)
		cand_queue = candidates.copy()

		if self.ext_cand:
			for item in candidates:
				for adj in item.neighborhood(layer_idx):
					cand_queue.insert(adj)
		
		while len(cand_queue) and len(result) < m_to_return:
			if result.every_is_further_than(nearest := cand_queue.pop_nearest()):
				result.insert(nearest)
			else:
				discarded.insert(nearest)
		
		if self.keep_pruned:
			while len(discarded) and len(result) < m_to_return:
				result.insert(discarded.pop_nearest())
		
		return result[:]		

	def search(self, query: Element, k_to_return: int) -> List[Element]:
		lead_point = self.entry

		for layer_idx in range(self.entry.level, 0, -1):
			lead_point = self.__search_layer(query, ElementSet(query, [lead_point]), 1, layer_idx).nearest()
		
		return self.__search_layer(query, ElementSet(query, [lead_point]), self.ef, 0)[:k_to_return]

	def bruteforce(self, query: Element, k_to_return: int) -> List[int]:
		def bruteforce_key(element: Element):
			nonlocal query
			return element.compute_distance_to(query)
		self.elements.sort(key=bruteforce_key)
		return [item.id for item in self.elements[:k_to_return]]

	def write_to(self, path):
		metadata = {}
		for key in dir(self):
			if key.startswith('_'):
				continue
			attr = getattr(self, key)
			if callable(attr):
				continue
			if type(attr) is list and isinstance(attr[0], Element):
				value = [item.py_repr() for item in attr]
			elif isinstance(attr, Element):
				value = attr.py_repr()
			else:
				value = attr
			metadata[key] = value

		with open(path, mode='w+', encoding='utf-8') as file:
			file.write(repr(metadata))

	def __iter__(self):
		return self.elements.__iter__()
