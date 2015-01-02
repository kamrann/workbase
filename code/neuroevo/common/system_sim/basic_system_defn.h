// basic_system_defn.h

#ifndef __NE_BASIC_SYSTEM_DEFN_H
#define __NE_BASIC_SYSTEM_DEFN_H

#include "system_defn.h"

#include <list>
#include <map>
#include <memory>


namespace sys {

	class basic_system_defn:
		public i_system_defn
	{
	public:
		virtual void add_agent_defn(std::string role, agent_defn_ptr defn) override;
		virtual void add_controller_defn(std::string controller_class, controller_defn_ptr defn) override;

		virtual ddl::defn_node get_defn(ddl::specifier& spc) override;

		virtual state_value_id_list get_system_state_values(ddl::navigator nav) const override;
		virtual ddl::dep_function< state_value_id_list >
			get_system_state_values_fn() const override;

		virtual std::vector< std::string > get_agent_type_names() const override;
//		virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const override;

		//virtual size_t get_agent_num_effectors(ddl::navigator spec_nav) const override;
		virtual ddl::dep_function< size_t > get_inst_num_effectors_fn() const override;

		virtual ddl::dep_function< std::vector< ddl::navigator > >
			get_connected_spec_navs_fn(std::string controller_cls, std::string controller_type) const override;

	protected:
		std::vector< std::pair< int, std::string > > get_available_agent_specs(ddl::navigator) const;
		ddl::navigator get_agent_instance_spec_nav(std::string spec_name, ddl::navigator nav) const;
		std::string get_agent_role_name(ddl::navigator spec_nav) const;
		std::string get_agent_type_name(ddl::navigator spec_nav) const;
		boost::optional< std::string > get_agent_instance_spec_name(ddl::navigator nav) const;
		
		i_agent_defn* get_agent_defn(std::string const& role, std::string const& type) const;
		i_controller_defn* get_controller_defn(std::string const& cls, std::string const& type) const;

		virtual ddl::defn_node get_system_core_defn(ddl::specifier& spc) = 0;

		ddl::defn_node get_agent_specification_list_defn(ddl::specifier&);
		ddl::defn_node get_agent_instance_list_defn(ddl::specifier&);
		ddl::defn_node get_agent_specification_defn(ddl::specifier&);
		ddl::defn_node get_agent_instance_defn(ddl::specifier&);
		ddl::defn_node get_agent_controller_defn(ddl::specifier&);

		virtual //state_value_id_list
			ddl::dep_function< state_value_id_list >
			get_system_core_state_values_fn() const = 0;

		//controller_ptr create_controller(prm::param_accessor acc) const;

		virtual system_ptr create_system_core(ddl::navigator nav) const = 0;
		virtual system_ptr create_system(ddl::navigator nav) const override;

	protected:
		// TODO: Use of shared pointer necessary only due to bug in Nov CTP
		typedef std::list< agent_defn_ptr > agent_defn_list_t;
		typedef std::map< std::string, std::shared_ptr< agent_defn_list_t > > agent_defn_map_t;

		typedef std::list< controller_defn_ptr > controller_defn_list_t;
		typedef std::map< std::string, std::shared_ptr< controller_defn_list_t > > controller_defn_map_t;

		agent_defn_map_t m_agent_defns;
		controller_defn_map_t m_controller_defns;

		/////////////
//		ddl::defn_node dn_spec_list_;
//		ddl::defn_node dn_inst_list_;
		ddl::defn_node spec_name_dn_;
		ddl::defn_node inst_spec_ref_dn_;

		ddl::dep_function< std::vector< std::string > > get_all_spec_names_fn_;			// ctx: anywhere
		ddl::dep_function< boost::optional< std::string > > get_spec_role_fn_;			// ctx: within spec
		ddl::dep_function< boost::optional< std::string > > get_spec_type_fn_;			// ctx: within spec
		ddl::dep_function< boost::optional< std::string > > get_inst_spec_ref_fn_;		// ctx: within instance
		ddl::dep_function< ddl::navigator > get_inst_spec_nav_fn_;						// ctx: within instance
		ddl::dep_function< state_value_id_list > get_system_state_values_fn_;			// ctx: anywhere
		ddl::dep_function< boost::optional< std::string > > get_controller_class_fn_;	// ctx: within instance
		ddl::dep_function< boost::optional< std::string > > get_controller_type_fn_;	// ctx: within instance
		ddl::dep_function< size_t > get_inst_num_effectors_fn_;							// ctx: within instance
	
		std::function< std::vector< ddl::navigator >(std::string, std::string, ddl::navigator) > get_connected_spec_navs_fn_impl_;
		// TODO: This is just a helper with the dependencies registered, can't be used directly
		ddl::dep_function< std::vector< ddl::navigator > > get_connected_spec_navs_fn_;		// ctx: anywhere
	};

}


#endif

