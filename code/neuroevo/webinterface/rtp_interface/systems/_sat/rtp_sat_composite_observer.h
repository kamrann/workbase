// rtp_sat_composite_observer.h

#ifndef __NE_RTP_SAT_COMPOSITE_OBSERVER_H
#define __NE_RTP_SAT_COMPOSITE_OBSERVER_H

#include "rtp_sat_observers.h"
#include "rtp_sat_objectives.h"


namespace rtp_sat
{
	class composite_observer: public i_sat_observer
	{
	public:
		static i_sat_observer* create_instance(std::set< agent_objective::Type > const& required_observations);

	public:
		virtual void reset();
		virtual void update(/*decision,*/ state_t const& st);
		virtual i_observer::observations_t record_observations(trial_data_t const& td);

	protected:
		std::map< std::string, i_sat_observer* > m_single_value_observers;
	};
}


#endif

