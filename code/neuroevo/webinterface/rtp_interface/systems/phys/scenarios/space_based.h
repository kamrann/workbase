// space_based.h

#ifndef __NE_RTP_PHYS_SPACE_BASED_H
#define __NE_RTP_PHYS_SPACE_BASED_H

#include "../rtp_phys_scenario.h"


namespace rtp_phys
{
	class space_based_scenario: public phys_scenario
	{
	public:
//		using phys_scenario::state_t;

		enum Params {

			Count,
		};

		static rtp_named_param_list params();

		space_based_scenario(rtp_param param);

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void load_initial_state(boost::any const& data, phys_system::state& st) const;

	protected:
		struct init_state_data
		{

		};
	};
}


#endif

