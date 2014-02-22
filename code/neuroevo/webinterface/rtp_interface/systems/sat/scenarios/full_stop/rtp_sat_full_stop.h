// rtp_sat_full_stop.h

#ifndef __NE_RTP_SAT_FULL_STOP_H
#define __NE_RTP_SAT_FULL_STOP_H

#include "../../rtp_sat_scenario.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class full_stop: public sat_scenario< dim >
	{
	public:
		using sat_scenario< dim >::state_t;

		enum Params {
			NumParams,
		};

		static rtp_named_param_list params();

		full_stop(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen);
	};
}


#endif

