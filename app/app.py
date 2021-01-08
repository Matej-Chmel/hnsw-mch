from itertools import product
from local_release.hnsw_mch import BenchmarkRunner, BoundProgressFunctions, Config, ProgressUpdater
from matplotlib import pyplot as plt
from os.path import abspath, dirname, join, pardir, realpath
from pathlib import Path
from tqdm import tqdm

REPO_PATH = abspath(join(dirname(realpath(__file__)), pardir))
BENCHMARKS_PATH = join(REPO_PATH, 'benchmarks')
DATASET_PATH = join(REPO_PATH, 'datasets')
Path(BENCHMARKS_PATH).mkdir(exist_ok=True)

def get_benchmark_path(name, extension):
	return join(BENCHMARKS_PATH, f'{name}.{extension}')
def get_dataset_path(name):
	return join(DATASET_PATH, f'{name}.bin')

class ProgressBar:
	bar = None

	@staticmethod
	def bind():
		return BoundProgressFunctions(
			ProgressBar.close,
			ProgressBar.start,
			ProgressBar.update
		)

	@staticmethod
	def close():
		if ProgressBar.bar is not None:
			ProgressBar.bar.close()
			ProgressBar.bar = None
	
	@staticmethod
	def get_updater():
		return ProgressUpdater(ProgressBar.bind())

	@staticmethod
	def start(desc, total):
		ProgressBar.bar = tqdm(desc=desc, total=total)
	
	@staticmethod
	def update():
		ProgressBar.bar.update()

def benchmark(title):
	global queries
	
	descriptions = []

	build_times, build_times_heuristic = [[], []], [[], []]
	config_tuples = list(product(ef_construction, m, ml, mmax_multiplier, use_heuristic))
	configs = []
	updater = ProgressBar.get_updater()

	if type(nodes) is int:
		runner = BenchmarkRunner(dimensions, nodes, queries, min_value, max_value, k, updater)
	else:
		runner = BenchmarkRunner(
			dimensions, get_dataset_path(nodes), get_dataset_path(queries),
			get_dataset_path(bruteforce_filename), updater
		)
		queries = runner.get_query_count()

	runner.reserve(len(config_tuples))

	for item in config_tuples:
		config = Config(item[0], False, False, item[1], item[2], item[1] * item[3], item[4])
		configs.append(config)
		runner.add(config, ef_search)

	for index, benchmark in enumerate(runner.benchmarks):
		descriptions.append(f'Config {index}:\n{benchmark.config}')

		def append_build_time(container):
			container[0].append(index)
			container[1].append(benchmark.build_time / 60000)

		def get_speed(time):
			try:
				return 1000 / (time / queries)
			except ZeroDivisionError:
				return 0

		append_build_time(build_times_heuristic if benchmark.config.use_heuristic else build_times)
		speed = [get_speed(time) for time in benchmark.search_times]
		plt.plot(benchmark.recall, speed, label=index, marker='o')

		if index == 0:
			for i, xy in enumerate(zip(benchmark.recall, speed)):
				plt.annotate(ef_search[i], xy)

	if runner.bruteforce_time > 0:
		plt.plot([1.0], [get_speed(runner.bruteforce_time)], label='bruteforce', marker='o')

	plt.xlabel('Recall')
	plt.ylabel('Queries per second [1/s]')
	plt.title('Approximate search')
	plt.legend()
	plt.savefig(get_benchmark_path(title, 'pdf'))

	plt.clf()
	plt.plot(build_times[0], build_times[1], label='simple', marker='o')
	plt.plot(build_times_heuristic[0], build_times_heuristic[1], label='heuristic', marker='o')
	plt.xlabel('Configuration')
	plt.ylabel('Build time [min]')
	plt.title('Build')
	plt.legend()
	plt.savefig(get_benchmark_path(f'{title}_build', 'pdf'))

	with open(get_benchmark_path(title, 'txt'), mode='w+', encoding='utf-8') as file:
		file.write('\n\n'.join(descriptions))
		file.write('\n')

SIFT = False

if SIFT:
	bruteforce_filename = 'knnQA1M'
	ef_construction = [100]
	figure_title = 'sift'
	nodes = 'sift1M'
	queries = 'siftQ1M'
	use_heuristic = [False]
else:
	ef_construction = [100]
	k = 100
	max_value = 184.0
	min_value = 0.0
	nodes = 30000
	queries = nodes // 100
	use_heuristic = [False]
	figure_title = str(nodes)

dimensions = 128
ef_search = [25, 50, 100, 500, 2000, 10000]
m = [5, 20, 40]
ml = [0.0, 0.33, 0.66]
mmax_multiplier = [2]

benchmark(figure_title)
