#pragma once
#include "imports.h"

namespace mch
{
	constexpr size_t FILENAME_MAX_LENGTH = 256;
	const string PATH_REPO_ROOT = "..\\..\\";
	const string PATH_DATASETS = PATH_REPO_ROOT + "datasets\\";
	const string PATH_MEASUREMENTS = PATH_REPO_ROOT + "measurements\\";

	string path_dataset(string filename);
	string path_measurement(string filename);
	float* load_file(string filename, size_t& out_length);
	void save_file(string filename, float* data, size_t length);
	void save_file(string filename, string content);
}
