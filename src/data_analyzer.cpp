#include <cmath>
#include <cstdio>
#include <limits>
#include <numeric>
#include <thread>
#include "lib/util/arg_parse.hpp"
#include "lib/util/console.hpp"
#include "lib/util/progress.hpp"
using namespace mch;

constexpr size_t ARGS_LEN = 5;
constexpr float FLOAT_MIN = numeric_limits<float>::lowest();
constexpr float FLOAT_MAX = numeric_limits<float>::max();
constexpr size_t READ_LEN = 16000;

typedef vector<float> floats;
typedef shared_ptr<floats> floats_shr;
typedef vector<floats_shr> floats2;

struct thread_output
{
	float min;
	float max;
	size_t integers_len;

	thread_output() : 
		min(FLOAT_MAX),
		max(FLOAT_MIN),
		integers_len(0)
	{}
};

void analyze(floats2 blocks, thread_output* output, ExclusiveBar* progress)
{
	progress->init(accumulate(blocks.begin(), blocks.end(), 0, 
		[](const size_t& subtotal, const floats_shr& block)
		{
			return subtotal + block->size();
		})
	);

	for(floats_shr& block: blocks)
	{
		for(float& item: *block)
		{
			if (item < output->min)
				output->min = item;
			if (item > output->max)
				output->max = item;
			if (roundf(item) == item)
				output->integers_len++;
		}

		progress->update(block->size());
	}

	progress->abort();
}

int main(int args_len, char** args)
{
	/*
		Arguments:

		data_analyzer			string		path to program
		wait_before_closing		bool		wait for keypress before closing	default = true
		progress_symbol			char		progress bar symbol
		workers_len				size_t		number of workers
		coords_path				string		path to file with coordinates
	*/

	if (!init(args_len, ARGS_LEN, args))
		return -1;
	
	FILE* coords_file;
	size_t workers_len;

	if (!parse_arg(workers_len, args[ARGS_LEN - 2], "Number of workers"))
		return -1;

	if (workers_len == 0)
		return aborted("Zero workers won't cut it.");

	if (!open_file(coords_file, args[ARGS_LEN - 1], "rb"))
		return aborted("Coordinates file could not be opened.");

	float coord_max = FLOAT_MIN;
	float coord_min = FLOAT_MAX;
	floats2 blocks;
	size_t coords_len = 0;
	size_t integers_len = 0;

	auto start = system_clock::now();
	IndefiniteBar bar("\nReading coordinates...", READ_LEN);

	for(;;)
	{
		floats_shr block = make_shared<floats>();
		block->resize(READ_LEN);

		size_t read = fread(block->data(), sizeof(float), READ_LEN, coords_file);
		bar.update(read);
		coords_len += read;

		if (read != READ_LEN)
		{
			if (read == 0)
				break;
			block->resize(read);
			blocks.push_back(block);
			break;
		}

		blocks.push_back(block);
	}
	bar.complete();

	vector<thread> workers;
	vector<thread_output> results;
	size_t blocks_per_thread = blocks.size() / workers_len;
	workers.reserve(workers_len);
	results.resize(workers_len);

	ConcurrentProgress progress("Analyzing coordinates...", workers_len);

	floats2::iterator iter = blocks.begin();
	for(size_t i = 0; i < workers_len - 1; i++)
	{
		workers.push_back(thread(
			analyze,
			floats2(iter, iter + blocks_per_thread),
			&results[i],
			&progress.bars[i]
		));
		iter += blocks_per_thread;
	}
	workers.push_back(thread(
		analyze,
		floats2(iter, blocks.end()),
		&results[workers_len - 1],
		&progress.bars[workers_len - 1]
	));

	progress.run();

	for(thread& worker: workers)
		worker.join();

	for(thread_output& result: results)
	{
		if (result.min < coord_min)
			coord_min = result.min;
		if (result.max > coord_max)
			coord_max = result.max;
		integers_len += result.integers_len;
	}

	auto end = system_clock::now();

	printf
	(
		"Results:\n\n"
		"Coordinates read: %zd\n"
		"From that %zd were integers.\n"
		"Minimum: %.45f\n"
		"Maximum: %.45f\n\n",
		coords_len,
		integers_len,
		coord_min,
		coord_max
	);

	long long diff_s = duration_cast<seconds>(end - start).count();
	printf("Total duration: %02lld:%02lld.\n", diff_s / 60, diff_s % 60);

	// clean and exit
	fclose(coords_file);
	return completed();
}
