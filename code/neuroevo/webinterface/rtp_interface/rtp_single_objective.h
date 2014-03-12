// rtp_single_objective.h

#ifndef __NE_RTP_SINGLE_OBJECTIVE_H
#define __NE_RTP_SINGLE_OBJECTIVE_H

#include "rtp_pop_wide_observer.h"

#include <string>
#include <vector>


// TODO: May rearrange hierarchy a little, since this does not need population wide evaluation...
class rtp_single_objective: public i_population_wide_observer
{
public:
	rtp_single_objective(std::string const& obj_id);

public:
	virtual eval_data_t initialize(size_t pop_size) const;
	virtual void register_datapoint(population_observations_t const& observations, eval_data_t& edata) const;
	virtual std::vector< boost::any > evaluate(eval_data_t const& edata) const;

protected:
	std::string m_obj_id;

	struct eval_data
	{
		std::vector< double > sums;
		size_t num_datapoints;

		eval_data(size_t pop_size): sums(pop_size, 0.0), num_datapoints(0)
		{}
	};
};



#endif


