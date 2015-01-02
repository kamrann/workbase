// activation_functions.h

#ifndef __WB_NN_ACTIVATION_FUNCTIONS_H
#define __WB_NN_ACTIVATION_FUNCTIONS_H

#include "activation_range.h"

#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>

#include <boost/optional.hpp>

#include <sstream>
#include <limits>
#include <algorithm>
#include <map>


namespace nnet {

	enum class ActivationFnType {
		// Asymmetric functions (output always 0 or positive)
		Sigmoid,				// (0, 1)
		Gaussian,				// (0, 1) Normal distribution function _n_
		LinearBounded,			// Clamped to (0, 1)
		Threshold,				// {0, 1} Output dependent only on sign of input

		// Functions with output range spanning the x axis
		_SymmetricStart,

		SigmoidSymmetric = _SymmetricStart,		// (-1, 1)	Note that this is the same as the hyperbolic tangent.
		GaussianSymmetric,		// (-1, 1)
		Linear,					// TODO: Clamping? search for '150' within FANN run method
		LinearBoundedSymmetric,	// Clamped to (-1, 1)
		ThresholdSymmetric,		// {-1, 1}
		Sine,					// (-1, 1)
		Cosine,					// (-1, 1)

		_SymmetricEnd,

		Count = _SymmetricEnd,

		None,
	};

	// TODO: Bimap, or perhaps spirit parser/emitter definitions
	extern const char* const ActivationFnNames[(int)ActivationFnType::Count];
	extern const char* const ActivationFnNames2[(int)ActivationFnType::Count];

	extern const std::map< std::string, ActivationFnType > ActivationFnNameMap;
	//

	inline bool is_symmetric(ActivationFnType fn)
	{
		return fn >= ActivationFnType::_SymmetricStart && fn < ActivationFnType::_SymmetricEnd;
	}

	inline bool is_bounded(ActivationFnType fn)
	{
		return fn != ActivationFnType::Linear;
	}

	inline bool is_bounded_below(ActivationFnType fn)
	{
		return fn != ActivationFnType::Linear;
	}

	inline bool is_bounded_above(ActivationFnType fn)
	{
		return fn != ActivationFnType::Linear;
	}

	inline bool is_binary(ActivationFnType fn)
	{
		return fn == ActivationFnType::Threshold || fn == ActivationFnType::ThresholdSymmetric;
	}

	inline bool is_increasing(ActivationFnType fn)
	{
		return fn != ActivationFnType::Sine && fn != ActivationFnType::Cosine &&
			fn != ActivationFnType::Gaussian && fn != ActivationFnType::GaussianSymmetric;
	}

	inline bool is_periodic(ActivationFnType fn)
	{
		return fn == ActivationFnType::Sine || fn == ActivationFnType::Cosine;
	}

	inline double function_period(ActivationFnType fn)
	{
		switch(fn)
		{
			case ActivationFnType::Sine:
			case ActivationFnType::Cosine:
			return 2.0 * M_PI;

			default:
			return 0.0;
		}
	}

	// TODO: This ideally should be based on fundamental properties available for each function, and thus not require
	// switch statement on individual function types.
	inline boost::optional< double > as_x_tends_to_negative_inf(ActivationFnType fn)
	{
		switch(fn)
		{
			case ActivationFnType::Linear:
			return activation_range::neg_infinity();

			case ActivationFnType::LinearBounded:
			case ActivationFnType::Threshold:
			case ActivationFnType::Sigmoid:
			case ActivationFnType::Gaussian:
			return 0.0;

			case ActivationFnType::LinearBoundedSymmetric:
			case ActivationFnType::ThresholdSymmetric:
			case ActivationFnType::SigmoidSymmetric:
			case ActivationFnType::GaussianSymmetric:
			return -1.0;

			default:
			return boost::none;
		}
	}

	inline boost::optional< double > as_x_tends_to_positive_inf(ActivationFnType fn)
	{
		switch(fn)
		{
			case ActivationFnType::Linear:
			return activation_range::pos_infinity();

			case ActivationFnType::LinearBounded:
			case ActivationFnType::Threshold:
			case ActivationFnType::Sigmoid:
			case ActivationFnType::Gaussian:
			case ActivationFnType::LinearBoundedSymmetric:
			case ActivationFnType::ThresholdSymmetric:
			case ActivationFnType::SigmoidSymmetric:
			case ActivationFnType::GaussianSymmetric:
			return 1.0;

			default:
			return boost::none;
		}
	}


	struct activation_function
	{
		ActivationFnType type;
		double steepness;

		activation_function(): type(ActivationFnType::None), steepness(1.0)
		{}
		activation_function(ActivationFnType _type, double _steepness = 1.0): type(_type), steepness(_steepness)
		{}
	};


