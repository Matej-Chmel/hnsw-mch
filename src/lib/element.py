from __future__ import annotations
from src.lib import dataclass, List, sqrt

def compute_distance_between(dimensions, coords_1, coords_2):
	distance = 0.0
	for i in range(dimensions):
		distance += (coords_1[i] - coords_2[i]) ** 2
	return sqrt(distance)

@dataclass
class Element:
	id: int
	level: int
	coords: List[float]
	query: Element = None
	distance: float = None
	layers: List[List[Element]] = None

	@classmethod
	def partially_load_from(cls, model: tuple):
		return Element(model[0], model[1], model[2], layers=model[3])

	def __complete_load__(self, elements):
		self.layers = [[elements[_id] for _id in layer] for layer in self.layers]

	def __post_init__(self):
		if not self.layers:
			self.layers = [[] for _ in range(self.level + 1)]
	
	def compute_distance_to(self, query: Element) -> float:
		if self.query is query:
			return
		self.query = query
		self.distance = compute_distance_between(len(self.coords), self.coords, query.coords)
		return self.distance
	
	def neighborhood(self, layer_idx) -> List[Element]:
		return self.layers[layer_idx]
	
	def set_neighborhood(self, elements, layer_idx) -> List[Element]:
		self.layers[layer_idx] = elements
	
	def connect(self, elements: List[Element], layer_idx):
		layer = self.neighborhood(layer_idx)

		for item in elements:
			item.neighborhood(layer_idx).append(self)
			layer.append(item)

	def __lt__(self, other: Element):
		return self.distance < other.distance

	def __hash__(self):
		return id(self)

	def py_repr(self):
		value = (self.id, self.level, self.coords, [[item.id for item in layer] for layer in self.layers])
		return value
