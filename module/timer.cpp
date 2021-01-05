#include "timer.h"

namespace mch
{
	Timer::Timer()
	{}
	void Timer::start()
	{
		this->_start = system_clock::now();
	}
	ll Timer::stop()
	{
		return duration_cast<milliseconds>(system_clock::now() - this->_start).count();
	}
}
