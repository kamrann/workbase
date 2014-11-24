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

		virtual std::string update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

		virtual state_value_id_list get_system_state_values(prm::param_accessor acc) const override;
			
		virtual std::vector< std::string > get_agent_type_names() const override;
		virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const override;
		virtual size_t get_agent_num_effectors(prm::param_accessor spec_acc) const override;

		virtual std::vector< prm::qualified_path > get_connected_agent_specs(std::string controller_cls, std::string controller_type, prm::param_accessor acc) const override;

	protected:
		static std::vector< std::pair< prm::qualified_path, std::string > > get_available_agent_specs(prm::param_accessor acc);
		static prm::qualified_path get_agent_instance_spec_path(std::string spec_name, prm::param_accessor acc);
		static std::string get_agent_role_name(prm::param_accessor spec_acc);
		static std::string get_agent_type_name(prm::param_accessor spec_acc);
		static boost::optional< std::string > get_agent_instance_spec_name(prm::param_accessor acc);
		
		i_agent_defn* get_agent_defn(std::string const& role, std::string const& type) const;
		i_controller_defn* get_controller_defn(std::string const& cls, std::string const& type) const;

		virtual std::string update_schema_provider_for_system_core(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const = 0;

		std::string update_schema_provider_for_agent_specification_list(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;
		std::string update_schema_provider_for_agent_instance_list(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;
		std::string update_schema_provider_for_agent_specification(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;
		std::string update_schema_provider_for_agent_instance(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;
		std::string update_schema_provider_for_agent_controller(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const;

		virtual state_value_id_list get_system_core_state_values(prm::param_accessor acc) const = 0;

		//controller_ptr create_controller(prm::param_accessor acc) const;

		virtual system_ptr create_system_core(prm::param_accessor acc) const = 0;
		virtual system_ptr create_system(prm::param_accessor acc) const override;

	protected:
		// TODO: Use of shared pointer necessary only due to bug in Nov CTP
		typedef std::list< agent_defn_ptr > agent_defn_list_t;
		typedef std::map< std::string, std::shared_ptr< agent_defn_list_t > > agent_defn_map_t;

		typedef std::list< controller_defn_ptr > controller_defn_list_t;
		typedef std::map< std::string, std::shared_ptr< controller_defn_list_t > > controller_defn_map_t;

		agent_defn_map_t m_agent_defns;
		controller_defn_map_t m_controller_defns;
	};

}


#endif

