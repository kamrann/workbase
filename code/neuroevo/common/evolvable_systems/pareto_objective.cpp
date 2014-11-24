// pareto_objective.cpp

#include "pareto_objective.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"


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


		pareto_objective_defn::pareto_objective_defn(state_val_access_fn_t sv_acc_fn):
			val_obj_defn_(sv_acc_fn)
		{

		}

		namespace sb = prm::schema;

		void pareto_objective_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
		{
			auto path = prefix;

			path += std::string{ "components" };

			val_obj_defn_.update_schema_provider(provider, path + std::string{ "comp" });

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::repeating_list(path.leaf().name(), "comp");
				return s;
			};

			path.pop();

			(*provider)[path] = [=](prm::param_accessor acc)
			{
				auto s = sb::list(path.leaf().name());
				sb::append(s, provider->at(path + std::string{ "components" })(acc));
				// TODO: pareto config? fitness conversion should be left to ga probably.
				return s;
			};
		}

		std::unique_ptr< pareto_objective > pareto_objective_defn::generate(prm::param_accessor acc, std::function< double(state_value_id) > get_state_value_fn)
		{
			std::vector< std::unique_ptr< value_objective > > components;

			acc.move_to(acc.find_path("components"));
			auto comp_paths = acc.children();
			for(auto const& p : comp_paths)
			{
				acc.move_to(p);
				components.push_back(val_obj_defn_.generate(acc, get_state_value_fn));
				acc.revert();
			}
			acc.revert();

			return std::make_unique< pareto_objective >(std::move(components));
		}

	}
}




