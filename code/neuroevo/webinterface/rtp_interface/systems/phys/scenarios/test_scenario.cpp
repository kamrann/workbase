// test_scenario.cpp

#include "test_scenario.h"


namespace rtp_phys {

	rtp_named_param_list test_scenario::params()
	{
		rtp_named_param_list p;
		return p;
	}

	test_scenario::test_scenario(rtp_param param)
	{

	}

	phys_system::state test_scenario::generate_initial_state(rgen_t& rgen) const
	{
		phys_system::state st;

		// TODO:
		
		return st;
	}
}


