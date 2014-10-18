// agent_defn.h

#ifndef __NE_SYSSIM_AGENT_DEFN_H
#define __NE_SYSSIM_AGENT_DEFN_H

#include "system_sim_fwd.h"
#include "system_state_values.h"

#include "params/param_fwd.h"

#include <string>
#include <vector>


namespace sys {

	class i_agent_defn
	{
	public:
		virtual std::string get_name() const = 0;
		virtual std::string update_schema_providor_for_spec(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;
		virtual std::string update_schema_providor_for_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;

		virtual state_value_id_list get_agent_state_values(prm::param_accessor acc) const = 0;
		virtual std::vector< std::string > sensor_inputs(prm::param_accessor acc) const = 0;
		virtual size_t num_effectors(prm::param_accessor acc) const = 0;

		virtual agent_ptr create_agent(prm::param_accessor spec_acc, prm::param_accessor inst_acc) const = 0;

	public:
		virtual ~i_agent_defn() noexcept
		{}
	};

}


#endif

