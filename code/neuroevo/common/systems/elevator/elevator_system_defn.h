// elevator_system_defn.h

#ifndef __WB_NE_ELEVATOR_SYSTEM_DEFN_H
#define __WB_NE_ELEVATOR_SYSTEM_DEFN_H

#include "system_sim/basic_system_defn.h"


namespace sys {

	struct update_info;

	namespace elev {

		typedef size_t floor_count_t;
		typedef size_t floor_t;

		class elevator_system_defn:
			public basic_system_defn
		{
		public:
			static std::unique_ptr< elevator_system_defn > create_default();

		public:
			virtual std::string get_name() const override;

			virtual bool is_instantaneous() const override;
			virtual update_info get_update_info() const override;

			virtual state_value_id_list get_system_core_state_values(prm::param_accessor acc) const override;

			virtual system_ptr create_system_core(prm::param_accessor acc) const override;

		public:
			static std::string floor_name(floor_t flr);

		protected:
			virtual std::string update_schema_provider_for_system_core(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix) const override;

		private:
//			static state_value_id_list generate_state_value_names(floor_count_t num_floors);
//			static std::map< std::string, int > generate_state_value_idmap(floor_count_t num_floors);

			enum class CoreStateValue: unsigned long;
			static const bimap< CoreStateValue, std::string > s_core_state_values;

			enum class FloorStateValue: unsigned long;
			static const bimap< FloorStateValue, std::string > s_floor_state_values;

			friend class elevator_system;
		};

	}
}


#endif


