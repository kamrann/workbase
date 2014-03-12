// interactive.h

#ifndef __NE_RTP_PHYS_INTERACTIVE_CONRTOLLER_H
#define __NE_RTP_PHYS_INTERACTIVE_CONRTOLLER_H

#include "../rtp_phys_controller.h"
#include "../rtp_phys_system.h"
#include "../../rtp_interactive_agent.h"
#include "../../rtp_interactive_input.h"
#include "../../../params/enum_par.h"

//
#include "../bodies/test_body.h"
//

#include <vector>


namespace rtp_phys
{
	class interactive_controller:
		public i_phys_controller,
		public i_interactive_agent
	{
	public:
		interactive_controller(): inputs(4, false)
		{}

		virtual void update(phys_system::state& st, phys_system::scenario_data sdata)
		{
			float torque = 0.0f;
			if(inputs[0])
			{
				torque = 300.0f;
			}
			else if(inputs[1])
			{
				torque = -300.0f;
			}
			else if(inputs[2])
			{
				torque = 100.0f;
			}
			else if(inputs[3])
			{
				torque = -100.0f;
			}
			test_body* tbody = (test_body*)st.body.get();
			tbody->fore->ApplyTorque(torque, true);
			tbody->rear->ApplyTorque(-torque, true);
		}

		virtual void register_input(interactive_input const& input)
		{
			if(input.key_index >= inputs.size())
			{
				inputs.resize(input.key_index + 1);
			}

			inputs[input.key_index] = input.down;
		}

	protected:
		std::vector< bool > inputs;
	};

}


#endif

