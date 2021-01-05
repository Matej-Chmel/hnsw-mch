#pragma once
#include "mch.h"

namespace mch
{
	class Timer
	{
		tp _start;

	public:
		Timer();

		void start();
		ll stop();
	};
}
