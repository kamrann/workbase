// objective_defn.h

#ifndef __WB_EV_SYS_OBJECTIVE_DEFN_H
#define __WB_EV_SYS_OBJECTIVE_DEFN_H

#include "system_sim/system_state_values.h"
#include "single_objective.h"
#include "pareto_objective.h"
#include "ddl/ddl.h"
#include "ddl/components/enum_choice.h"

#include <memory>
#include <functional>


namespace sys {
	namespace ev {

		class objective_defn
		{
		public:
			typedef //std::function< state_value_id_list(ddl::navigator) > state_val_access_fn_t;
				ddl::dep_function< state_value_id_list > state_val_access_fn_t;

		public:
			objective_defn(state_val_access_fn_t& sv_acc_fn);

		public:
			ddl::defn_node get_defn(ddl::specifier& spc);
			std::unique_ptr< objective > generate(ddl::navigator nav, std::function< double(state_value_id) > get_state_value_fn);

		protected:
//			state_val_access_fn_t& sv_acc_fn_;
			ddl::enum_choice< std::unique_ptr< objective >, std::function< double(state_value_id) > > obj_type_defn_;
			//single_objective_defn single_;
			//pareto_objective_defn pareto_;
		};

	}
}



#endif

