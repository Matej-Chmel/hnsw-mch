#include "file_util.h"

namespace mch
{
	string path_dataset(string filename)
	{
		return PATH_DATASETS + filename;
	}
	string path_measurement(string filename)
	{
		return PATH_MEASUREMENTS + filename;
	}
	float* load_file(string filename, size_t& out_length)
	{
		FILE* file = fopen(filename.c_str(), "rb");

		if(!file)
			return nullptr;

		fread(&out_length, sizeof(size_t), 1, file);
		float* data = new float[out_length];
		fread(data, sizeof(float), out_length, file);

		fclose(file);
		return data;
	}
	void save_file(string filename, float* data, size_t length)
	{
		FILE* file = fopen(filename.c_str(), "wb+");

		fwrite(&length, sizeof(size_t), 1, file);
		fwrite(data, sizeof(float), length, file);

		fclose(file);
	}
	void save_file(string filename, string content)
	{
		FILE* file = fopen(filename.c_str(), "w+");
		fwrite(content.c_str(), sizeof(char), content.length(), file);
		fclose(file);
	}
}
