#include "file_util.h"
#include "util.h"

namespace mch
{
	void crashf(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		vprintf(format, args);
		va_end(args);
		pause_exit();
		exit(EXIT_FAILURE);
	}
	void pause_exit()
	{
		fputs("Press any key to exit.", stdout);
		(void)getchar();
	}
	string to_str(bool value)
	{
		return value ? "true" : "false";
	}
}
