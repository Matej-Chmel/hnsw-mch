#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace mch
{
	using namespace std;
	using namespace chrono;

	typedef long long ll;
	typedef system_clock::time_point tp;

	#ifdef ARCH32

	typedef mt19937 Generator;

	#else

	typedef mt19937_64 Generator;

	#endif
}
