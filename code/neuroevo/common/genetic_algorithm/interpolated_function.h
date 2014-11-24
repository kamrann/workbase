// interpolated_function.h

#ifndef __WB_GA_INTERPOLATED_FUNCTION_H
#define __WB_GA_INTERPOLATED_FUNCTION_H


namespace ga {

	template < typename ValueType, typename ControlType >
	struct interpolated_function
	{
		typedef ValueType value_t;
		typedef ControlType control_t;

		struct control_point
		{
			control_t control;
			value_t value;

			control_point(control_t c = {}, value_t v = {}):
				control(c),
				value(v)
			{}
		};

		interpolated_function(value_t initial, control_point cutoff):
			_cpoints({ control_point{ control_t{}, initial }, cutoff })
		{}

		// TODO: constructors for multi-stage functions, and also possibly interpolations other than linear

		inline double operator() (control_t pos) const
		{
			if(pos >= _cpoints.back().control)
			{
				return _cpoints.back().value;
			}

			size_t cp_idx = 1;
			auto const count = _cpoints.size();
			while(pos > _cpoints[cp_idx].control)
			{
				++cp_idx;
			}

			double fraction = (double)(pos - _cpoints[cp_idx - 1].control) /
				(_cpoints[cp_idx].control - _cpoints[cp_idx - 1].control);
			return interpolate(cp_idx - 1, cp_idx, fraction);
		}

		inline double interpolate(size_t idx1, size_t idx2, double fraction) const
		{
			// Linear
			return _cpoints[idx1].value + fraction * (_cpoints[idx2].value - _cpoints[idx1].value);
		}

		std::vector< control_point > _cpoints;
	};

}


#endif


