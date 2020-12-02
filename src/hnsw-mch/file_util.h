#pragma once
#include "imports.h"

namespace mch
{
	constexpr size_t FILENAME_MAX_LENGTH = 256;
	const string PATH_REPO_ROOT = "..\\..\\";
	const string PATH_DATASETS = PATH_REPO_ROOT + "datasets\\";
	const string PATH_MEASUREMENTS = PATH_REPO_ROOT + "measurements\\";
	constexpr size_t READ_SIZE = 65536;

	string path_dataset(string filename);
	string path_measurement(string filename);
	float* load_file(string filename, size_t& out_length);
	void save_file(string filename, float* data, size_t length);
	void save_file(string filename, string content);

	template <typename T>
	T* load_file_data(string filename, size_t& out_length);

	template <typename T>
	T* load_file_data(string filename, size_t& out_length)
	{
		FILE* file = fopen(filename.c_str(), "rb");

		if(!file)
			return nullptr;

		size_t capacity = READ_SIZE;
		vector<T> buffer;

		for(int i = 1;; i++)
		{
			buffer.resize(i * READ_SIZE);

			size_t read = fread(buffer.data(), sizeof(T), READ_SIZE, file);

			if(read != READ_SIZE)
			{
				if(read != 0)
					buffer.resize(i * READ_SIZE - (READ_SIZE - read));

				break;
			}
		}

		out_length = buffer.size();
		T* data = new T[out_length];
		memcpy(data, buffer.data(), out_length);

		return data;
	}
}
