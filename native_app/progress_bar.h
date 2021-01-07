#pragma once
#include "progress_updater.h"

namespace mch
{
	namespace progress
	{
		constexpr size_t WIDTH = 32;
		constexpr size_t BAR_WIDTH = WIDTH + 49;
		constexpr size_t C_STR_WIDTH = WIDTH + 1;
		constexpr double DECIMAL_WIDTH = double(WIDTH);
		constexpr size_t NUMS_WIDTH = WIDTH + 7;
		constexpr size_t RFRAME_WIDTH = 43;
		constexpr size_t SYMBOL = char(219);
	}

	#define PROGRESS_FORMAT "%02lld:%02lld"

	class ProgressBar
	{
	private:
		char* bar;
		double block_ticks;
		double current;
		size_t draw_pos;
		double next_update;
		char* nums;
		tp start_point;
		double total;
		double update_ticks;

		ll get_diff_s();
		void write_nums();

		ProgressBar();
		ProgressBar(ProgressBar const&) = delete;
		ProgressBar(ProgressBar&&) = delete;
		void operator=(ProgressBar const&) = delete;

	public:
		static ProgressBar& get_instance();

		~ProgressBar();

		BoundProgressFunctions bind();
		void close();
		void start(const char* title, size_t total);
		void update();
	};
}
