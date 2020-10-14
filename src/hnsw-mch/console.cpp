#include "console.h"

namespace mch
{
	bool wait_before_closing = true;

	void _format_exit(const char* printf_format, va_list& args)
	{
		vprintf(printf_format, args);
		va_end(args);

		if (wait_before_closing)
		{
			fputs("\n\nPress any key to exit the program.\n", stdout);
			getchar();
		}
		else fputc('\n', stdout);
	}
	
	int abort_code(int code, const char* printf_format, ...) 
	{
		va_list args;
		va_start(args, printf_format);
		_format_exit(printf_format, args);
		return code;
	}

	int aborted(const char* printf_format, ...)
	{
		va_list args;
		va_start(args, printf_format);
		_format_exit(printf_format, args);
		return -1;
	}
	
	int completed(const char* printf_format, ...)
	{
		va_list args;
		va_start(args, printf_format);
		_format_exit(printf_format, args);
		return 0;
	}

	#ifdef __linux__

	void load_caret()
	{
		fputs("\033[u", stdout);
	}

	void save_caret()
	{
		fputs("\033[s", stdout);
	}

	#elif _WIN32

	void load_caret()
	{
		SetConsoleCursorPosition(private_console::handle, private_console::caret_pos);
	}

	void save_caret()
	{
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (GetConsoleScreenBufferInfo(private_console::handle, &info))
			private_console::caret_pos = info.dwCursorPosition;
		else private_console::caret_pos = { 0, 0 };
	}

	#else

	void load_caret()
	{
		fputs("\n\n\n\n\n\n\n\n\n\n\n\n", stdout);
	}

	void save_caret() {}

	#endif
}
