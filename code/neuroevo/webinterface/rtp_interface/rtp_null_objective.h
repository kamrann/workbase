// rtp_null_objective.h

#ifndef __NE_RTP_NULL_OBJECTIVE_H
#define __NE_RTP_NULL_OBJECTIVE_H

#include "rtp_pop_wide_observer.h"

#include <string>
#include <vector>


namespace rtp {

	// For performance testing purposes, does nothing, all objective values produced are zero.
	class rtp_null_objective: public i_population_wide_observer
	{
	public:
		rtp_null_objective();

	public:
		virtual eval_data_t initialize(size_t pop_size) const;
		virtual void register_datapoint(population_observations_t const& observations, eval_data_t& edata) const;
		virtual std::vector< boost::any > evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const;
	};

}


#endif


