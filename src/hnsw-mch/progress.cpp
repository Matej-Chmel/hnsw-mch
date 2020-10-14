#include "progress.h"

namespace mch
{
	IndefiniteBar::IndefiniteBar(const char* title, size_t update_ticks) :
		update_ticks(update_ticks), next_update(update_ticks), left(0), processed(0)
	{
		size_t bar_len = PROGRESS_WIDTH + 26;
		size_t train_len = PROGRESS_WIDTH / 2;
		char* frame = new char[PROGRESS_WIDTH + 1];

		this->bar = new char[bar_len];
		this->frame = this->bar + 2;
		this->nums = this->bar + PROGRESS_WIDTH + 7;
		this->right = train_len - 1;

		memset(frame, progress_symbol, train_len);
		memset(frame + train_len, '_', PROGRESS_WIDTH - train_len);
		frame[PROGRESS_WIDTH] = '\0';
		snprintf(this->bar, bar_len, "\r[%s]    0    00:00", frame);

		printf("%s\n", title);
		fputs(this->bar, stdout);
		delete[] frame;

		this->start = system_clock::now();
	}

	IndefiniteBar::~IndefiniteBar()
	{
		delete[] this->bar;
	}

	void IndefiniteBar::print(bool completed = false)
	{
		long long diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();
		snprintf
		(
			this->nums, INDEF_RFRAME_WIDTH, "%zd    %02lld:%02lld",
			this->processed, diff_s / 60, diff_s % 60
		);
		fputs(this->bar, stdout);

		if (completed)
			printf("\nCompleted in %02lld:%02lld.\n\n", diff_s / 60, diff_s % 60);
	}

	void IndefiniteBar::complete()
	{
		this->print(true);
	}

	void IndefiniteBar::update(size_t processed)
	{
		this->processed += processed;

		if (this->processed < this->next_update)
			return;
		this->next_update += this->update_ticks;

		if (++this->right == PROGRESS_WIDTH)
			this->right = 0;

		this->frame[this->right] = progress_symbol;
		this->frame[this->left] = '_';

		if (++this->left == PROGRESS_WIDTH)
			this->left = 0;

		this->print();
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

	void ProgressBar::write_nums()
	{
		long long diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();
		snprintf(this->nums, PB_RFRAME_WIDTH, "%zd / %zd    %zd %%    %02lld:%02lld",
			size_t(this->current), size_t(this->total), size_t((this->current * 100) / this->total),
			diff_s / 60, diff_s % 60
		);
	}

	void ProgressBar::update(float ticks)
	{
		if (this->current == this->total)
			return;

		this->current += ticks;

		if (this->current < this->next_update)
			return;
		this->next_update += this->update_ticks;
		if (this->current > this->total)
			this->current = this->total;

		size_t draw_count = (this->current / this->block_ticks) - (this->draw_pos - 2);
		if (draw_count > 0)
			for (size_t i = 0; i < draw_count; i++)
				this->bar[this->draw_pos++] = progress_symbol;

		this->write_nums();
		fputs(this->bar, stdout);

		if (this->current != this->total)
			return;
		long long diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();
		printf("\nCompleted in %02lld:%02lld.\n\n", diff_s / 60, diff_s % 60);
	}
}
