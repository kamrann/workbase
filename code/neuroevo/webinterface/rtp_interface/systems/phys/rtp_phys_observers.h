// rtp_phys_observers.h

#ifndef __NE_RTP_PHYS_OBSERVERS_H
#define __NE_RTP_PHYS_OBSERVERS_H

#include "rtp_phys_system.h"
#include "../../rtp_observer.h"


namespace rtp
{
	class i_phys_observer: public i_observer
	{
	public:
		typedef phys_system::state state_t;
		typedef phys_system::trial_data trial_data_t;

	public:
		virtual i_observer::observations_t record_observations(trial_data_t const& td) = 0;
	};
}


#endif

