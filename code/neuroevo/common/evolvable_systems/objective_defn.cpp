// objective_defn.cpp

#include "objective.h"
#include "single_objective.h"
#include "pareto_objective.h"

#include "params/param_accessor.h"
#include "params/dynamic_enum_schema.h"


namespace sys {
	namespace ev {

		objective_defn::objective_defn(state_val_access_fn_t sv_acc_fn)
		{
			obj_type_defn_.register_option(
				"single_objective", std::make_shared< single_objective_defn >(sv_acc_fn));
			obj_type_defn_.register_option(
				"pareto_multi_objective", std::make_shared< pareto_objective_defn >(sv_acc_fn));
		}

		void objective_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			obj_type_defn_.update_schema_provider(provider, prefix);
		}
		
		std::unique_ptr< objective > objective_defn::generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn)
		{
			return obj_type_defn_.generate(acc, get_state_value_fn);
		}

	}
}




