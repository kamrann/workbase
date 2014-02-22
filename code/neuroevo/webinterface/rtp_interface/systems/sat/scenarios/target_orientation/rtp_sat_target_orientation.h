// rtp_sat_target_orientation.h

#ifndef __NE_RTP_SAT_TARGET_ORIENTATION_H
#define __NE_RTP_SAT_TARGET_ORIENTATION_H

#include "../../rtp_sat_scenario.h"


namespace rtp_sat
{
	template < WorldDimensionality dim >
	class target_orientation: public sat_scenario< dim >
	{
	public:
		using sat_scenario< dim >::state_t;
		using sat_scenario< dim >::scenario_data;

		enum Params {
			Target,

			NumParams,
		};

		static rtp_named_param_list params();

		target_orientation(rtp_param param);

	public:
		virtual state_t generate_initial_state(rgen_t& rgen);
		virtual scenario_data_t get_scenario_data() const;

	private:
		double m_target;
	};
}


#endif

