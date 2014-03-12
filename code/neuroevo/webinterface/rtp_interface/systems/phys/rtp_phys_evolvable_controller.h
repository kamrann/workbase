// rtp_phys_evolvable_controller.h

#ifndef __NE_RTP_PHYS_EVOLVABLE_CONTROLLER_H
#define __NE_RTP_PHYS_EVOLVABLE_CONTROLLER_H

#include "../../params/enum_par.h"

#include <string>
#include <tuple>


class i_genome_mapping;
class i_agent_factory;

namespace rtp_phys
{
	class evolvable_controller
	{
	public:
		enum Type {
			MLP,

			Count,
		};

		static std::string const Names[Type::Count];

		class enum_param_type: public rtp_enum_param_type
		{
		public:
			enum_param_type();
		};

		static rtp_param_type* params(Type ea_type);
		static std::tuple< i_genome_mapping*, i_agent_factory* > create_instance(Type type, rtp_param param);
	};
}


#endif

