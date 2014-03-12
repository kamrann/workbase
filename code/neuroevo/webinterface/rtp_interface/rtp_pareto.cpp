// rtp_pareto.cpp

#include "rtp_pareto.h"

#include "util/pareto.h"

#include <algorithm>


rtp_pareto::rtp_pareto(std::vector< std::string > const& components): m_components(components)
{
	
}

i_population_wide_observer::eval_data_t rtp_pareto::initialize(size_t pop_size) const
{
	return eval_data_t(eval_data(pop_size);
}

void rtp_pareto::register_datapoint(population_observations_t const& observations, eval_data_t& edata) const
{
	eval_data& ed = boost::any_cast<eval_data&>(edata);
	size_t const pop_size = ed.sums.size();
	for(size_t i = 0; i < pop_size; ++i)
	{
		ed.sums[i] += boost::any_cast< double >(observations[i].at(m_obj_id));
	}
	++ed.num_datapoints;
}

std::vector< boost::any > rtp_pareto::evaluate(eval_data_t const& edata)
{
	size_t const pop_size = observations.size();
	size_t const num_comps = m_components.size();

	// TODO: allow other types than double?
	std::vector< pareto< double > > pareto_vals(pop_size);

	for(size_t i = 0; i < pop_size; ++i)
	{
		for(size_t c = 0; c < num_comps; ++c)
		{
			pareto_vals[i].push_component(boost::any_cast< double >(observations[i].at(m_components[c])));
		}
	}

	std::vector< size_t > ranks(pop_size);
	pareto< double >::calc_ranks(pareto_vals.begin(), pareto_vals.end(), ranks.begin());

	std::vector< boost::any > any_ranks(pop_size);
	std::transform(ranks.begin(), ranks.end(), any_ranks.begin(), [](size_t rank){
		return -(double)(rank);
	});
	return any_ranks;
}



