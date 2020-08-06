#include <cstring>
#include <ctime>
#include "arg_parse.hpp"
#include "concurrent_progress.hpp"
#include "console.hpp"

namespace mch
{
	bool init(int& args_len, const int& EXPECTED_LEN, char** args)
	{
		if (args_len >= 2)
			parse_arg<bool>(wait_before_closing, args[1], "Wait before closing", true);

		if (args_len >= 3)
			parse_arg<char>(progress_symbol, args[2], "Progress symbol", '@');

		if (args_len != EXPECTED_LEN)
			return abort_code(false, "Expected %d arguments. Received %d.\n", EXPECTED_LEN, args_len);
		return true;
	}

	bool create_graph_file(char* dir_path, FILE*& file)
	{
		time_t rawtime = time(NULL);
		size_t buffer_len = strlen(dir_path) + 30;
		char* buffer = new char[buffer_len];

		strftime
		(
			buffer + snprintf(buffer, buffer_len, "%s", dir_path),
			30, "graph_%d-%m-%Y_%H-%M-%S.bin", localtime(&rawtime)
		);

		if (open_file(file, buffer, "wb+"))
		{
			printf("New graph path: %s\n\n", buffer);
			delete[] buffer;
			return true;
		}

		delete[] buffer;
		return false;
	}

	string lowercase(char* data)
	{
		string result;
		char c;

		while ((c = *data++))
			result += tolower(c);

		return result;
	}

	bool open_file(FILE*& file, char* path, const char* mode)
	{
		file = fopen(path, mode);
		if (!file)
		{
			printf("File %s could not be opened.\n", path);
			perror("Reason: ");
			return false;
		}
		return true;
	}
}
