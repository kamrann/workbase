// system_defn.h

#ifndef __NE_SYSTEM_DEFN_H
#define __NE_SYSTEM_DEFN_H

#include "system_sim_fwd.h"

#include "params/param.h"
#include "params/param_fwd.h"

#include <string>
#include <vector>
#include <memory>


namespace sys {

//	struct agents_data;
	struct update_info;

	class i_controller_defn;

	class i_system_defn
	{
	public:
		virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;
		//static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

		virtual void add_controller_defn(std::string controller_class, std::unique_ptr< i_controller_defn > defn) = 0;

		virtual std::vector< std::string > get_agent_type_names() const = 0;
		virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const = 0;
		virtual size_t get_agent_num_effectors(prm::param agent_type) const = 0;
		virtual std::vector< std::string > get_system_state_values(prm::param_accessor param_vals) const = 0;

		virtual system_ptr create_system(prm::param_accessor acc) const = 0;

		// TODO: probably want to combine these two
		virtual bool is_instantaneous() const = 0;
		virtual update_info get_update_info() const = 0;

		// ?? virtual std::shared_ptr< i_properties const > get_state_properties() const = 0;

	public:
		virtual ~i_system_defn()
		{}
	};

}


#endif

