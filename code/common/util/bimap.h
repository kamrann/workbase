// bimap.h

#ifndef __WB_UTIL_BIMAP_H
#define __WB_UTIL_BIMAP_H

#include <boost/bimap.hpp>


template < typename LeftKey, typename RightKey >
class bimap: public boost::bimap < LeftKey, RightKey >
{
public:
	typedef boost::bimap< LeftKey, RightKey > base_t;
	using base_t::base_t;

	bimap(std::initializer_list< typename base_t::value_type > il): base_t{ std::begin(il), std::end(il) }
	{}

public:
	inline RightKey at(LeftKey k) const
	{
		return left.find(k)->second;
	}

	inline LeftKey at(RightKey k) const
	{
		return right.find(k)->second;
	}
};


#endif


