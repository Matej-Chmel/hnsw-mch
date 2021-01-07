#pragma once
#include "mch.h"

namespace mch
{
	typedef function<void()> CloseFunc;
	typedef function<void(const char*, size_t)> StartFunc;
	typedef function<void()> UpdateFunc;

	class BoundProgressFunctions
	{
	public:
		CloseFunc close;
		StartFunc start;
		UpdateFunc update;

		BoundProgressFunctions(CloseFunc close, StartFunc start, UpdateFunc update);
	};
}
