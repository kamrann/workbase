// random_procreation_selection.h

#ifndef __RANDOM_PROCREATION_SELECTION_H
#define __RANDOM_PROCREATION_SELECTION_H

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <set>

/*
Selects parents randomly from the parent population.
*/

template < typename RGen = boost::random::mt19937 >
class random_procreation_selection
{
private:
	RGen& rgen;

public:
	random_procreation_selection(RGen& rg):
		rgen(rg)
	{}

private:
	
public:
	template < typename InputIterator >
	void preprocess_population(InputIterator pop_start, InputIterator pop_end)
	{}

	template < typename InputIterator, typename OutputIterator >
	void select_parents(size_t num_parents, InputIterator pop_start, InputIterator pop_end, OutputIterator dest)
	{
		boost::random::uniform_int_distribution<> const rdist(0, pop_end - pop_start - 1);

		std::set< size_t > selected;
		for(size_t p = 0; p < num_parents; ++p)
		{
			size_t index;
			do
			{
				index = rdist(rgen);
			}
			while(selected.find(index) != selected.end());
			
			*dest = &pop_start[index].genome;
			++dest;

			selected.insert(index);
		}
	}
};


#endif


