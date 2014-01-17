// mutation.h

#ifndef __MUTATION_H
#define __MUTATION_H

#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#ifdef _DEBUG
#include <iostream>
#endif


template < typename Genome, typename RGen = boost::random::mt19937 >
struct basic_real_mutation
{
	typedef Genome genome_t;

	double max_perturbation;
	boost::random::uniform_01<> rdist_01;
	boost::random::uniform_real_distribution<> rdist_clamped;
	RGen& rgen;

	basic_real_mutation(RGen& rg, double max_perturb = 0.3):
		max_perturbation(max_perturb),
		rdist_clamped(-1.0, 1.0),
		rgen(rg)
	{}

	template < typename Individual >
	inline void operator() (Individual& child, double rate)
	{
		// For each gene, dependent upon mutation rate...
		for(auto& gene: child.genome)
		{
			if(rdist_01(rgen) < rate)
			{
				// ...add or subtract a small value.
				gene += rdist_clamped(rgen) * max_perturbation;

#ifdef _DEBUG
				std::cout << "m ";
#endif
			}
#ifdef _DEBUG
			else { std::cout << "x "; }
#endif
		}
	}
};


#endif


