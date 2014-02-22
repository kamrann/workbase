// agents.cpp

#include "agents.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	typename passive_agent< dim >::decision_t passive_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;
		// TODO: num thrusters??
		dec.resize(4, false);
		//
		return dec;
	}

	template < WorldDimensionality dim >
	interactive_agent< dim >::interactive_agent(): input()
	{}

	template < WorldDimensionality dim >
	typename interactive_agent< dim >::decision_t interactive_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;
		// TODO: num thrusters??
		dec.resize(4);
		for(size_t i = 0; i < 4; ++i)
		{
			dec[i] = input[i];
		}
		return dec;
	}

	template < WorldDimensionality dim >
	void interactive_agent< dim >::register_input(size_t idx, bool activate)
	{
		input[idx] = activate;
	}

	template < WorldDimensionality dim >
	typename neural_net_agent< dim >::decision_t neural_net_agent< dim >::make_decision(state_t const& st, scenario_data_t sdata)
	{
		decision_t dec;
		// TODO: num thrusters??
		dec.resize(4, false);
		//
		return dec;
	}
}


