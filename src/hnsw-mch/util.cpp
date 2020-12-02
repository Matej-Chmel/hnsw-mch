#include "file_util.h"
#include "util.h"

namespace mch
{
	constexpr size_t BOOL_STRING_MAX_LENGTH = 4;

	string bool_to_string(bool value)
	{
		return value ? "Yes" : "No";
	}
	string add_config_to_filename(const char* filename)
	{
		string progress;
		char* config = new char[FILENAME_MAX_LENGTH];

		if constexpr(SHOW_PROGRESS)
			progress = "progress";
		else
			progress = "no-progress";

		snprintf
		(
			config, FILENAME_MAX_LENGTH,
			"%s_%s_%s",
			filename
			,
			#ifdef NDEBUG
			"release"
			#else
			"debug"
			#endif
			,
			progress.c_str()
		);

		string result(config);
		delete[] config;

		return result;
	}
	void pause(const char* message)
	{
		printf("%s\nPress any key to continue.\n", message);
		(void) getchar();
	}
	void crash(const char* message)
	{
		pause(message);
		exit(EXIT_FAILURE);
	}
	void crashf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		fputs("Press any key to exit.", stdout);
		va_end(args);
		(void) getchar();
	}
}
