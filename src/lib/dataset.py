from src.lib import array, compute_distance_between, itemgetter, List, random_float, Tuple

def create_dataset(dimensions, n_elements, minimum, maximum) -> List[Tuple[float]]:
	return [(round(random_float(minimum, maximum), 0) for _ in range(dimensions)) for _ in range(n_elements)]

def read_dataset(dimensions, n_elements, path) -> List[Tuple[float]]:
	dataset = []
	with open(path, mode='rb') as file:
		for _ in range(n_elements):
			coords = array('f')
			coords.fromfile(file, dimensions)
			dataset.append(tuple(coords))
	return dataset

def write_dataset(dataset: List[Tuple[float]], path):
	with open(path, mode='wb+') as file:
		for item in dataset:
			array('f', item).tofile(file)

def bruteforce(dataset: List[Tuple[float]], query: List[float], k_to_return: int) -> List[int]:
	distances = [(index, compute_distance_between(len(item), item, query)) for index, item in enumerate(dataset)]
	distances.sort(key=lambda item: item[1])
	return distances[:k_to_return]
