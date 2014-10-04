// interval_io.h

#ifndef __WB_INTERVAL_IO_H
#define __WB_INTERVAL_IO_H

#include "multi_interval.h"
#include "text_io.h"

#include <ostream>
#include <istream>


namespace bint = boost::numeric;


template < typename T, typename Policies >
std::ostream& operator<< (std::ostream& strm, bint::interval< T, Policies > const& i)
{
	static const std::string infinity_char = "\342\210\236";
	if(bint::singleton(i))
	{
		strm << i.lower();
	}
	else
	{
		strm << "[";
		if(bint::is_negatively_bounded(i))
		{
			strm << i.lower();
		}
		else
		{
			strm << "-" << infinity_char;
		}
		strm << ", ";
		if(bint::is_positively_bounded(i))
		{
			strm << i.upper();
		}
		else
		{
			strm << infinity_char;
		}
		strm << "]";
	}
	return strm;
}

template < typename T, typename Policies, template <typename, typename> class Approximator >
inline std::ostream& operator<< (std::ostream& strm, multi_interval< T, Policies, Approximator > const& mitv)
{
	auto braces = !mitv.is_single_interval() || mitv.is_single_value();
	if(braces)
	{
		strm << "{";
	}
	bool initial = true;
	for(auto const& sub : mitv)
	{
		if(!initial)
		{
			strm << ", ";
		}
		strm << sub;
		initial = false;
	}
	if(braces)
	{
		strm << "}";
	}
	return strm;
}

template < typename T, typename Policies >
std::istream& operator>> (std::istream& strm, bint::interval< T, Policies >& i)
{
	double l, u;
	strm >> std::ws;
	if(strm.peek() == '[')
	{
		strm >> eat("[") >> l >> eat(",") >> u >> eat("]");
	}
	else
	{
		strm >> l;
		u = l;
	}

	if(strm)
	{
		i.assign(l, u);
	}

	return strm;
}

template < typename T, typename Policies, template <typename, typename> class Approximator >
std::istream& operator>> (std::istream& strm, multi_interval< T, Policies, Approximator >& i)
{
	multi_interval< T, Policies, Approximator > result;

	strm >> std::ws;
	auto open = strm.peek();
	if(open == std::char_traits< char >::to_int_type('{'))
	{
		strm >> eat("{");
		bool first = true;
		while(strm && !(strm >> std::ws).eof() && strm.peek() != std::char_traits< char >::to_int_type('}'))
		{
			if(!first)
			{
				strm >> eat(",");
			}

			multi_interval< T, Policies, Approximator >::interval itv;
			strm >> itv;
			result.insert(itv);
			first = false;
		}
		strm >> eat("}");
	}
	else if(open == std::char_traits< char >::to_int_type('['))
	{
		multi_interval< T, Policies, Approximator >::interval itv;
		strm >> itv;
		result.insert(itv);
	}
	else
	{
		strm.setstate(std::ios::failbit);
	}

	if(strm)
	{
		i = std::move(result);
	}

	return strm;
}



#endif


