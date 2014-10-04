// elevator_system_defn.h

#ifndef __WB_NE_ELEVATOR_SYSTEM_DEFN_H
#define __WB_NE_ELEVATOR_SYSTEM_DEFN_H

#include "../../system_sim/basic_system_defn.h"


namespace sys {

	struct update_info;

	namespace elev {

		typedef size_t floor_count_t;
		typedef size_t floor_t;

		class elevator_system_defn:
			public basic_system_defn
		{
		public:
			virtual bool is_instantaneous() const override;
			virtual update_info get_update_info() const override;

			virtual std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;
			//static agents_data generate_agents_data(prm::param_accessor param_vals, bool evolvable);

			virtual std::vector< std::string > get_agent_type_names() const override;
			virtual std::vector< std::string > get_agent_sensor_names(prm::param agent_type, prm::param_accessor param_vals) const override;
			virtual size_t get_agent_num_effectors(prm::param agent_type) const override;
			virtual std::vector< std::string > get_system_state_values(prm::param_accessor param_vals) const override;

			virtual system_ptr create_system(prm::param_accessor acc) const override;

		public:
			static std::string floor_name(floor_t flr);

		private:
			static std::vector< std::string > generate_state_value_names(floor_count_t num_floors);
			static std::map< std::string, int > generate_state_value_idmap(floor_count_t num_floors);

			friend class elevator_system;
		};

	}
}


#endif


