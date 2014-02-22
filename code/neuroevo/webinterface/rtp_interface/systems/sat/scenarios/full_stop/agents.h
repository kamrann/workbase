// agents.h

#ifndef __NE_RTP_SAT_FULL_STOP_AGENTS_H
#define __NE_RTP_SAT_FULL_STOP_AGENTS_H

#include "../../rtp_sat_system.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class passive_agent: public sat_system< dim >::i_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);
	};

	template < WorldDimensionality dim >
	class interactive_agent: public sat_system< dim >::i_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);

		interactive_agent();
		void register_input(size_t idx, bool activate);

	protected:
		// TODO:
		std::array< bool, 10 > input;
	};

	template < WorldDimensionality dim >
	class neural_net_agent: public sat_system< dim >::i_agent
	{
	public:
		typedef typename sat_system< dim >::decision decision_t;
		typedef typename sat_system< dim >::state state_t;
		typedef typename sat_system< dim >::scenario_data scenario_data_t;

	public:
		virtual decision_t make_decision(state_t const& st, scenario_data_t sdata);

	private:
		// nn
	};
}


#endif