	inline double evaluate(activation_function const& fn, double const x)
	{
		auto const sx = fn.steepness * x;
		switch(fn.type)
		{
			case ActivationFnType::SigmoidSymmetric:		return std::tanh(sx);//2.0 / (1.0 + std::exp(-2.0 * sx)) - 1.0;
			case ActivationFnType::Sigmoid:					return 1.0 / (1.0 + std::exp(-2.0 * sx));
			case ActivationFnType::Linear:					return sx;
			case ActivationFnType::LinearBounded:			return sx < 0.0 ? 0.0 : (sx > 1.0 ? 1.0 : sx);
			case ActivationFnType::LinearBoundedSymmetric:	return sx < -1.0 ? -1.0 : (sx > 1.0 ? 1.0 : sx);
			case ActivationFnType::Threshold:				return x > 0 ? 1.0 : 0.0;
			case ActivationFnType::ThresholdSymmetric:		return x > 0 ? 1.0 : -1.0;
			case ActivationFnType::Sine:					return std::sin(sx);
			case ActivationFnType::Cosine:					return std::cos(sx);
			case ActivationFnType::Gaussian:				return std::exp(-sx * sx);
			case ActivationFnType::GaussianSymmetric:		return 2.0 * std::exp(-sx * sx) - 1.0;
		}
	}

	inline activation_range evaluate(activation_function const& fn, activation_range::interval const& itv)
	{
		auto const sx = itv * fn.steepness;
		switch(fn.type)
		{
			case ActivationFnType::SigmoidSymmetric:		return activation_range{ bint::tanh(sx) };
			case ActivationFnType::Sigmoid:					return activation_range{ 1.0 / (1.0 + bint::exp(-2.0 * sx)) };
			case ActivationFnType::Linear:					return activation_range{ sx };
			case ActivationFnType::LinearBounded:
			return activation_range::from_interval(std::max(sx.lower(), 0.0), std::min(sx.upper(), 1.0));
			case ActivationFnType::LinearBoundedSymmetric:
			return activation_range::from_interval(std::max(sx.lower(), -1.0), std::min(sx.upper(), 1.0));
			case ActivationFnType::Threshold:
			return activation_range::from_set({ itv.lower() <= 0 ? 0.0 : 1.0, itv.upper() > 0 ? 1.0 : 0.0 });
			case ActivationFnType::ThresholdSymmetric:
			return activation_range::from_set({ itv.lower() <= 0 ? -1.0 : 1.0, itv.upper() > 0 ? 1.0 : -1.0 });
			case ActivationFnType::Sine:					return activation_range{ bint::sin(sx) };
			case ActivationFnType::Cosine:					return activation_range{ bint::cos(sx) };
			case ActivationFnType::Gaussian:				return activation_range{ bint::exp(-bint::square(sx)) };
			case ActivationFnType::GaussianSymmetric:		return activation_range{ 2.0 * bint::exp(-bint::square(sx)) - 1.0 };
		}
	}

	inline activation_range evaluate(activation_function const& fn, activation_range const& rg)
	{
		activation_range result;
		for(auto const& itv : rg)
		{
			if(bint::singleton(itv))
			{
				result |= evaluate(fn, itv.lower());
			}
			else
			{
				result |= evaluate(fn, itv);
			}
		}
		return result;
	}

	template < typename Derived >
	inline Eigen::ArrayXd evaluate(activation_function const& fn, Eigen::ArrayBase< Derived > const& v)
	{
		switch(fn.type)
		{
			case ActivationFnType::SigmoidSymmetric:
			return ((v * (-2.0 * fn.steepness)).exp() + 1.0).cwiseInverse() * 2.0 - 1.0;

			case ActivationFnType::Sigmoid:
			return ((v * (-2.0 * fn.steepness)).exp() + 1.0).cwiseInverse();

			case ActivationFnType::Linear:
			return v * fn.steepness;
			
			case ActivationFnType::LinearBounded:
			return (v * fn.steepness).cwiseMax(0.0).cwiseMin(1.0);

			case ActivationFnType::LinearBoundedSymmetric:
			return (v * fn.steepness).cwiseMax(-1.0).cwiseMin(1.0);
			
			case ActivationFnType::Threshold:
			{
				// TODO: Can this be done using an ArrayBase component-wise method?
				auto count = v.size();
				Eigen::ArrayXd result(count);
				for(size_t i = 0; i < count; ++i)
				{
					result[i] = v[i] > 0 ? 1.0 : 0.0;
				}
				return result;
			}
			
			case ActivationFnType::ThresholdSymmetric:
			{
				// TODO: Can this be done using an ArrayBase component-wise method?
				auto count = v.size();
				Eigen::ArrayXd result(count);
				for(size_t i = 0; i < count; ++i)
				{
					result[i] = v[i] > 0 ? 1.0 : -1.0;
				}
				return result;
			}
			
			case ActivationFnType::Sine:
			return (v * fn.steepness).sin();
			
			case ActivationFnType::Cosine:
			return (v * fn.steepness).cos();
			
			case ActivationFnType::Gaussian:
			return (-(v * fn.steepness).square()).exp();
			
			case ActivationFnType::GaussianSymmetric:
			return (-(v * fn.steepness).square()).exp() * 2.0 - 1.0;
		}
	}

