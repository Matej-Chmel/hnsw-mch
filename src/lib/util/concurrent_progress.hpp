#ifndef MCH_CONCURRENT_PROGRESS
#define MCH_CONCURRENT_PROGRESS

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace mch
{
	using namespace std;
	using namespace chrono;

	constexpr size_t PROGRESS_WIDTH = 32;
	constexpr size_t PB_RFRAME_WIDTH = 43;
	constexpr auto REFRESH_DELAY = 256ms;

	extern atomic_size_t _bars_completed;
	extern char progress_symbol;

	typedef unique_ptr<mutex> mutex_unq;
	typedef unique_lock<mutex> ulock;
	typedef system_clock::time_point tp;

	class ConcurrentProgress;

	/*
		A variant of ProgressBar defined in progress.hpp.
		Used when multiple progress bars are running at the same time.
		One thread can update the progress and the other can write it to the console.
	*/
	class ExclusiveBar
	{
	private:
		friend ConcurrentProgress;

		mutex_unq _mutex;
		ulock _print_lock;
		ulock _update_lock;
		tp start;
		bool running;
		float current;
		float block_ticks;
		float update_ticks;
		float next_update;
		float total;
		char* bar;
		char* nums;
		size_t draw_pos;

		void print_to(char*& dest);
		void write_nums();

	public:
		ExclusiveBar();
		ExclusiveBar(ExclusiveBar&& obj) = default;
		~ExclusiveBar();

		void abort();
		void init(float total);
		void update(float ticks);
	};

	/*
		Encapsulating class for multiple progress bars running at the same time.
		Provides the run() method for the thread that writes the progress to the console.
	*/
	class ConcurrentProgress
	{
	private:
		size_t bars_len;
		tp start;
		char* out;

	public:
		vector<ExclusiveBar> bars;

		ConcurrentProgress(const char* title, size_t workers_len);
		~ConcurrentProgress();

		void run();
	};
}

#endif
