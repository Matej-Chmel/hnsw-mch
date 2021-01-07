#pragma once
#include "bound_progress_functions.h"

namespace mch
{
	class ProgressUpdater
	{
	public:
		CloseFunc close;
		StartFunc start;
		UpdateFunc update;

		ProgressUpdater(BoundProgressFunctions bound);
		ProgressUpdater(CloseFunc close, StartFunc start, UpdateFunc update);
	};
}
