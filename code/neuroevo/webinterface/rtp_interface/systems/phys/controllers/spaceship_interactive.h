// spaceship_interactive.h

#ifndef __NE_RTP_PHYS_SPACESHIP_INTERACTIVE_CONRTOLLER_H
#define __NE_RTP_PHYS_SPACESHIP_INTERACTIVE_CONRTOLLER_H

#include "../../rtp_interactive_controller.h"
#include "../../rtp_interactive_input.h"

#include <vector>


namespace rtp
{
	class spaceship_interactive_controller//:
//		public i_interactive_controller
	{
	public:
		static std::string update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::unique_ptr< i_controller_factory > create_factory(prm::param_accessor param);

	public:
	};

}


#endif

