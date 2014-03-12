// rtp_phys_composite_observer.h

#ifndef __NE_RTP_PHYS_COMPOSITE_OBSERVER_H
#define __NE_RTP_PHYS_COMPOSITE_OBSERVER_H

#include "rtp_phys_observers.h"
#include "rtp_phys_objectives.h"


namespace rtp_phys
{
	class composite_observer: public i_phys_observer
	{
	public:
		static i_phys_observer* create_instance(std::set< agent_objective::Type > const& required_observations);

	public:
		virtual void reset();
		virtual void update(/*decision,*/ state_t const& st);
		virtual i_observer::observations_t record_observations(trial_data_t const& td);

	protected:
		std::map< std::string, i_phys_observer* > m_single_value_observers;
	};
}


#endif

