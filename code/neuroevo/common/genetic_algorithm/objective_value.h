// objective_value.h
/*
Represents a domain specific assessment of an individual.
The fitness value as used internally to the genetic algorithm is derived from this value.
*/

#ifndef __WB_GA_OBJECTIVE_VALUE_H
#define __WB_GA_OBJECTIVE_VALUE_H

#include "util/pareto.h"

#include <yaml-cpp/yaml.h>

#include <boost/variant.hpp>


namespace ga {

	class objective_value
	{
	public:
		enum class type {
			Real,
			Pareto,
		};

	protected:
		// TODO: May eventually want to support other representations, such as integer, pareto< integer >, or
		// pareto of multiple different type. For now, just double and pareto< double >.
		typedef boost::variant <
			double,
			pareto< double >
		> variant_t;

	public:
		objective_value();
		objective_value(double val);
		objective_value(pareto< double > val);

	public:
		inline double as_real() const
		{
			return boost::get< double >(_value);
		}

		inline pareto< double > const& as_pareto() const
		{
			return boost::get< pareto< double > >(_value);
		}

		std::string to_string() const;
		YAML::Node to_yaml() const;

	protected:
		variant_t _value;
	};

}



#endif


