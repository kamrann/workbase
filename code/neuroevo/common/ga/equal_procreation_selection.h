// equal_procreation_selection.h

#ifndef __EQUAL_PROCREATION_SELECTION_H
#define __EQUAL_PROCREATION_SELECTION_H

#include <boost/random/mersenne_twister.hpp>

/*
Selects parents from the parent population one after the other.
*/

template < typename RGen = boost::random::mt19937 >
class equal_procreation_selection
{
private:
	size_t current;

public:
	equal_procreation_selection(RGen& rgen): current(0)
	{}

private:

public:
	template < typename InputIterator >
	void preprocess_population(InputIterator pop_start, InputIterator pop_end)
	{
		current = 0;
	}

	template < typename InputIterator, typename OutputIterator >
	void select_parents(size_t num_parents, InputIterator pop_start, InputIterator pop_end, OutputIterator dest)
	{
		size_t const parent_pop_size = pop_end - pop_start;
		for(size_t p = 0; p < num_parents; ++p)
		{
			size_t const index = current++;
			current %= parent_pop_size;
						
			*dest = &pop_start[index];
			++dest;
		}
	}
};


#endif


