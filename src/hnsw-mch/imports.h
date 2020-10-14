#pragma once
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mch
{
	using namespace std;
	using namespace chrono;
	using namespace this_thread;

	extern system_clock::time_point __start__;
	extern system_clock::time_point __end__;

	#define PERF_CODE auto __start__ = system_clock::now(); auto __end__ = system_clock::now();

	#define PERF __start__ = system_clock::now();
	#define DONE(dest) __end__ = system_clock::now(); dest = duration_cast<milliseconds>(__end__ - __start__).count()

	#define TIME(ms) ms / 1000, ms % 1000
	#define TIME_FORMAT " %llu.%03llu\n"
}
