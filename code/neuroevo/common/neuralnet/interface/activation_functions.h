// activation_functions.h

#ifndef __WB_NN_ACTIVATION_FUNCTIONS_H
#define __WB_NN_ACTIVATION_FUNCTIONS_H

#include <sstream>
#include <limits>


namespace nnet {

	enum class ActivationFn {
		// Asymmetric functions (output always 0 or positive)
		Sigmoid,				// (0, 1)
		Gaussian,				// (0, 1) Normal distribution function _n_
		Threshold,				// {0, 1} Output dependent only on sign of input

		// Functions without output range spanning the x axis
		_SymmetricStart,

		Linear = _SymmetricStart,
		LinearBounded,			// TODO: Clamping? search for '150' within FANN run method
		SigmoidSymmetric,		// (-1, 1)	Note that this is the same as the hyperbolic tangent.
		GaussianSymmetric,		// (-1, 1)
		ThresholdSymmetric,		// {-1, 1}
		Sine,					// (-1, 1)
		Cosine,					// (-1, 1)

		_SymmetricEnd,

		Count = _SymmetricEnd,

		None,
	};

	extern const char* const ActivationFnNames[(int)ActivationFn::Count];

	inline bool is_symmetric(ActivationFn fn)
	{
		return fn >= ActivationFn::_SymmetricStart && fn < ActivationFn::_SymmetricEnd;
	}

	inline bool is_bounded(ActivationFn fn)
	{
		return fn != ActivationFn::Linear;
	}

	inline bool is_bounded_below(ActivationFn fn)
	{
		return fn != ActivationFn::Linear;
	}

	inline bool is_bounded_above(ActivationFn fn)
	{
		return fn != ActivationFn::Linear;
	}

	inline bool is_binary(ActivationFn fn)
	{
		return fn == ActivationFn::Threshold || fn == ActivationFn::ThresholdSymmetric;
	}

	struct activation_range
	{
		double lower_bound;
		double upper_bound;
		bool binary;

		activation_range(
			double lower,
			double upper,
			bool _binary = false
			):
			lower_bound(lower),
			upper_bound(upper),
			binary(_binary)
		{}

		static inline double infinity()
		{
			return std::numeric_limits< double >::max();
		}

		inline bool is_bounded() const
		{
			return lower_bound != -infinity() && upper_bound != infinity();
		}

		inline bool is_binary() const
		{
			return binary;
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
			std::string str =
				bound_string(lower_bound, precision)
				+ ", "
				+ bound_string(upper_bound, precision);
			return enclosed_string(str);
		}
	};

	inline activation_range get_function_range(ActivationFn fn)
	{
		return activation_range{
			is_bounded_below(fn) ? (is_symmetric(fn) ? -1.0 : 0.0) : -activation_range::infinity(),
			is_bounded_above(fn) ? 1.0 : activation_range::infinity(),
			is_binary(fn)
		};
	}

}


#endif


