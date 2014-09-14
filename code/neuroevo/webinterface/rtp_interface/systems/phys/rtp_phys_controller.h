// rtp_phys_controller.h

#ifndef __NE_RTP_PHYS_CONTROLLER_H
#define __NE_RTP_PHYS_CONTROLLER_H

#include "../rtp_controller.h"

#include "wt_param_widgets/pw_fwd.h"

#include <string>


namespace rtp
{
	class i_phys_controller: public i_controller
	{
	public:
		enum Type {
			Evolved,

			Passive,
			Interactive,
			Database,

			Count,
			None = Count,
			Default = None,
		};

		static std::string const Names[Type::Count];

		static prm::param get_controller_agent_type(prm::param_accessor param);
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, bool evolvable);
		static std::unique_ptr< i_controller_factory > create_factory(prm::param_accessor param);
		static i_phys_controller* create_instance(prm::param_accessor param);
		static i_phys_controller* create_instance(prm::param& param);
	};
}


#include <yaml-cpp/yaml.h>

namespace YAML {

	template <>
	struct convert < rtp::i_phys_controller::Type >
	{
		static Node encode(rtp::i_phys_controller::Type const& rhs)
		{
			return Node(rtp::i_phys_controller::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::i_phys_controller::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp::i_phys_controller::Type > const mapping_;
	};
}


#endif

