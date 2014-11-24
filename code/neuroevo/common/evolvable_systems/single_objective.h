// single_objective.h

#ifndef __WB_EV_SYS_SINGLE_OBJECTIVE_H
#define __WB_EV_SYS_SINGLE_OBJECTIVE_H

#include "objective.h"
#include "value_objective.h"

#include "params/param_fwd.h"

#include <memory>


namespace sys {
	namespace ev {

		class single_objective:
			public objective
		{
		public:
			single_objective(std::unique_ptr< value_objective > value_obj);

		public:
			virtual ga::objective_value::type get_type() override;

			virtual void reset() override;
			virtual void update() override;
			virtual ga::objective_value evaluate() override;

		protected:
			std::unique_ptr< value_objective > value_obj_;
		};


		class single_objective_defn
		{
		public:
			typedef std::function< state_value_id_list(prm::param_accessor) > state_val_access_fn_t;

		public:
			single_objective_defn(state_val_access_fn_t sv_acc_fn);

		public:
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);
			void update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix, int) {}
			std::unique_ptr< single_objective > generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn);

		protected:
			value_objective_defn val_obj_defn_;
		};

	}
}



#endif

