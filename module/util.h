#pragma once
#include "mch.h"

namespace mch
{
	void crashf(const char* format, ...);
	void pause_exit();
	string to_str(bool value);
}
