// rtp_pareto.cpp

#include "rtp_pareto.h"

#include "util/pareto.h"

#include <algorithm>
#include <sstream>


rtp_pareto::rtp_pareto(std::vector< std::string > const& components): m_components(components)
{
	
}

i_population_wide_observer::eval_data_t rtp_pareto::initialize(size_t pop_size) const
{
	return eval_data_t(eval_data(pop_size));
}

void rtp_pareto::register_datapoint(population_observations_t const& observations, eval_data_t& edata) const
{
	eval_data& ed = boost::any_cast<eval_data&>(edata);
	size_t const pop_size = ed.size();
	size_t const num_comps = m_components.size();
	for(size_t i = 0; i < pop_size; ++i)
	{
		pareto< double > pareto_val;
		for(size_t c = 0; c < num_comps; ++c)
		{
			pareto_val.push_component(boost::any_cast< double >(observations[i].at(m_components[c])));
		}
		ed[i].push_back(pareto_val);
	}
}

std::vector< boost::any > rtp_pareto::evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const
{
	eval_data const& ed = boost::any_cast<eval_data const&>(edata);
	size_t const pop_size = ed.size();
	size_t const num_datapoints = ed.front().size();
	size_t const num_comps = m_components.size();

	// TODO: For now using probably of being dominated to determine fitness.
	// Could use average rank (is this different?), create a single pareto for each idv by averaging their separate
	// objective components over the trials (feels like this is bad), or other methods - see http://www.info.univ-angers.fr/pub/basseur/papers/EMO_2007_2.pdf
	std::vector< size_t > dominated_counts(pop_size, 0);

	for(size_t i = 0; i < pop_size; ++i)
	{
		for(size_t d = 0; d < num_datapoints; ++d)
		{
			for(size_t j = i + 1; j < pop_size; ++j)
			{
				// TODO: Does it make sense here to compare every pair of pareto vals across trials also??
				if(ed[i][d].dominated_by(ed[j][d]))
				{
					++dominated_counts[i];
				}
				else if(ed[j][d].dominated_by(ed[i][d]))
				{
					++dominated_counts[j];
				}
			}
		}
	}

	size_t const MaxDominatedCount = (pop_size - 1) * num_datapoints;

	std::vector< boost::any > fitnesses(pop_size);
	std::transform(dominated_counts.begin(), dominated_counts.end(), fitnesses.begin(), [=](size_t count){
		return 1.0 - (double)count / MaxDominatedCount;
	});

	if(analysis)
	{
		std::vector< double > averages(num_comps, 0.0);
		for(size_t i = 0; i < pop_size; ++i)
		{
			for(size_t d = 0; d < num_datapoints; ++d)
			{
				for(size_t c = 0; c < num_comps; ++c)
				{
					double val = ed[i][d][c];
					averages[c] += val;
				}
			}
		}

		std::stringstream ss;
		std::fixed(ss);
		ss.precision(2);
		ss << "Average components [ ";
		for(size_t c = 0; c < num_comps; ++c)
		{
			if(c > 0)
			{
				ss << ", ";
			}
			ss << (averages[c] / pop_size);
		}
		ss << " ]; ";
		ss << "Non-dominated ";
		for(size_t i = 0; i < pop_size; ++i)
		{
			// TODO: How to deal with multiple trials?
			// CURRENTLY JUST USING FIRST TRIAL BELOW!!!!!!!!!!!!!!!!!!!!!!!!!
			// Potentially related to question in nested loops above
			if(dominated_counts[i] == 0)
			{
				ss << "[ ";
				for(size_t c = 0; c < num_comps; ++c)
				{
					if(c > 0)
					{
						ss << ", ";
					}
					ss << ed[i][0][c];
				}
				ss << " ] ";
			}
		}
		ss << std::endl;
		analysis = ss.str();
	}

	return fitnesses;
}



