#include <chrono>
#include "lib/util/arg_parse.hpp"
#include "lib/util/console.hpp"
#include "lib/graph.hpp"
using namespace mch;
using namespace std::chrono;

constexpr int ARGS_LEN = 13;

int main(int args_len, char** args)
{
	/*
		Arguments:

		graph_builder			string		path to the program
		wait_before_closing		bool		wait for keypress before closing	default = true
		progress_symbol			char		progress bar symbol
		dimensions				size_t		dimensions of elements
		m						size_t		number of established connections
		mmax					size_t		maximum number of connections per element
		ef						size_t		size of the dynamic candidate list
		ml						float		normalization factor for level generation
		use_heuristic			bool		should program use heuristic when selecting neighbors
		ext_cand				bool		valid only with heuristic: extend candidate list when selecting neighbors
		keep_pruned				bool		valid only with heuristic: add some of the discarded elements as neighbors
		output_dir_path			string		path to directory where the graph will be built
		coords_path				string		path to file with coordinates
	*/

	if (!init(args_len, ARGS_LEN, args))
		return -1;

	size_t dimensions, m, mmax, ef;
	float ml;
	bool use_heuristic, ext_cand, keep_pruned;
	FILE *coords_file, *graph_file;

	// parse arguments
	if (!parse_args<8>
	(
		{
			arg_type::SIZE_T, arg_type::SIZE_T, arg_type::SIZE_T, arg_type::SIZE_T,
			arg_type::FLOAT, arg_type::BOOL, arg_type::BOOL, arg_type::BOOL
		},
		{
			"Dimensions", "M", "MMAX", "EF", "ML",
			"Use heuristic", "Extend candidates", "Keep pruned connections"
		},
		args + 3,
		&dimensions, &m, &mmax, &ef, &ml, &use_heuristic, &ext_cand, &keep_pruned
	)) return aborted("Parsing of arguments failed.");

	// open files
	if (!open_file(coords_file, args[args_len - 1], "rb"))
		return aborted("Coordinates file could not be opened.");
	if (!create_graph_file(args[args_len - 2], graph_file))
		return abort_code(fclose(coords_file) - 1, "New graph file could not be created.");

	// graph creation starts
	auto start = system_clock::now();
	
	Graph* graph = new Graph(
		use_heuristic, ext_cand, keep_pruned, dimensions,
		m, mmax, ef, ml, coords_file
	);
	graph->write_to(graph_file);

	auto end = system_clock::now();
	delete graph;

	long long diff_s = duration_cast<seconds>(end - start).count();
	printf("Graph built in %02lld:%02lld.\n", diff_s / 60, diff_s % 60);

	// clean and exit
	fclose(coords_file);
	fclose(graph_file);
	return completed();
}
