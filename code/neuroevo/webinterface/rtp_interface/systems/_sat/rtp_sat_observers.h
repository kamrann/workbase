// rtp_sat_observers.h

#ifndef __NE_RTP_SAT_OBSERVERS_H
#define __NE_RTP_SAT_OBSERVERS_H

#include "rtp_sat_system.h"
#include "../../rtp_observer.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class i_sat_observer: public i_observer
	{
	public:
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::trial_data trial_data_t;

	public:
		virtual void update(/*decision,*/ state_t const& st) = 0;
		virtual i_observer::observations_t record_observations(trial_data_t const& td) = 0;
	};
}


#endif

