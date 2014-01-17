// truncation_survival_selection.h

#ifndef __TRUNCATION_SURVIVAL_SELECTION_H
#define __TRUNCATION_SURVIVAL_SELECTION_H

/*
Selects by truncating the population based on fitness ordering.

Requires a SortableFitness compliant fitness type.
*/

template < typename IndividualIterator >
class truncation_survival_selection
{
private:
	typedef IndividualIterator	iterator_t;

	iterator_t pop_start, pop_end;

public:
	truncation_survival_selection(iterator_t p_start, iterator_t p_end):
		pop_start(p_start),
		pop_end(p_end)
	{
		init();
	}

private:
	void init()
	{
		// Sort population in descending fitness
		std::sort(pop_start, pop_end);
	}
	
public:
	template < typename OutputIterator >
	void select_individuals(size_t count, OutputIterator dest)
	{
		std::copy(pop_start, pop_start + count, dest);
	}
};


#endif


