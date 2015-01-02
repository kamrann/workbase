// system_defn.h

#ifndef __NE_SYSTEM_DEFN_H
#define __NE_SYSTEM_DEFN_H

#include "system_sim_fwd.h"
#include "system_state_values.h"

#include "ddl/ddl.h"

#include <string>
#include <vector>
#include <memory>


namespace sys {

	struct update_info;

	class i_system_defn
	{
	public:
		virtual std::string get_name() const = 0;

		virtual ddl::defn_node get_defn(ddl::specifier& spc) = 0;

		virtual void add_agent_defn(std::string role, agent_defn_ptr defn) = 0;
		virtual void add_controller_defn(std::string controller_class, controller_defn_ptr defn) = 0;

		virtual std::vector< std::string > get_agent_type_names() const = 0;
//		virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const = 0;

//		virtual size_t get_agent_num_effectors(ddl::navigator spec_nav) const = 0;
		virtual ddl::dep_function< size_t > get_inst_num_effectors_fn() const = 0;	// from instance nav

		virtual state_value_id_list get_system_state_values(ddl::navigator nav) const = 0;
		virtual ddl::dep_function< state_value_id_list >
			get_system_state_values_fn() const = 0;

		// TODO: virtual bool is_defined(prm::param_accessor acc) const = 0;
		virtual system_ptr create_system(ddl::navigator nav) const = 0;

		// TODO: probably want to combine these two
		virtual bool is_instantaneous() const = 0;
		virtual update_info get_update_info() const = 0;

//		virtual system_drawer_ptr get_drawer() const = 0;

		////////////// TODO: This is dodgy since parameters are really referring to basic_system_defn.
		// Thing is, for evolved system, external access to agent/controller pairings is needed...
		// Current impl: returns list of paths to each agent spec which has 1 or more instances connected
		// with the given controller.
//		virtual void get_connected_agent_specs(std::string controller_cls, std::string controller_type, ddl::navigator nav) const = 0;
		virtual ddl::dep_function< std::vector< ddl::navigator > >
			get_connected_spec_navs_fn(std::string controller_cls, std::string controller_type) const = 0;
		//////////////

	public:
		virtual ~i_system_defn()
		{}
	};

}


#endif

