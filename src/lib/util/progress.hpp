#ifndef MCH_PROGRESS
#define MCH_PROGRESS

#include "concurrent_progress.hpp"

namespace mch
{
	constexpr size_t INDEF_RFRAME_WIDTH = 19;

	// Indefinite progress bar moving from left to right and then back to the beginning.
	class IndefiniteBar
	{
	private:
		tp start;
		int left;
		int right;
		char* bar;
		char* frame;
		char* nums;
		size_t processed;
		size_t update_ticks;
		size_t next_update;

		void print(bool completed);

	public:
		IndefiniteBar(const char* title, size_t update_ticks);
		~IndefiniteBar();

		void complete();
		void update(size_t processed);
	};

	// Simple progress bar that fills frame from left to right.
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

		void update(float ticks);
	};
}

#endif
