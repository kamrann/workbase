// objective.h

#ifndef __WB_EV_SYS_OBJECTIVE_H
#define __WB_EV_SYS_OBJECTIVE_H

#include "system_sim/system_state_values.h"

#include "genetic_algorithm/objective_value.h"

#include "params/param_fwd.h"
#include "params/dynamic_enum_schema.h"

#include <memory>
#include <functional>


namespace sys {
	namespace ev {

		class objective
		{
		public:
			virtual ga::objective_value::type get_type() = 0;

			virtual void reset() = 0;
			virtual void update() = 0;
			virtual ga::objective_value evaluate() = 0;

		public:
			virtual ~objective() {};
		};


		class objective_defn
		{
		public:
			typedef std::function< state_value_id_list(prm::param_accessor) > state_val_access_fn_t;

		public:
			objective_defn(state_val_access_fn_t sv_acc_fn);

		public:
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			std::unique_ptr< objective > generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn);

		protected:
			state_val_access_fn_t sv_acc_fn_;
			prm::dynamic_enum_schema< std::unique_ptr< objective >, std::function< double(state_value_id) > > obj_type_defn_;
		};

	}
}



#endif

