// test_scenario.h

#ifndef __NE_RTP_PHYS_TEST_SCENARIO_H
#define __NE_RTP_PHYS_TEST_SCENARIO_H

#include "../rtp_phys_scenario.h"


namespace rtp_phys
{
	class test_scenario: public phys_scenario
	{
	public:
		using phys_scenario::state_t;

		enum Params {
			NumParams,
		};

		static rtp_named_param_list params();

		test_scenario(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen) const;
	};
}


#endif

