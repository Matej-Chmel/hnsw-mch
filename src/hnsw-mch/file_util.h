#pragma once
#include "imports.h"

namespace mch
{
	string res(string filename);
	float* load_file(string filename, size_t& out_length);
	void save_file(string filename, float* data, size_t length);
	void save_file(string filename, string content);
}
