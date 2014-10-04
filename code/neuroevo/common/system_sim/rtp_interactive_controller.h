// rtp_interactive_controller.h

#ifndef __NE_RTP_INTERACTIVE_CONTROLLER_H
#define __NE_RTP_INTERACTIVE_CONTROLLER_H

#include "rtp_controller.h"
#include "rtp_interactive_input.h"

#include "params/param_fwd.h"


namespace sys {

	class i_interactive_controller:
		public i_controller
	{
	public:
		static std::vector< std::pair< std::string, std::string > > get_available_bindings();
		static std::string update_direct_bindings_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
		static std::unique_ptr< i_controller_factory > create_factory(prm::param_accessor param_vals);

	public:
		virtual const interactive_input_set& get_required_interactive_inputs() const = 0;
		virtual void register_input(interactive_input const& input) = 0;
	};

}


#endif

