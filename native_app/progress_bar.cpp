#include "progress_bar.h"

namespace mch
{
	ll ProgressBar::get_diff_s()
	{
		return duration_cast<seconds>(system_clock::now() - this->start_point).count();
	}
	void ProgressBar::write_nums()
	{
		ll diff_s = this->get_diff_s();

		snprintf(this->nums, progress::RFRAME_WIDTH, "%zd / %zd    %zd %%    " PROGRESS_FORMAT,
			size_t(this->current), size_t(this->total), size_t((this->current * 100) / this->total),
			diff_s / 60, diff_s % 60
		);
	}
	ProgressBar::ProgressBar():
		bar(new char[progress::BAR_WIDTH]), block_ticks(0), current(0), draw_pos(0), next_update(0),
		nums(this->bar + progress::NUMS_WIDTH), total(0), update_ticks(0)
	{}
	ProgressBar& ProgressBar::get_instance()
	{
		static ProgressBar instance;
		return instance;
	}
	ProgressBar::~ProgressBar()
	{
		delete[] this->bar;
	}
	BoundProgressFunctions ProgressBar::bind()
	{
		return
		{
			std::bind(&ProgressBar::close, this),
			std::bind(&ProgressBar::start, this, _1, _2),
			std::bind(&ProgressBar::update, this)
		};
	}
	void ProgressBar::close()
	{
		ll diff_s = this->get_diff_s();
		printf("\nCompleted in " PROGRESS_FORMAT ".\n\n", diff_s / 60, diff_s % 60);
	}
	void ProgressBar::start(const char* title, size_t total)
	{
		this->block_ticks = total / progress::DECIMAL_WIDTH;
		this->current = 0;
		this->draw_pos = 2;
		this->total = total;
		this->update_ticks = this->total / 100;
		this->next_update = this->update_ticks;

		char* frame = new char[progress::C_STR_WIDTH];
		memset(frame, '_', progress::WIDTH);
		frame[progress::WIDTH] = '\0';
		snprintf(this->bar, progress::BAR_WIDTH, "\r[%s]    0 / 0    0 %%    00:00", frame);
		delete[] frame;

		printf("%s\n", title);
		fputs(this->bar, stdout);
		this->start_point = system_clock::now();
	}
	void ProgressBar::update()
	{
		this->current++;

		if(this->current < this->next_update)
			return;

		this->next_update += this->update_ticks;

		size_t draw_count = size_t((this->current / this->block_ticks) - float(this->draw_pos - 2));

		if(draw_count > 0)
			for(size_t i = 0; i < draw_count; i++)
				this->bar[this->draw_pos++] = progress::SYMBOL;

		this->write_nums();
		fputs(this->bar, stdout);
	}
}
