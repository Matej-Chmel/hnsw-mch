#include <cmath>
#include <cstdio>
#include <cstring>
#include <thread>
#include "console.hpp"
#include "concurrent_progress.hpp"
using namespace std::this_thread;

namespace mch
{
	atomic_size_t _bars_completed = 0;
	char progress_symbol = '@';

	ExclusiveBar::ExclusiveBar():
		_mutex(make_unique<mutex>()),
		_update_lock(*_mutex, defer_lock), _print_lock(*_mutex, defer_lock),
		running(true), current(0), draw_pos(1),
		bar(new char[PROGRESS_WIDTH + 49])
	{
		char* frame = new char[PROGRESS_WIDTH + 1];
		this->nums = this->bar + PROGRESS_WIDTH + 6;

		memset(frame, '_', PROGRESS_WIDTH);
		frame[PROGRESS_WIDTH] = '\0';
		snprintf(this->bar, PROGRESS_WIDTH + 49, "[%s]    waiting  0 %%\n", frame);

		delete[] frame;
		this->start = system_clock::now();
	}

	ExclusiveBar::~ExclusiveBar() 
	{
		delete[] this->bar;
	}

	void ExclusiveBar::print_to(char*& dest)
	{
		this->_print_lock.lock();
		char* ptr = this->bar;
		while(*ptr)
			*dest++ = *ptr++;
		this->_print_lock.unlock();
	}

	void ExclusiveBar::write_nums()
	{
		snprintf
		(
			this->nums, PB_RFRAME_WIDTH, "%10zd / %10zd    %3zd %%\n",
			size_t(this->current), size_t(this->total),
			size_t((this->current * 100) / this->total)
		);
	}

	void ExclusiveBar::abort() 
	{
		if (!this->running)
			return;
		_bars_completed++;
		this->running = false;
	}

	void ExclusiveBar::init(float total)
	{
		this->block_ticks = total / float(PROGRESS_WIDTH);
		this->update_ticks = this->block_ticks / 2.0f;
		this->next_update = update_ticks;
		this->total = total;
	}

	void ExclusiveBar::update(float ticks)
	{
		if (!this->running)
			return;

		this->current += ticks;

		if (this->current < this->next_update)
			return;

		this->next_update += this->update_ticks;

		if (this->current >= this->total)
		{
			long long diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();
			this->running = false;
			this->current = this->total;

			this->_update_lock.lock();

			memset(this->bar + 1, progress_symbol, PROGRESS_WIDTH);
			this->write_nums();

			snprintf
			(
				this->nums + 32, PB_RFRAME_WIDTH - 32, "    %02lld:%02lld\n",
				diff_s / 60, diff_s % 60
			);

			this->_update_lock.unlock();
			_bars_completed++;
			return;
		}

		size_t draw_count = (this->current / this->block_ticks) - (this->draw_pos - 1);

		this->_update_lock.lock();

		if (draw_count > 0)
			for (size_t i = 0; i < draw_count; i++)
				this->bar[this->draw_pos++] = progress_symbol;

		this->write_nums();
		
		this->_update_lock.unlock();
	}

	ConcurrentProgress::ConcurrentProgress(const char* title, size_t workers_len):
		bars_len(workers_len)
	{
		_bars_completed = 0;
		this->bars.reserve(workers_len);
		this->out = new char[workers_len * (PROGRESS_WIDTH + 49) + 7];

		for(size_t i = 0; i < workers_len; i++)
			this->bars.emplace_back();

		printf("%s\n\n", title);
		this->start = system_clock::now();
	}

	ConcurrentProgress::~ConcurrentProgress() 
	{
		delete[] this->out;
	}

	void ConcurrentProgress::run()
	{
		save_caret();

		for(;;)
		{
			char* ptr = this->out;

			for(ExclusiveBar& item: this->bars)
				item.print_to(ptr);
			
			long long diff_s = duration_cast<seconds>(system_clock::now() - this->start).count();

			if (_bars_completed >= this->bars_len)
			{
				*ptr = '\0';
				printf("%s\nCompleted in %02lld:%02lld.\n\n", this->out, diff_s / 60, diff_s % 60);
				return;
			}

			snprintf(ptr, 7, "\n%02lld:%02lld", diff_s / 60, diff_s % 60);
			fputs(this->out, stdout);

			sleep_for(REFRESH_DELAY);
			load_caret();
		}
	}
}
