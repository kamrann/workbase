// rtp_pop_wide_observer.h

#ifndef __NE_RTP_POPULATION_WIDE_OBSERVER_H
#define __NE_RTP_POPULATION_WIDE_OBSERVER_H

#include "rtp_observer.h"
#include "params/enum_par.h"

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <vector>


namespace YAML {
	class Node;
}

class i_population_wide_observer
{
public:
	enum Type {
		Single,
		Pareto,

		Count,
		Default = Single,
	};

	static std::string const Names[Count];

	class enum_param_type: public rtp_enum_param_type
	{
	public:
		enum_param_type();
	};

	static i_population_wide_observer* create_instance(Type type, rtp_param param);

public:
	typedef std::vector< i_observer::observations_t > population_observations_t;
	typedef boost::any eval_data_t;

public:
	virtual eval_data_t initialize(size_t pop_size) const = 0;
	virtual void register_datapoint(population_observations_t const& observations, eval_data_t& edata) const = 0;
	virtual std::vector< boost::any > evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const = 0;
//	virtual std::string get_analysis(eval_data_t const& edata) const = 0;
};



#endif


