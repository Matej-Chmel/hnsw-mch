DEBUG = False

try:
	if DEBUG:
		from local_debug.hnsw_mch import Benchmark, Bruteforce, Config, Dataset
	else:
		from local_release.hnsw_mch import Benchmark, Bruteforce, Config, Dataset
except ImportError:
	from sys import exit
	exit('Module hnsw_mch is not available.')

def benchmark(title, *args):
	config = Config(*args)
	bruteforce = Bruteforce()
	bruteforce.search(nodes, queries, k)
	benchmark = Benchmark(config)
	benchmark.run(bruteforce, nodes, queries, ef, k)

	print(
		title,
		f'Build time: {benchmark.build_time} ms',
		f"Recall: {', '.join([f'{round(value, 2)} %' for value in benchmark.recall])}",
		f"Search times: {', '.join([f'{value} ms' for value in benchmark.search_times])}",
		end='\n\n',
		sep='\n'
	)

ef = [item for item in range(25, 526, 100)]
k = 300
nodes = Dataset(128, 30000, 0.0, 184.0)
queries = Dataset(128, 3000, 0.0, 184.0)

benchmark('Neighbors selected by simple sort', 100, False, False, 20, 0.33, 40, False)
benchmark('Neighbors selected by heuristic', 100, False, False, 20, 0.33, 40, True)
