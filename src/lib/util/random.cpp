#include "random.hpp"

namespace mch
{
	RndDouble::RndDouble() : distr(0.0, 1.0)
	{
		random_device rd;
		this->gen = mt19937_64(rd());
	}

	double RndDouble::operator()()
	{
		return this->distr(this->gen);
	}
}
