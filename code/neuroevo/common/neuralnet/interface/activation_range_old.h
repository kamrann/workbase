// activation_range.h

#ifndef __WB_NN_ACTIVATION_RANGE_H
#define __WB_NN_ACTIVATION_RANGE_H

#include "util/multi_interval.h"

#include <boost/optional.hpp>

#include <sstream>
#include <limits>
#include <algorithm>


namespace nnet {

	namespace bint = boost::numeric;

	typedef multi_interval<
		double,
		bint::interval_lib::policies<
		bint::interval_lib::save_state< bint::interval_lib::rounded_transc_std< double > >,
		bint::interval_lib::checking_base< double >
		>
	> multi_itv_base;

	class activation_range:
		public multi_itv_base
	{
		double lower_bound;
		double upper_bound;
		bool binary;

		activation_range(
			double bound1,
			double bound2,
			bool _binary = false
			):
			lower_bound(std::min(bound1, bound2)),
			upper_bound(std::max(bound1, bound2)),
			binary(_binary)
		{}

		inline void set_zero()
		{
			lower_bound = upper_bound = 0.0;
			binary = true;	// TODO: ?? 
		}

		static inline activation_range unbounded()
		{
			return activation_range{ -infinity(), infinity() };
		}

		static inline double infinity()
		{
			return std::numeric_limits< double >::max();
		}

		inline bool is_bounded_below() const
		{
			return lower_bound != -infinity();
		}

		inline bool is_bounded_above() const
		{
			return upper_bound != infinity();
		}

		inline bool is_bounded() const
		{
			return is_bounded_below() && is_bounded_above();
		}

		inline bool is_binary() const
		{
			return binary;
		}

		inline bool contains(double value) const
		{
			return value >= lower_bound && value <= upper_bound;
		}

		inline double span() const
		{
			if(is_bounded())
			{
				return upper_bound - lower_bound;
			}
			else
			{
				return infinity();
			}
		}

		inline void multiply_by(double value)
		{
			if(value == 0.0)
			{
				// TODO: Currently not supporting an empty range, but maybe want to
				// ie. continuous, with 2 equal and exclusive bounds
				set_zero();
				return;
			}

			if(is_bounded_below())
			{
				lower_bound *= value;
			}
			else if(value < 0.0)
			{
				lower_bound = -lower_bound;
			}

			if(is_bounded_above())
			{
				upper_bound *= value;
			}
			else if(value < 0.0)
			{
				upper_bound = -upper_bound;
			}

			if(value < 0.0)
			{
				std::swap(lower_bound, upper_bound);
			}
		}

		inline std::string enclosed_string(std::string const& str) const
		{
			if(is_binary())
			{
				return "{" + str + "}";
			}
			else
			{
				return "(" + str + ")";
			}
		}

		static inline std::string bound_string(double bound, size_t precision = 1)
		{
			if(bound == infinity())
			{
				return "\342\210\236";//"\u221E";
			}
			else if(bound == -infinity())
			{
				return "-\xE2\x88\x9E";//"-\u221E";
			}
			else
			{
				std::stringstream ss;
				std::fixed(ss);
				ss.precision(precision);
				ss << bound;
				return ss.str();
			}
		}

		inline std::string to_string(size_t precision = 1) const
		{
			std::string str = bound_string(lower_bound, precision);
			auto const bounds_equal = (lower_bound == upper_bound);
			if(!(is_binary() && bounds_equal))
			{
				str += ", " + bound_string(upper_bound, precision);
			}
			return enclosed_string(str);
		}
	};

}


#endif


