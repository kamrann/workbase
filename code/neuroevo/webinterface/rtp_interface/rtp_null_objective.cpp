// rtp_null_objective.cpp

#include "rtp_null_objective.h"


namespace rtp {

	rtp_null_objective::rtp_null_objective()
	{

	}

	i_population_wide_observer::eval_data_t rtp_null_objective::initialize(size_t pop_size) const
	{
		return eval_data_t{ pop_size };
	}

	void rtp_null_objective::register_datapoint(population_observations_t const& observations, eval_data_t& edata) const
	{

	}

	std::vector< boost::any > rtp_null_objective::evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const
	{
		size_t pop_size = boost::any_cast<size_t>(edata);
		return std::vector< boost::any >(pop_size, boost::any{ 0.0 });
	}

}


