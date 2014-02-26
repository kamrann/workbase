// fixed_or_random.h

#ifndef __FIXED_OR_RANDOM_H
#define __FIXED_OR_RANDOM_H

#include <boost/variant.hpp>


// TODO: Will need some generalisation to be able to apply this to vectors, orientations, etc
template <
	typename T,
	typename Dist,
	typename RGen
>
class fixed_or_random
{
public:
	fixed_or_random(T const& fixed = T()): m_val(fixed)
	{}

	fixed_or_random(T const& rand_range_start, T const& rand_range_end): m_val(std::make_pair(rand_range_start, rand_range_end))
	{}

	inline T get(RGen& rgen) const
	{
		visitor vis(rgen);
		return boost::apply_visitor(vis, m_val);
	}

private:
	typedef Dist distribution_t;

	typedef boost::variant<
		T,
		std::pair< T, T >
	> var_t;

	struct visitor: public boost::static_visitor< T >
	{
		visitor(RGen& _rgen): rgen(_rgen)
		{}

		inline T operator() (T const& v)
		{
			return v;
		}

		inline T operator() (std::pair< T, T > const& v)
		{
			distribution_t dist(v.first, v.second);
			return dist(rgen);
		}

		RGen& rgen;
	};

private:
	var_t m_val;
};


#endif

