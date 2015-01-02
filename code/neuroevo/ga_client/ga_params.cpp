// ga_params.cpp

#include "ga_params.h"
#include "ga_params_rate_functions.h"

#include "genetic_algorithm/fitness_assignment.h"

#include "function_opt_domain.h"
#include "simple_timer_domain.h"
#include "evolvable_systems/evolved_system_domain_defn.h"


namespace ga {

	ga_defn::ga_defn()
	{
		rate_fn_defn.register_option("fixed", fixed_rate_fn_defn::get_defn, fixed_rate_fn_defn::generate);
		rate_fn_defn.register_option("variable", generation_variable_rate_fn_defn::get_defn, generation_variable_rate_fn_defn::generate);

		domain_defn.register_option("func_opt", std::make_shared< domain::fnopt::function_opt_domain_defn >());
		domain_defn.register_option("timer", std::make_shared< domain::timer::simple_timer_domain_defn >());
		domain_defn.register_option("syssim", std::make_shared< sys::ev::evolved_system_domain_defn >());
	}

	ddl::defn_node ga_defn::get_defn_for_ga(ddl::specifier& spc)
	{
		ddl::defn_node pop_size = spc.integer("pop_size")
			(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
			(ddl::spc_default < ddl::integer_defn_node::value_t > { 10 })
			;

		ddl::defn_node cx_rate = rate_fn_defn.get_defn(spc);
		// TODO:	cx_op = domain_defn.update_schema_provider(provider, path + std::string{ "cx_op" }, domain::Components::CrossoverOp);

		ddl::defn_node crossover = spc.composite("crossover")(ddl::define_children{}
			("cx_rate", cx_rate)
// TODO: how?			("cx_op", cx_op)
			);

		ddl::defn_node mut_rate = rate_fn_defn.get_defn(spc);
		// TODO:			domain_defn.update_schema_provider(provider, path + std::string{ "mut_op" }, domain::Components::MutationOp);

		ddl::defn_node mutation = spc.composite("mutation")(ddl::define_children{}
			("mut_rate", mut_rate)
			// TODO: how?			("mut_op", mut_op)
			);

		/* TODO: Maybe put this in fitness assignment section, or in fact eventually, probably better off
		with this being part of the domain defn */
		ddl::defn_node num_trials = spc.integer("num_trials")
			(ddl::spc_min < ddl::integer_defn_node::value_t > { 1 })
			(ddl::spc_default < ddl::integer_defn_node::value_t > { 1 })
			;
		ddl::defn_node trial_merging = spc.enumeration("trial_merging")
			(ddl::spc_range < size_t > { 1, 1 })
			(ddl::define_enum_fixed{}("average")("worst"))
			(ddl::spc_default < ddl::enum_defn_node::str_value_t > { { "average" } })
			;

		ddl::defn_node trials = spc.composite("trials")(ddl::define_children{}
			("num_trials", num_trials)
			("trial_merging", trial_merging)
			);
		////////////////////////////////////

		return spc.composite("ga_cfg")(ddl::define_children{}
			("pop_size", pop_size)
			("crossover", crossover)
			("mutation", mutation)
			("trials", trials)
			);
	}

	ddl::defn_node ga_defn::get_defn(ddl::specifier& spc)
	{
		return spc.composite("ga_client")(ddl::define_children{}
			("ga_cfg", get_defn_for_ga(spc))
			("domain_cfg", domain_defn.get_defn(spc))
//			("fitness_assignment", fa_defn_.get_defn(spc))
			);
	}


	std::shared_ptr< i_problem_domain > ga_defn::generate_domain(ddl::navigator nav)
	{
		auto domain = domain_defn.generate(nav["domain_cfg"]);
		return domain;
	}

	std::unique_ptr< genalg > ga_defn::generate_alg(ddl::navigator nav, std::shared_ptr< i_problem_domain > domain)
	{
		auto alg = std::make_unique< genalg >(*domain);

		nav = nav["ga_cfg"];

		auto pop_size = nav["pop_size"].get().as_int();
		alg->set_initial_population_size(pop_size);

		alg->set_crossover_rate_fn(rate_fn_defn.generate(nav["crossover"]["cx_rate"]));
		alg->set_mutation_rate_fn(rate_fn_defn.generate(nav["mutation"]["mut_rate"]));

		trials_cfg trials;
		trials.num_trials = nav["trials"]["num_trials"].get().as_int();
		// TODO: bimap
		auto merging_str = nav["trials"]["trial_merging"].get().as_single_enum_str();
		if(merging_str == "average")
		{
			trials.merging = trials_cfg::MergeMethod::Average;
		}
		else
		{
			trials.merging = trials_cfg::MergeMethod::Worst;
		}
		alg->set_trials_configuration(trials);

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




