// rtp_pareto.h

#ifndef __NE_RTP_PARETO_H
#define __NE_RTP_PARETO_H

#include "rtp_pop_wide_observer.h"

#include "util/pareto.h"

#include <string>
#include <vector>


namespace rtp {

	class rtp_pareto: public i_population_wide_observer
	{
	public:
		rtp_pareto(std::vector< std::string > const& components);

	public:
		virtual eval_data_t initialize(size_t pop_size) const;
		virtual void register_datapoint(population_observations_t const& observations, eval_data_t& edata) const;
		virtual std::vector< boost::any > evaluate(eval_data_t const& edata, boost::optional< std::string >& analysis) const;
		//	virtual std::string get_analysis(eval_data_t const& edata) const;

	protected:
		std::vector< std::string > m_components;

		//	typedef std::vector< double > obj_components;
		typedef std::vector< pareto< double > > idv_datapoints;
		typedef std::vector< idv_datapoints > eval_data;
	};

}


#endif


