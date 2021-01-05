#include "distance.h"

namespace mch
{
	float distance_between(float* a, float* b, size_t dimensions)
	{
		float sum = 0;

		for(size_t i = 0; i < dimensions; i++)
		{
			float difference = a[i] - b[i];
			sum += difference * difference;
		}

		return sum;
	}
}
