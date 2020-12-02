#pragma once
#include "imports.h"

namespace mch
{
	string bool_to_string(bool value);
	string add_config_to_filename(const char* filename);
	void pause(const char* message);
	void crash(const char* message);
	void crashf(const char* format, ...);
}
