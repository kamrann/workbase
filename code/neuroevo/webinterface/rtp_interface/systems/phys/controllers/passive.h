// passive.h

#ifndef __NE_RTP_PHYS_PASSIVE_CONRTOLLER_H
#define __NE_RTP_PHYS_PASSIVE_CONRTOLLER_H

#include "../rtp_phys_controller.h"
#include "../rtp_phys_system.h"
#include "../../../params/enum_par.h"


namespace rtp_phys
{
	class passive_controller: public i_phys_controller
	{
	public:
		virtual void update(phys_system::state& st, phys_system::scenario_data sdata)
		{}
	};

}


#endif

