// proportional_selection.h

#ifndef __PROPORTIONAL_SELECTION_H
#define __PROPORTIONAL_SELECTION_H

#include <iterator>


template <
	typename Iterator,
	typename CumulativeType
>
inline Iterator select_proportionally(Iterator first, Iterator last, double rand_point)
{
	CumulativeType cumulative(0);
	while(first != last && rand_point >= cumulative + *first)
	{
		cumulative += *first;
		++first;
	}
	return first;
}

template <
	typename Iterator
>
inline Iterator select_proportionally(Iterator first, Iterator last, double rand_point)
{
	return select_proportionally< Iterator, std::iterator_traits< Iterator >::value_type >(first, last, rand_point);
}


#endif


