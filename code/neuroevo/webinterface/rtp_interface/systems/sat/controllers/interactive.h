// interactive.h

#ifndef __NE_RTP_SAT_INTERACTIVE_CONRTOLLER_H
#define __NE_RTP_SAT_INTERACTIVE_CONRTOLLER_H

#include "../rtp_sat_controller.h"
#include "../rtp_sat_system.h"
#include "../../rtp_interactive_agent.h"
#include "../../rtp_interactive_input.h"
#include "../../../params/enum_par.h"

#include <vector>


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class interactive_controller:
		public i_sat_controller,
		public i_interactive_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);

		interactive_controller();
		virtual void register_input(interactive_input const& input);

	protected:
		std::vector< bool > inputs;
	};
}


#endif

