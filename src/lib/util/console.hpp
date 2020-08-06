#ifndef MCH_CONSOLE
#define MCH_CONSOLE

#include <cstdarg>

namespace mch
{
	using namespace std;

	extern bool wait_before_closing;

	void _format_exit(const char* printf_format, va_list& args);
	int abort_code(int code, const char* printf_format, ...);
	int aborted(const char* printf_format, ...);
	int completed(const char* printf_format = "Completed.", ...);
	void load_caret();
	void save_caret();
}

#ifdef _WIN32

#include <windows.h>
#undef max
#undef min

namespace mch::private_console
{
	inline COORD caret_pos = { 0, 0 };
	inline const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

#endif

#endif
