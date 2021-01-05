#include "config.h"
#include "util.h"

namespace mch
{
	constexpr size_t BUFFER_SIZE = 128;

	Config::Config(size_t ef, bool extend_candidates, bool keep_pruned, size_t m, float ml, size_t mmax, bool use_heuristic):
		ef(ef), extend_candidates(extend_candidates), keep_pruned(keep_pruned), m(m), ml(ml), mmax(mmax), use_heuristic(use_heuristic)
	{}
	string Config::to_string()
	{
		char buffer[BUFFER_SIZE];
		snprintf
		(
			buffer, BUFFER_SIZE,
			"ef: %zd\nextend_candidates: %s\nkeep_pruned: %s\nm: %zd\nml: %.3f\nmmax: %zd\nuse_heuristic: %s",
			this->ef, to_str(this->extend_candidates).c_str(), to_str(this->keep_pruned).c_str(),
			this->m, this->ml, this->mmax, to_str(this->use_heuristic).c_str()
		);
		return buffer;
	}
}
