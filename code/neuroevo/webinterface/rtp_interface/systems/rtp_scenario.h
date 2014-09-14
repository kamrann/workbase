// rtp_scenario.h

#ifndef __NE_RTP_SCENARIO_H
#define __NE_RTP_SCENARIO_H

#include "../rtp_defs.h"


namespace rtp {

	class i_decision;

	class i_scenario
	{
	public:
		/*	virtual boost::optional< agent_id_t > register_agent() = 0;
			virtual void generate_initial_state(rgen_t& rgen) = 0;
			virtual bool pending_decision(agent_id_t id) = 0;
			virtual void register_solution_decision(i_decision const& dec) = 0;
			virtual bool update() = 0;
			*/
	};

}


#endif

