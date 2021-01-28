#pragma once
#include "progress_updater.h"

namespace mch
{
	class Dataset
	{
	public:
		float* coords;
		size_t count;
		size_t dimensions;

		Dataset(size_t dimensions, const char* path, size_t* count);
		Dataset(size_t dimensions, size_t count, float min_value, float max_value, size_t seed, ProgressUpdater* updater);
		~Dataset();

		float* get_coord(size_t idx);
	};
}
