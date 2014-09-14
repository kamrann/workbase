// rtp_single_objective.cpp

#include "rtp_single_objective.h"

#include <sstream>


namespace rtp {

	rtp_single_objective::rtp_single_objective(std::string const& obj_id, MergeMethod merge):
		m_obj_id(obj_id),
		m_merge(merge)
	{

	}

	i_population_wide_observer::eval_data_t rtp_single_objective::initialize(size_t pop_size) const
	{
		double initial = 0.0;
		switch(m_merge)
		{
			case MergeMethod::Average:
			initial = 0.0;
			break;

			case MergeMethod::Minimum:
			initial = std::numeric_limits< double >::max();
			break;
		}

		return eval_data_t(eval_data(pop_size, initial));
	}

	void rtp_single_objective::register_datapoint(population_observations_t const& observations, eval_data_t& edata) const
	{
		eval_data& ed = boost::any_cast<eval_data&>(edata);
		size_t const pop_size = ed.values.size();
		for(size_t i = 0; i < pop_size; ++i)
		{
			auto value = boost::any_cast<double>(observations[i].at(m_obj_id));
			switch(m_merge)
			{
				case MergeMethod::Average:
				ed.values[i] += value;
				break;

				case MergeMethod::Minimum:
				ed.values[i] = std::min(ed.values[i], value);
				break;
			}
		}
		++ed.num_datapoints;
	}

	std::vector< boost::any > rtp_single_objective::evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const
	{
		eval_data const& ed = boost::any_cast<eval_data const&>(edata);
		size_t const pop_size = ed.values.size();

		std::vector< boost::any > results(pop_size);
		for(size_t i = 0; i < pop_size; ++i)
		{
			switch(m_merge)
			{
				case MergeMethod::Average:
				results[i] = ed.values[i] / ed.num_datapoints;
				break;

				case MergeMethod::Minimum:
				results[i] = ed.values[i];
				break;
			}
		}

		if(analysis)
		{
			double sum = 0.0;
			boost::optional< double > high;
			for(size_t i = 0; i < pop_size; ++i)
			{
				double val = boost::any_cast<double>(results[i]);
				sum += val;
				if(!high || val > *high)
				{
					high = val;
				}
			}

			std::stringstream ss;
			std::fixed(ss);
			ss.precision(2);
			ss << "Average [" << (sum / pop_size) << "]";
			ss << "; ";
			ss << "Highest [" << *high << "]";
			//ss << std::endl;
			analysis = ss.str();
		}

		return results;
	}
	/*
	std::string rtp_single_objective::get_analysis(eval_data_t const& edata) const
	{
	std::stringstream ss;
	std::fixed(ss);
	ss.precision(2);
	ss << "Average [";
	ss <<
	return ss.str();
	}
	*/

}


