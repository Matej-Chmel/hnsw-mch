#pragma once

namespace mch
{
	struct GenMeasurement
	{
		long long node_ms;
        long long query_ms;
		long long bruteforce_ms;
	};

    struct BuildMeasurement
	{
		long long build_ms;
		long long approx_ms;
		float median_accuracy;
	};
}
