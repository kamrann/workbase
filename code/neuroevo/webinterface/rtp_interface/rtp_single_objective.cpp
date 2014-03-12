// rtp_single_objective.cpp

#include "rtp_single_objective.h"


rtp_single_objective::rtp_single_objective(std::string const& obj_id): m_obj_id(obj_id)
{
	
}

i_population_wide_observer::eval_data_t rtp_single_objective::initialize(size_t pop_size) const
{
	return eval_data_t(eval_data(pop_size);
}

void rtp_single_objective::register_datapoint(population_observations_t const& observations, eval_data_t& edata) const
{
	eval_data& ed = boost::any_cast<eval_data&>(edata);
	size_t const pop_size = ed.sums.size();
	for(size_t i = 0; i < pop_size; ++i)
	{
		ed.sums[i] += boost::any_cast< double >(observations[i].at(m_obj_id));
	}
	++ed.num_datapoints;
}

std::vector< boost::any > rtp_single_objective::evaluate(eval_data_t const& edata) const
{
	eval_data const& ed = boost::any_cast<eval_data const&>(edata);
	size_t const pop_size = ed.sums.size();

	std::vector< boost::any > results(pop_size);
	for(size_t i = 0; i < pop_size; ++i)
	{
		results[i] = ed.sums[i] / ed.num_datapoints;
	}
	return results;
}



