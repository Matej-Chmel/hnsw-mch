from __future__ import annotations
from src.lib import Element, SortedSet, Union

def sort_key(element: Element):
	return element.distance, id(element)

class ElementSet:
	def __init__(self, query: Element, elements):
		self.query = query
		if type(elements) is list:
			for item in elements:
				item.compute_distance_to(self.query)
			self.elements = SortedSet(elements, sort_key)
		else:
			self.elements = SortedSet([item for item in elements], sort_key)

	def copy(self) -> ElementSet:
		return ElementSet(self.query, [item for item in self.elements])

	def insert(self, item: Element) -> bool:
		item.compute_distance_to(self.query)
		if item in self.elements:
			return False
		self.elements.add(item)
		return True

	def nearest(self) -> Element:
		return self.elements[0]
	
	def pop_nearest(self) -> Element:
		return self.elements.pop(0)
	
	def furthest(self) -> Element:
		return self.elements[-1]
	
	def pop_furthest(self) -> Element:
		return self.elements.pop()
	
	def every_is_further_than(self, item: Element):
		return True if not len(self) else item < self.nearest()

	def __getitem__(self, index) -> Union[Element, List[Element]]:
		return self.elements[index]

	def __iter__(self):
		return self.elements.__iter__()

	def __len__(self):
		return len(self.elements)
