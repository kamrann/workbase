// passive.h

#ifndef __NE_RTP_SAT_PASSIVE_CONRTOLLER_H
#define __NE_RTP_SAT_PASSIVE_CONRTOLLER_H

#include "../rtp_sat_controller.h"
#include "../rtp_sat_system.h"
#include "../../../params/enum_par.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class passive_agent: public i_sat_controller
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);
	};
}


#endif


