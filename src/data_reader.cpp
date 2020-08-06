#include <iostream>
#include <sstream>
#include "lib/util/arg_parse.hpp"
#include "lib/util/console.hpp"
#include "lib/util/progress.hpp"
using namespace mch;

constexpr size_t ARGS_LEN = 4;
constexpr size_t READ_LEN = 16000;

typedef vector<float> floats;
typedef shared_ptr<floats> floats_shr;
typedef vector<floats_shr> floats2;

int main(int args_len, char** args)
{
	/*
		Arguments:

		data_reader				string		path to program
		wait_before_closing		bool		wait for keypress before closing	default = true
		progress_symbol			char		progress bar symbol
		coords_path				string		path to file with coordinates
	*/

	if (!init(args_len, ARGS_LEN, args))
		return -1;

	FILE* coords_file;

	if (!open_file(coords_file, args[ARGS_LEN - 1], "rb"))
		return aborted("Coordinates file could not be opened.");

	floats2 blocks;
	auto start = system_clock::now();
	IndefiniteBar bar("\nReading coordinates...", READ_LEN);

	for(;;)
	{
		floats_shr block = make_shared<floats>();
		block->resize(READ_LEN);

		size_t read = fread(block->data(), sizeof(float), READ_LEN, coords_file);
		bar.update(read);

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
	fclose(coords_file);

	size_t coords_to_read;
	for(size_t main_idx = 0; main_idx < blocks.size(); main_idx++)
	{
		floats_shr block = blocks[main_idx];

		for(size_t block_idx = 0; block_idx < block->size();)
		{
			size_t end_idx;
			stringstream converter;

			cout << "Enter number of coordinates to read: ";
			cin >> coords_to_read;

			if (cin.fail())
			{
				cin.clear();
				cin.ignore(512, '\n');
				end_idx = block->size();
			}
			else if (coords_to_read != 0)
			{
				end_idx = block_idx + coords_to_read;
				if (end_idx > block->size())
					end_idx = block->size();
			}
			else
			{
				cin.clear();
				cin.ignore(512, '\n');
				return completed("Stopped reading.\nCompleted.");
			}
			
			for(; block_idx < end_idx; block_idx++)
				converter << (*block)[block_idx] << '\n';
			cout << converter.str();
		}
	}

	return completed();
}
