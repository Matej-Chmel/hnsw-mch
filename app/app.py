from itertools import product
from local_release.hnsw_mch import BenchmarkRunner, Config
from matplotlib import pyplot as plt
from os.path import abspath, dirname, join, pardir, realpath
from pathlib import Path
from tqdm import tqdm

SIFT = False
SCRIPT_PATH = dirname(realpath(__file__))
BENCHMARKS_PATH = abspath(join(SCRIPT_PATH, pardir, 'benchmarks'))
DATASET_PATH = join(SCRIPT_PATH, 'datasets')
Path(BENCHMARKS_PATH).mkdir(exist_ok=True)

def get_benchmark_path(name, extension):
	return join(BENCHMARKS_PATH, f'{name}.{extension}')
def get_dataset_path(name):
	return join(DATASET_PATH, f'{name}.bin')

def benchmark(title):
	description = open(get_benchmark_path(title, 'txt'), mode='w+', encoding='utf-8')

	build_times, build_times_heuristic = [[], []], [[], []]
	config_tuples = list(product(ef_construction, m, ml, mmax_multiplier, use_heuristic))
	configs = []

	if type(nodes) is int:
		runner = BenchmarkRunner(dimensions, nodes, queries, min_value, max_value, k)
	else:
		bruteforce_path = get_dataset_path(bruteforce_filename)
		node_path = get_dataset_path(nodes)
		query_path = get_dataset_path(queries)

		print(bruteforce_path, node_path, query_path, sep='\n')

		runner = BenchmarkRunner(
			dimensions,
			node_path,
			query_path,
			bruteforce_path,
			k
		)

	runner.reserve(len(config_tuples))
	progress = tqdm(desc='Building configurations', total=len(config_tuples))

	for item in config_tuples:
		config = Config(item[0], False, False, item[1], item[2], item[1] * item[3], item[4])
		configs.append(config)
		runner.add(config, ef_search)
		progress.update()

	progress.close()

	for index, benchmark in enumerate(runner.benchmarks):
		description.write(f'Config {index}:\n{benchmark.config}\n\n')

		def append_build_time(container):
			container[0].append(index)
			container[1].append(benchmark.build_time)

		def get_speed(time):
			try:
				return 1000 / (time / queries)
			except ZeroDivisionError:
				return 0

		append_build_time(build_times_heuristic if benchmark.config.use_heuristic else build_times)
		speed = [get_speed(time) for time in benchmark.search_times]
		plt.plot(benchmark.recall, speed, label=index, marker='o')

		for i, xy in enumerate(zip(benchmark.recall, speed)):
			plt.annotate(ef_search[i], xy)

	plt.plot([1.0], [runner.bruteforce_time], label='bruteforce', marker='o')

	plt.xlabel('Recall')
	plt.ylabel('Queries per second [1/s]')
	plt.title('Approximate search')
	plt.legend()
	plt.savefig(get_benchmark_path(title, 'pdf'))

	plt.clf()
	plt.plot(build_times[0], build_times[1], label='simple', marker='o')
	plt.plot(build_times_heuristic[0], build_times_heuristic[1], label='heuristic', marker='o')
	plt.xlabel('Configuration')
	plt.ylabel('Build time [ms]')
	plt.title('Build')
	plt.legend()
	plt.savefig(get_benchmark_path(f'{title}_build', 'pdf'))

	description.close()

if SIFT:
	bruteforce_filename = 'knnQA1M'
	ef_construction = [100]
	ef_search = [100]
	figure_title = 'sift'
	nodes = 'sift1M'
	queries = 'siftQ1M'
	use_heuristic = [False]
else:
	ef_construction = [25, 50, 100, 200]
	ef_search = [25, 50, 100, 300]
	nodes = 30000
	queries = nodes // 10
	use_heuristic = [False, True]
	figure_title = str(nodes)

dimensions = 128
k = 100
m = [20]
max_value = 184.0
min_value = 0.0
ml = [0.33]
mmax_multiplier = [2]

benchmark(figure_title)
