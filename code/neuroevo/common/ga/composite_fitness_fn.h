// composite_fitness_fn.h

#ifndef __COMPOSITE_FITNESS_FN_H
#define __COMPOSITE_FITNESS_FN_H

#include "fitness.h"

#include <boost/function.hpp>

#include <vector>


template < typename EpochData, typename RealFitnessType = basic_real_fitness >
class linear_comb
{
private:
	typedef EpochData			epoch_data_t;
	typedef RealFitnessType		fitness_t;

	typedef boost::function< fitness_t (epoch_data_t) >	fitness_fn_t;
	typedef std::pair< fitness_fn_t, double > entry;

	typedef std::vector< entry >					components_t;
	typedef typename components_t::const_iterator	comp_it;

	components_t components;

public:
	linear_comb(fitness_fn_t const& fn1, double w1, fitness_fn_t const& fn2)
	{
		components.push_back(entry(fn1, w1));
		components.push_back(entry(fn2, 1.0 - w1));
	}

	linear_comb(fitness_fn_t const& fn1, double w1, fitness_fn_t const& fn2, double w2, fitness_fn_t const& fn3)
	{
		components.push_back(entry(fn1, w1));
		components.push_back(entry(fn2, w2));
		components.push_back(entry(fn3, 1.0 - w1 - w2));
	}

public:
	fitness_t operator() (epoch_data_t const& ed) const
	{
		comp_it it = components.begin();
		fitness_t retval = it->first(ed) * it->second;
		while(++it != components.end())
		{
			retval = retval + it->first(ed) * it->second;
		}
		return retval;
	}
};


template < typename EpochData, typename RealFitnessType = basic_real_fitness >
class product
{
private:
	typedef EpochData			epoch_data_t;
	typedef RealFitnessType		fitness_t;

	typedef boost::function< fitness_t (epoch_data_t) >	fitness_fn_t;

	typedef std::vector< fitness_fn_t >				components_t;
	typedef typename components_t::const_iterator	comp_it;

	components_t components;

public:
	product(fitness_fn_t const& fn1, fitness_fn_t const& fn2)
	{
		components.push_back(fn1);
		components.push_back(fn2);
	}

	product(fitness_fn_t const& fn1, fitness_fn_t const& fn2, fitness_fn_t const& fn3)
	{
		components.push_back(fn1);
		components.push_back(fn2);
		components.push_back(fn3);
	}

public:
	fitness_t operator() (epoch_data_t const& ed) const
	{
		comp_it it = components.begin();
		fitness_t retval = (*it)(ed);
		while(++it != components.end())
		{
			retval = retval * (*it)(ed);
		}
		return retval;
	}
};


template < typename EpochData, typename RealFitnessType = basic_real_fitness >
class quotient
{
private:
	typedef EpochData			epoch_data_t;
	typedef RealFitnessType		fitness_t;

	typedef boost::function< fitness_t (epoch_data_t) >	fitness_fn_t;

	fitness_fn_t numer;
	fitness_fn_t denom;

public:
	quotient(fitness_fn_t const& numerator, fitness_fn_t const& denominator): numer(numerator), denom(denominator)
	{}

public:
	fitness_t operator() (epoch_data_t const& ed) const
	{
		return numer(ed) / denom(ed);
	}
};


#endif


