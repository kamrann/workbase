// pareto_objective.cpp

#include "pareto_objective.h"

//#include "params/param_accessor.h"
//#include "params/schema_builder.h"


namespace sys {
	namespace ev {

		pareto_objective::pareto_objective(std::vector< std::unique_ptr< value_objective > > components):
			components_(std::move(components))
		{

		}

		ga::objective_value::type pareto_objective::get_type()
		{
			return ga::objective_value::type::Pareto;
		}

		void pareto_objective::reset()
		{
			for(auto& comp : components_)
			{
				comp->reset();
			}
		}
		
		void pareto_objective::update()
		{
			for(auto& comp : components_)
			{
				comp->update();
			}
		}
		
		ga::objective_value pareto_objective::evaluate()
		{
			pareto< double > par;
			for(auto& comp : components_)
			{
				comp->finalise();

				auto val = comp->get_objective_value();
				if(val)
				{
					par.push_component(*val);
				}
				else
				{
					// TODO: 
					throw std::runtime_error("objective value not computed");
				}
			}

			return par;
		}


		pareto_objective_defn::pareto_objective_defn(state_val_access_fn_t& sv_acc_fn):
			val_obj_defn_(sv_acc_fn)
		{

		}


		ddl::defn_node pareto_objective_defn::get_defn(ddl::specifier& spc)
		{
			ddl::defn_node component_defn = val_obj_defn_.get_defn(spc);

			ddl::defn_node component_list = spc.list("components")
				(ddl::spc_range < size_t > { 1, boost::none })
				(ddl::spc_item{ component_defn })
				;

			return spc.composite("pareto")(ddl::define_children{}
				("components", component_list)
				// TODO: pareto config? fitness conversion should be left to ga probably.
				);
		}

		std::unique_ptr< pareto_objective > pareto_objective_defn::generate(ddl::navigator nav, std::function< double(state_value_id) > get_state_value_fn)
		{
			std::vector< std::unique_ptr< value_objective > > components;

			auto comp_list_nav = nav["components"];
			auto count = comp_list_nav.list_num_items();
			for(size_t i = 0; i < count; ++i)
			{
				components.push_back(val_obj_defn_.generate(comp_list_nav[i], get_state_value_fn));
			}

			return std::make_unique< pareto_objective >(std::move(components));
		}

	}
}




