// rtp_phys_controller.h

#ifndef __NE_RTP_PHYS_CONTROLLER_H
#define __NE_RTP_PHYS_CONTROLLER_H

#include "../rtp_agent.h"
#include "rtp_phys_system.h"
#include "../../params/enum_par.h"

#include <string>


namespace rtp_phys
{
	class i_phys_controller: public i_agent
	{
	public:
		enum Type {
			Passive,
			Interactive,

			Count,
		};

		static std::string const Names[Type::Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		static rtp_param_type* params(Type type);
		static i_phys_controller* create_instance(Type type, rtp_param param);

	public:
		virtual void update(phys_system::state& st, phys_system::scenario_data sdata) = 0;
	};
}


#endif

