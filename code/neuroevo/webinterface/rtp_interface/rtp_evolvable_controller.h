// rtp_evolvable_controller.h

#ifndef __NE_RTP_EVOLVABLE_CONTROLLER_H
#define __NE_RTP_EVOLVABLE_CONTROLLER_H

#include "wt_param_widgets/pw_fwd.h"

#include <string>
#include <tuple>


namespace YAML {
	class Node;
}

namespace rtp
{
	class i_genome_mapping;
	class i_controller_factory;

	class evolvable_controller
	{
	public:
		enum Type {
			MLP,

			Count,
			None = Count,
			Default = None,
		};

		static std::string const Names[Type::Count];

		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::tuple<
			std::unique_ptr< i_genome_mapping >,
			std::unique_ptr< i_controller_factory >
		> create_instance(prm::param_accessor param);
	};
}


#endif

