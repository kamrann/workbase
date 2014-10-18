// phys2d_system_defn.h

#ifndef __WB_NE_PHYSICS_2D_SYSTEM_DEFN_H
#define __WB_NE_PHYSICS_2D_SYSTEM_DEFN_H

#include "scenario_defn.h"
#include "system_sim/basic_system_defn.h"


namespace sys {

	struct update_info;

	namespace phys2d {

		class phys2d_system_defn:
			public basic_system_defn
		{
		public:
			static std::unique_ptr< phys2d_system_defn > create_default();

		public:
			virtual std::string get_name() const override;

			virtual bool is_instantaneous() const override;
			virtual update_info get_update_info() const override;

			void add_scenario_defn(scenario_defn_ptr defn);

			virtual state_value_id_list get_system_core_state_values(prm::param_accessor param_vals) const override;

			virtual system_ptr create_system_core(prm::param_accessor acc) const override;

		private:
			virtual std::string update_schema_provider_for_system_core(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

		private:
			enum class StateValue: unsigned long;
			static const bimap< StateValue, std::string > s_state_values;

			std::map< std::string, scenario_defn_ptr > m_scenario_defns;

			friend class phys2d_system;
		};

	}
}


#endif


