// ga_params.cpp

#include "ga_params.h"
#include "ga_params_rate_functions.h"

#include "genetic_algorithm/fitness_assignment.h"

#include "params/param_accessor.h"
#include "params/schema_builder.h"

#include "function_opt_domain.h"
#include "evolvable_systems/evolved_system_domain_defn.h"


namespace ga {

	namespace sb = prm::schema;

	ga_defn::ga_defn()
	{
		rate_fn_defn.register_option("fixed", fixed_rate_fn_defn::update_schema_provider, fixed_rate_fn_defn::generate);
		rate_fn_defn.register_option("variable", generation_variable_rate_fn_defn::update_schema_provider, generation_variable_rate_fn_defn::generate);

		domain_defn.register_option("func_opt", std::make_shared< domain::fnopt::function_opt_domain_defn >());
		domain_defn.register_option("syssim", std::make_shared< sys::ev::evolved_system_domain_defn >());
	}

	std::string ga_defn::update_schema_provider_for_ga(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto path = prefix;

		(*provider)[path + std::string{ "pop_size" }] = [=](prm::param_accessor acc)
		{
			auto s = sb::integer(
				"pop_size",
				10,
				1
				);
			return s;
		};

		path += std::string{ "crossover" };

		rate_fn_defn.update_schema_provider(provider, path + std::string{ "cx_rate" });

		// TODO:			domain_defn.update_schema_provider(provider, path + std::string{ "cx_op" }, domain::Components::CrossoverOp);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string{ "cx_rate" })(acc));
			//..				sb::append(s, provider->at(path + std::string{ "cx_op" })(acc));
			return s;
		};

		path.pop();

		path += std::string{ "mutation" };

		rate_fn_defn.update_schema_provider(provider, path + std::string{ "mut_rate" });

		// TODO:			domain_defn.update_schema_provider(provider, path + std::string{ "mut_op" }, domain::Components::MutationOp);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string{ "mut_rate" })(acc));
			//..				sb::append(s, provider->at(path + std::string{ "mut_op" })(acc));
			return s;
		};

		path.pop();

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + std::string{ "pop_size" })(acc));
			sb::append(s, provider->at(path + std::string{ "crossover" })(acc));
			sb::append(s, provider->at(path + std::string{ "mutation" })(acc));
			return s;
		};

		return path.leaf().name();
	}

	void ga_defn::update_schema_provider(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto path = prefix;

		auto ga_cfg = std::string{ "ga_cfg" };
		auto domain_cfg = std::string{ "domain_cfg" };
		auto fitness = std::string{ "fitness_assignment" };
		
		update_schema_provider_for_ga(provider, path + ga_cfg);
		domain_defn.update_schema_provider(provider, path + domain_cfg);
//		fa_defn_.update_schema_provider(provider, path + fitness);

		(*provider)[path] = [=](prm::param_accessor acc)
		{
			auto s = sb::list(path.leaf().name());
			sb::append(s, provider->at(path + ga_cfg)(acc));
			sb::append(s, provider->at(path + domain_cfg)(acc));
//			sb::append(s, provider->at(path + fitness)(acc));
			return s;
		};
	}


	std::shared_ptr< i_problem_domain > ga_defn::generate_domain(prm::param_accessor acc)
	{
		auto domain = domain_defn.generate(acc("domain_cfg"));
		return domain;
	}

	std::unique_ptr< genalg > ga_defn::generate_alg(prm::param_accessor acc, std::shared_ptr< i_problem_domain > domain)
	{
		auto alg = std::make_unique< genalg >(*domain);

		acc.move_relative(std::string{ "ga_cfg" });

		auto pop_size = prm::extract_as< int >(acc["pop_size"]);
		alg->set_initial_population_size(pop_size);

		acc.move_relative(std::string{ "crossover" });
		acc.move_relative(std::string{ "cx_rate" });
		alg->set_crossover_rate_fn(rate_fn_defn.generate(acc));
		acc.revert();
		acc.revert();

		acc.move_relative(std::string{ "mutation" });
		acc.move_relative(std::string{ "mut_rate" });
		alg->set_mutation_rate_fn(rate_fn_defn.generate(acc));
		acc.revert();
		acc.revert();

		acc.revert();

		auto obj_val_type = domain->objective_value_type();
		switch(obj_val_type)
		{
			case objective_value::type::Real:
			alg->set_fitness_assignment(&direct_fitness_assignment);
			break;
			case objective_value::type::Pareto:
			alg->set_fitness_assignment(&pareto_nondominated_ranking_fitness_assignment);
			break;
		}

		return std::move(alg);
	}

}




