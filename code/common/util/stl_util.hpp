// stl_util.hpp
/*!
Adds some extensions to the std namespace.
*/

#ifndef __STL_UTIL_H
#define __STL_UTIL_H

#include <algorithm>
#include <functional>
#include <ratio>
#include <cstdint>


namespace std
{
	// Returns iterator pointing to the element preceding the first element found equal to value, or last if no such element exists
	template < typename InputIterator, typename T >
	inline InputIterator find_preceding(InputIterator first, InputIterator last, const T& value)
	{
		InputIterator prec = last;
		for(; first != last; prec = first++)
		{
			if(*first == value)
				break;
		}
		return prec;
	}

	// Returns iterator pointing to the element succeeding the first element found equal to value, or last if no such element exists
	template < typename InputIterator, typename T >
	inline InputIterator find_succeeding(InputIterator first, InputIterator last, const T& value)
	{
		for(; first != last; ++first)
		{
			if(*first == value)
			{
				++first;
				break;
			}
		}
		return first;
	}

	// Naive implementation to cycle through k-permutations using iterators on an n-container
	template < typename RandomAccessIterator >
	inline bool next_k_permutation(RandomAccessIterator first, RandomAccessIterator mid, RandomAccessIterator last)
	{
		typedef typename std::iterator_traits< RandomAccessIterator >::value_type value_type;

		std::sort(mid, last, std::greater< value_type >());
		return std::next_permutation(first, last);
	}

	/*! Generate count sequential values starting from initial */
	template < typename OutputIterator >
	inline void generate_n_sequential(OutputIterator first, size_t count, typename OutputIterator::value_type initial = typename OutputIterator::value_type())
	{
		std::generate_n(first, count, [&initial] { return initial++; });
	}


	/* Multiply a compile time ratio by a runtime integer, given result as a whole number */
	template <
		std::intmax_t Num,
		std::intmax_t Den
	>
	inline std::intmax_t ratio_product(std::intmax_t value)
	{
		return value * Num / Den;
	}

	template <
		typename Ratio
	>
	inline std::intmax_t ratio_product(std::intmax_t value)
	{
		return ratio_product< Ratio::num, Ratio::den >(value);
	}

	/* Return the max/min of two compile time ratios */
	template <
		typename R1,
		typename R2,
		bool R1_Greater
	>
	struct ratio_minmax_impl
	{
		typedef R1 max_type;
		typedef R2 min_type;
	};

	template <
		typename R1,
		typename R2
	>
	struct ratio_minmax_impl< R1, R2, false >
	{
		typedef R2 max_type;
		typedef R1 min_type;
	};

	template <
		typename R1,
		typename R2
	>
	using ratio_max = typename ratio_minmax_impl< R1, R2, ratio_greater< R1, R2 >::value >::max_type;

	template <
		typename R1,
		typename R2
	>
	using ratio_min = typename ratio_minmax_impl< R1, R2, ratio_greater< R1, R2 >::value >::min_type;

}


#endif