#ifndef MCH_RANDOM
#define MCH_RANDOM

#include <random>

namespace mch
{
	using namespace std;

	class RndDouble
	{
	private:
		mt19937_64 gen;
		uniform_real_distribution<double> distr;
	
	public:
		RndDouble();

		double operator()();
	};
}

#endif
