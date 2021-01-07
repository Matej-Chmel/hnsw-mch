#include "bound_progress_functions.h"

namespace mch
{
	BoundProgressFunctions::BoundProgressFunctions(CloseFunc close, StartFunc start, UpdateFunc update):
		close(close), start(start), update(update)
	{}
}
