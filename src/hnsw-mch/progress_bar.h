#pragma once
#include "perf.h"

namespace mch
{
	constexpr size_t PROGRESS_WIDTH = 32;
	constexpr size_t PB_RFRAME_WIDTH = 43;
	extern char progress_symbol;

	class ProgressBar
	{
	private:
		tp start;
		float current;
		float block_ticks;
		float update_ticks;
		float next_update;
		float total;
		size_t draw_pos;
		char* bar;
		char* nums;

		void write_nums();

	public:
		ProgressBar(const char* title, float total);
		~ProgressBar();

		void update();
	};
}
