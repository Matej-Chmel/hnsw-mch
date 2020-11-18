#include "progress_bar.h"

namespace mch
{
	char progress_symbol = char(219);

	void ProgressBar::write_nums()
	{
		ll diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();

		snprintf(this->nums, PB_RFRAME_WIDTH, "%zd / %zd    %zd %%    %02lld:%02lld",
			size_t(this->current), size_t(this->total), size_t((this->current * 100) / this->total),
			diff_s / 60, diff_s % 60
		);
	}
	ProgressBar::ProgressBar(const char* title, float total):
		current(0), block_ticks(total / float(PROGRESS_WIDTH)), update_ticks(block_ticks / 2.0f),
		next_update(update_ticks), total(total), draw_pos(2)
	{
		size_t bar_len = PROGRESS_WIDTH + 49;
		char* frame = new char[PROGRESS_WIDTH + 1];
		this->bar = new char[bar_len];
		this->nums = this->bar + PROGRESS_WIDTH + 7;

		memset(frame, '_', PROGRESS_WIDTH);
		frame[PROGRESS_WIDTH] = '\0';
		snprintf(this->bar, bar_len, "\r[%s]    0 / 0    0 %%    00:00", frame);

		printf("%s\n", title);
		fputs(this->bar, stdout);

		delete[] frame;
		this->start = system_clock::now();
	}
	ProgressBar::~ProgressBar()
	{
		delete[] this->bar;
	}
	void ProgressBar::update()
	{
		if(this->current == this->total)
			return;

		this->current++;

		if(this->current < this->next_update)
			return;

		this->next_update += this->update_ticks;

		if(this->current > this->total)
			this->current = this->total;

		size_t draw_count = size_t((this->current / this->block_ticks) - float((this->draw_pos - 2)));

		if(draw_count > 0)
			for(size_t i = 0; i < draw_count; i++)
				this->bar[this->draw_pos++] = progress_symbol;

		this->write_nums();
		fputs(this->bar, stdout);

		if(this->current != this->total)
			return;

		ll diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();
		printf("\nCompleted in %02lld:%02lld.\n\n", diff_s / 60, diff_s % 60);
	}
}
