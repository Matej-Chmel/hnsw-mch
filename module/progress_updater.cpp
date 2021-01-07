#include "progress_updater.h"

namespace mch
{
	ProgressUpdater::ProgressUpdater(BoundProgressFunctions bound): ProgressUpdater(bound.close, bound.start, bound.update)
	{}
	ProgressUpdater::ProgressUpdater(CloseFunc close, StartFunc start, UpdateFunc update): close(close), start(start), update(update)
	{}
}
