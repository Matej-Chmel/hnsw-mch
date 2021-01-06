#pragma once
#include "nearest_set.h"

namespace mch
{
	class Dataset
	{
	public:
		float* coords;
		size_t count;
		size_t dimensions;

		Dataset(size_t dimensions, const char* path);
		Dataset(size_t dimensions, size_t count, float min_value, float max_value);
		~Dataset();

		float* get_coord(size_t idx);
	};
}