	// Allows evaluation when x could be -/+ infinity.
	// Throws if fn is a periodic function.
	inline double evaluate_safe(activation_function const& fn, double const x)
	{
		if(x == activation_range::neg_infinity())
		{
			return *as_x_tends_to_negative_inf(fn.type);
		}
		else if(x == activation_range::pos_infinity())
		{
			return *as_x_tends_to_positive_inf(fn.type);
		}
		else
		{
			return evaluate(fn, x);
		}
	}

	// NOTE: This is making assumptions about the collection of functions we have
	inline activation_range get_function_range(ActivationFnType fn)
	{
		return activation_range{
			/*			is_bounded_below(fn) ? (is_symmetric(fn) ? -1.0 : 0.0) : activation_range::neg_infinity(),
			is_bounded_above(fn) ? 1.0 : activation_range::pos_infinity(),
			is_binary(fn)
			*/
			evaluate(activation_function{ fn, 1.0 }, activation_range::interval::whole())
		};
	}

	inline activation_range get_function_range(activation_function const& fn, activation_range const& input_range)
	{
		return evaluate(fn, input_range);

		/*
		auto const binary_output = is_binary(fn.type) || input_range.is_binary();

		if(is_increasing(fn.type) || input_range.is_binary())
		{
			// If function is increasing or input range is binary, just need to evaluate min and max x
			return activation_range{
				evaluate_safe(fn, input_range.lower_bound),
				evaluate_safe(fn, input_range.upper_bound),
				binary_output
			};
		}
		else if(!is_periodic(fn.type))
		{
			// Not increasing but not periodic
			auto const start = evaluate_safe(fn, input_range.lower_bound);
			auto const end = evaluate_safe(fn, input_range.upper_bound);
			switch(fn.type)
			{
				case ActivationFnType::Gaussian:
				case ActivationFnType::GaussianSymmetric:
				if(input_range.contains(0.0))
				{
					return activation_range{ std::min(start, end), 1.0, binary_output };
				}
				else
				{
					return activation_range{ start, end, binary_output };
				}

				default:
				throw std::exception("Fix this");
			}
		}
		else if(!input_range.is_bounded() || input_range.span() >= function_period(fn.type))
		{
			// Period function with input unbounded or wider than period leads to entire function range
			return get_function_range(fn.type);
		}
		else
		{
			// Periodic function, bounded input range with span less than period of function
			auto const period = function_period(fn.type);
			auto canonical_x1 = fmod(input_range.lower_bound, period);
			if(canonical_x1 < 0.0)
			{
				canonical_x1 += period;
			}
			auto canonical_x2 = fmod(input_range.upper_bound, period);
			if(canonical_x2 < 0.0)
			{
				canonical_x2 += period;
			}
			if(canonical_x1 > canonical_x2)
			{
				std::swap(canonical_x1, canonical_x2);
			}

			std::vector< double > minima, maxima;
			switch(fn.type)
			{
				case ActivationFnType::Sine:
				// Maximum at Pi/2, Minimum at 3Pi/2
				minima.emplace_back(3.0 * M_PI / 2);
				maxima.emplace_back(M_PI / 2);
				break;

				case ActivationFnType::Cosine:
				// Maximum at 0, Minimum at Pi
				minima.emplace_back(M_PI);
				maxima.emplace_back(0.0);
				break;

				default:
				throw std::exception("Fix this");
			}

			// Not the fastest, but can deal with unlikely case of multiple maxima or minima within a period
			auto const y1 = evaluate(fn, canonical_x1);
			auto const y2 = evaluate(fn, canonical_x2);
			auto y_min = std::min(y1, y2);
			auto y_max = std::max(y1, y2);

			for(auto const& minimum : minima)
			{
				auto const y = evaluate(fn, minimum);
				y_min = std::min(y_min, y);
			}

			for(auto const& maximum : maxima)
			{
				auto const y = evaluate(fn, maximum);
				y_max = std::max(y_max, y);
			}

			return activation_range(y_min, y_max, binary_output);
		}
		*/
	}

}


#endif


