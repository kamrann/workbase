// system_defn.h

#ifndef __NE_SYSTEM_DEFN_H
#define __NE_SYSTEM_DEFN_H

#include "system_sim_fwd.h"
#include "system_state_values.h"

#include "params/param.h"
#include "params/param_fwd.h"

#include <string>
#include <vector>
#include <memory>


namespace sys {

	struct update_info;

	class i_system_defn
	{
	public:
		virtual std::string get_name() const = 0;

		virtual std::string update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;

		virtual void add_agent_defn(std::string role, agent_defn_ptr defn) = 0;
		virtual void add_controller_defn(std::string controller_class, controller_defn_ptr defn) = 0;

		virtual std::vector< std::string > get_agent_type_names() const = 0;
		virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const = 0;
		virtual size_t get_agent_num_effectors(prm::param agent_type) const = 0;

		virtual state_value_id_list get_system_state_values(prm::param_accessor param_vals) const = 0;

		virtual system_ptr create_system(prm::param_accessor acc) const = 0;

		// TODO: probably want to combine these two
		virtual bool is_instantaneous() const = 0;
		virtual update_info get_update_info() const = 0;

//		virtual system_drawer_ptr get_drawer() const = 0;

	public:
		virtual ~i_system_defn()
		{}
	};

}


#endif

