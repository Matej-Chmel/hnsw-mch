#pragma once

namespace mch
{
	class BruteforceItem
	{
	public:
		float* coords;
		float distance;

		BruteforceItem(float* coords);

		void compute_distance_to(float* query, size_t dimensions);
	};
}
