#include "dataset.h"
#include "file_util.h"
#include "util.h"

namespace mch
{
	Dataset::Dataset(const char* path, size_t dimensions):
		dimensions(dimensions)
	{
		this->coords = load_file_data<float>(path, this->count);
		this->count /= this->dimensions;

		if(!this->coords)
			crashf("File %s could not be opened.", path);
	}
	Dataset::Dataset(size_t dimensions, size_t count, float min_value, float max_value):
		count(count), dimensions(dimensions)
	{
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<float> dist(min_value, max_value);

		size_t length = this->count * this->dimensions;
		this->coords = new float[length];

		for(size_t i = 0; i < length; i++)
			this->coords[i] = dist(gen);
	}
	Dataset::~Dataset()
	{
		delete[] this->coords;
	}
	float* Dataset::get_coord(size_t idx)
	{
		return this->coords + idx * this->dimensions;
	}
}
