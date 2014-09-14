// space_based.h

#ifndef __NE_RTP_PHYS_SPACE_BASED_H
#define __NE_RTP_PHYS_SPACE_BASED_H

#include "../rtp_phys_scenario.h"


namespace rtp
{
	class space_based_scenario: public phys_scenario
	{
	public:
//		using phys_scenario::state_t;

		enum Params {

			Count,
		};

		space_based_scenario();

	public:
		virtual boost::any generate_initial_state(rgen_t& rgen) const;
		virtual void load_initial_state(boost::any const& data, b2World* world);

	protected:
		struct init_state_data
		{

		};
	};
}


#endif

