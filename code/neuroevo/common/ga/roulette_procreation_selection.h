// roulette_procreation_selection.h

#ifndef __ROULETTE_PROCREATION_SELECTION_H
#define __ROULETTE_PROCREATION_SELECTION_H

#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

/*
Note: could do kind of roulette with SortableFitness, whereby a parameter specifies how much more likely the most fit is to 
reproduce than the least fit. This might in some cases be better even when RealFitness is available, as it doesn't rely on the
ability of the fitness function to produce sensibly distributed fitnesses.
*/

/*
Selects proportionally based on fitness values.

Requires a RealFitness compliant fitness type.
*/

template < typename FitnessType, typename RGen = boost::random::mt19937 >
class roulette_procreation_selection
{
private:
	typedef FitnessType fitness_t;

	boost::random::uniform_real_distribution<> dist;
	RGen& rgen;

public:
	roulette_procreation_selection(RGen& rg):
		rgen(rg)
	{}

private:
	template < typename InputIterator >
	fitness_t calc_total_fitness(InputIterator pop_start, InputIterator pop_end)
	{
		fitness_t total(0.0);
		while(pop_start != pop_end)
		{
			total += pop_start->fitness;

			++pop_start;
		}

		return total;
	}

	template < typename InputIterator >
	size_t get_index_by_rand_point(InputIterator pop_start, fitness_t const& rand_point)
	{
		// TODO: Issue of potential 0 fitnesses - in theory could be so many 0 values that there are not enough non-0s
		// to select N distinct parents...

		fitness_t cumulative(0.0);
		size_t index = 0;
/*		while(pop_fit[gn_index].is_unfit())
		{
			++gn_index;
		}
*/		while(rand_point >= cumulative + pop_start->fitness)
		{
			cumulative += pop_start->fitness;
			++index;
			++pop_start;
/*			while(pop_fit[gn_index].is_unfit())
			{
				++gn_index;
			}
*/		}

//		assert(gn_index < pop_fit.size());
		return index;
	}

public:
	template < typename InputIterator >
	void preprocess_population(InputIterator pop_start, InputIterator pop_end)
	{
		// TODO: Need to use .value here suggests we should maybe simplify basic_real_fitness to essentially be a double value
		// Otherwise need to build in some method to allow this functionality without direct double conversion
		dist = boost::random::uniform_real_distribution<>(0.0, calc_total_fitness(pop_start, pop_end).value);
	}

	template < typename InputIterator, typename OutputIterator >
	void select_parents(size_t num_parents, InputIterator pop_start, InputIterator pop_end, OutputIterator dest)
	{
		std::set< size_t > selected;
		for(size_t p = 0; p < num_parents; ++p)
		{
			size_t index;
			do
			{
				index = get_index_by_rand_point(pop_start, dist(rgen));
			}
			while(selected.find(index) != selected.end());
			
			*dest = &pop_start[index].genome;
			++dest;

			selected.insert(index);
		}
	}
};


#endif


