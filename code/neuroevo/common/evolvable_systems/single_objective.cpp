// single_objective.cpp

#include "single_objective.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


namespace sys {
	namespace ev {

		single_objective::single_objective(std::unique_ptr< value_objective > value_obj):
			value_obj_(std::move(value_obj))
		{

		}

		ga::objective_value::type single_objective::get_type()
		{
			return ga::objective_value::type::Real;
		}

		void single_objective::reset()
		{
			value_obj_->reset();
		}
		
		void single_objective::update()
		{
			value_obj_->update();
		}
		
		ga::objective_value single_objective::evaluate()
		{
			value_obj_->finalise();

			auto val = value_obj_->get_objective_value();
			if(val)
			{
				return *val;
			}
			else
			{
				// TODO: 
				throw std::runtime_error("objective value not computed");
			}
		}


		single_objective_defn::single_objective_defn(state_val_access_fn_t sv_acc_fn):
			val_obj_defn_(sv_acc_fn)
		{

		}

		void single_objective_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			val_obj_defn_.update_schema_provider(provider, prefix);
		}

		std::unique_ptr< single_objective > single_objective_defn::generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn)
		{
			return std::make_unique< single_objective >(val_obj_defn_.generate(acc, get_state_value_fn));
		}

	}
}




