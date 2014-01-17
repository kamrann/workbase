// basic_rate_functors.h

#ifndef __BASIC_RATE_FUNCTORS_H
#define __BASIC_RATE_FUNCTORS_H


template < size_t DefaultPercent >
struct FixedRateFtr
{
	double rate;

	FixedRateFtr(double r = DefaultPercent / 100.0): rate(r)
	{}

	// TODO: replace this arg with generic evolution state information
	inline double operator() (double evo_stage) const
	{
		return rate;
	}
};

typedef FixedRateFtr< 70 > FixedCrossoverRate;
typedef FixedRateFtr< 10 > FixedMutationRate;


template < size_t DefaultInitial, size_t DefaultFinal >
struct LinearRateFtr
{
	double initial_rate, final_rate;

	LinearRateFtr(double _initial = DefaultInitial / 100.0, double _final = DefaultFinal): initial_rate(_initial), final_rate(_final)
	{}

	inline double operator() (double evo_stage) const
	{
		return initial_rate + (final_rate - initial_rate) * evo_stage;
	}
};

typedef LinearRateFtr< 50, 80 > LinearCrossoverRate;
typedef LinearRateFtr< 20, 5 > LinearMutationRate;


#endif


