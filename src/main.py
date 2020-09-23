from glob import glob
from os.path import abspath, dirname, join, pardir, realpath
from sys import argv
from src.lib import array, bruteforce, create_dataset, Element, eval, Graph, read_dataset, write_dataset

DATA_DIR = abspath(join(realpath(dirname(__file__)), pardir, 'data'))

def coords_(filename):
	return join(DATA_DIR, 'coords', filename)

def graphs_(filename):
	return join(DATA_DIR, 'graphs', filename)

def unique_filename(folder_func, extension, *identifiers):
	name = folder_func('_'.join(str(item) for item in identifiers))
	return f"{name}_{len(glob(f'{name}*')) + 1}.{extension}"

def new_dataset(dimensions, n_elements, is_query):
	name = f"{dimensions}_{n_elements}_{'q' if is_query else 'e'}_"
	name = f"{name}{len(glob(coords_(f'{name}*'))) + 1}"
	write_dataset(create_dataset(dimensions, n_elements, 0, 185), unique_filename(
		coords_, 'bin', dimensions, n_elements, 'q' if is_query else 'e'
	))

def new_bruteforce(dataset, queries, k_to_return):
	neighbors = [[index] + bruteforce(dataset, item, k_to_return) for index, item in enumerate(queries)]
	neighbors = [item for sublist in neighbors for item in sublist]
	with open(unique_filename(coords_, 'bin', len(dataset[0]), len(dataset)), mode='wb+') as file:
		array('L', neighbors).tofile(file)

def percentages_correct(bruteforce_results, search_results):
	out = []
	for b, s in zip(bruteforce_results, search_results):
		b_ids = [item[0] for item in b]
		correct = [(item.id in b_ids) for item in s]
		out.append(len(correct) / len(s) * 100)
	return out

def build_graph():
	graph = Graph.from_coords(metadata, coords_(metadata['coords_path']))
	graph.write_to(unique_filename(graphs_, 'py', graph.dimensions, len(graph.elements)))

def measure_graph():
	graph = Graph.load_from(graphs_(metadata['graph_path']))
	dataset = [item.coords for item in graph]
	queries = read_dataset(graph.dimensions, metadata['n_queries'], coords_(metadata['queries_path']))
	k_to_return = metadata['k_to_return']
	bruteforce_results = [bruteforce(dataset, item, k_to_return) for item in queries]
	search_results = [graph.search(Element(-1, -1, coords), k_to_return) for coords in queries]
	print('\n'.join(
		f'{p} %'
		for p in percentages_correct(bruteforce_results, search_results)
	)

def build_dataset():
	new_dataset(metadata['dimensions'], metadata['n_elements'], metadata['is_query'])

TASKS = {
	'build graph': build_graph,
	'build dataset': build_dataset,
	'measure graph': measure_graph
}

def main():
	TASKS[metadata['task']]()

if __name__ == "__main__":
	try:
		with open(argv[1]) as file:
			metadata = eval(file.read())
	except IndexError:
		print('No argument file was passed in.')
	except OSError:
		print('Could not read the argument file.')
	main()
