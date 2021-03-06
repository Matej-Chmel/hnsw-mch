#pragma once
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <functional>
#include <random>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace mch
{
	using namespace std;
	using namespace chrono;
	using namespace placeholders;

	typedef long long ll;
	typedef system_clock::time_point tp;

	#ifdef ARCH32

	typedef mt19937 Generator;

	#else

	typedef mt19937_64 Generator;

	#endif

	#define DEBUG_GRAPH
}
