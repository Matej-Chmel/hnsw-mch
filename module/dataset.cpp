#include "dataset.h"
#include "file_util.h"
#include "util.h"

namespace mch
{
	Dataset::Dataset(size_t dimensions, const char* path):
		dimensions(dimensions)
	{
		this->coords = load_file_data<float>(path, this->count);
		this->count /= this->dimensions;

		if(!this->coords)
			crashf("File %s could not be opened.\n", path);
	}
	Dataset::Dataset(size_t dimensions, size_t count, float min_value, float max_value, size_t seed, ProgressUpdater* updater):
		count(count), dimensions(dimensions)
	{
		mt19937 gen(seed);
		uniform_real_distribution<float> dist(min_value, max_value);

		size_t length = this->count * this->dimensions;
		this->coords = new float[length];

		if (updater == nullptr)
			for(size_t i = 0; i < length; i++)
				this->coords[i] = dist(gen);
		else
		{
			updater->start("Generating coordinates", length);

			for(size_t i = 0; i < length; i++)
			{
				this->coords[i] = dist(gen);
				updater->update();
			}

			updater->close();
		}
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
