// rtp_sim.cpp

#include "rtp_sim.h"
#include "systems/rtp_system.h"
#include "systems/rtp_agent.h"
#include "rtp_observer.h"
#include "rtp_pop_wide_observer.h"
#include "params/integer_par.h"
#include "rtp_genome.h"
#include "rtp_genome_mapping.h"
#include "rtp_procreation_selection.h"

#include "wt_param_widgets/pw_yaml.h"

#include "ga/ga.h"
#include "ga/fitness.h"
#include "ga/roulette_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
#include "ga/single_objective.h"
#include "ga/pareto_multiple_objective.h"
#include "ga/collective_obj_val_ftr.h"
#include "ga/ranking_fitness.h"
#include "ga/rtp_interface/rtp_genome_wrap.h"
#include "ga/rtp_interface/rtp_individual.h"
#include "ga/rtp_interface/rtp_procreation_selection.h"

#include <chrono>


namespace YAML {
	template <>
	struct convert< rtp_procreation_selection::Type >
	{
		static Node encode(rtp_procreation_selection::Type const& rhs)
		{
			return Node(rtp_procreation_selection::Names[rhs]);
		}

		static bool decode(Node const& node, rtp_procreation_selection::Type& rhs)
		{
			if(!node.IsScalar())
			{
				return false;
			}

			auto it = mapping_.find(node.Scalar());
			if(it == mapping_.end())
			{
				return false;
			}

			rhs = it->second;
			return true;
		}

		static std::map< std::string, rtp_procreation_selection::Type > const mapping_;
	};

	std::map< std::string, rtp_procreation_selection::Type > const convert< rtp_procreation_selection::Type >::mapping_ = {
		{ "Random", rtp_procreation_selection::Type::Random },
		{ "Equal", rtp_procreation_selection::Type::Equal},
//		{ "Roulette", rtp_procreation_selection::Type::Roulette },
	};
}

rtp_named_param_list rtp_simulation::evo_params()
{
	rtp_named_param_list p;
	p.push_back(rtp_named_param(new rtp_integer_param_type(10, 1), "Population Size"));
	p.push_back(rtp_named_param(new rtp_integer_param_type(10, 1), "Num Generations"));
	p.push_back(rtp_named_param(new rtp_integer_param_type(1, 1), "Trials/Generation"));
	p.push_back(rtp_named_param(new rtp_integer_param_type(0, 0), "Random Seed"));
	p.push_back(rtp_named_param(new rtp_procreation_selection::enum_param_type(), "Procreation"));
	return p;
}

namespace sb = prm::schema;

YAML::Node rtp_simulation::get_evo_schema(YAML::Node const& param_vals)
{
	auto schema = sb::list("evo_params");
	sb::label(schema, "Evolutionary Params");

	sb::append(schema, sb::integer("Population Size", 10, 1));
	sb::append(schema, sb::integer("Num Generations", 10, 1));
	sb::append(schema, sb::integer("Trials/Generation", 1, 1));
	sb::append(schema, sb::integer("Random Seed", 0, 0));
	sb::append(schema, sb::enum_selection("Procreation", { begin(rtp_procreation_selection::Names), end(rtp_procreation_selection::Names) }));

	return schema;
}


rtp_simulation::rtp_simulation(rtp_param sys_param, rtp_param evo_param)
{
	std::tie(system, gn_mapping, a_factory, obs, resultant_obj) = i_system::create_instance(sys_param, true);

	rtp_param_list evo_parameters = boost::any_cast<rtp_param_list>(evo_param);
	population_size = boost::any_cast< int >(evo_parameters[0]);
	total_generations = boost::any_cast< int >(evo_parameters[1]);
	trials_per_generation = boost::any_cast< int >(evo_parameters[2]);
	int seed = boost::any_cast< int >(evo_parameters[3]);	// TODO: uint version of param?
	if(seed == 0)
	{
		// For now, 0 represents random
		base_seed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
	}
	else
	{
		base_seed = seed;
	}
	
	proc_sel = rtp_procreation_selection::create_instance(evo_parameters[4], rgen);
}

rtp_simulation::rtp_simulation(YAML::Node const& sys_param, YAML::Node const& evo_param)
{
	std::tie(system, gn_mapping, a_factory, obs, resultant_obj) = i_system::create_instance(sys_param, true);

	population_size = prm::find_value(evo_param, "Population Size").as< int >();
	total_generations = prm::find_value(evo_param, "Num Generations").as< int >();
	trials_per_generation = prm::find_value(evo_param, "Trials/Generation").as< int >();
	int seed = prm::find_value(evo_param, "Random Seed").as< int >();	// TODO: uint version of param?
	if(seed == 0)
	{
		// For now, 0 represents random
		base_seed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
	}
	else
	{
		base_seed = seed;
	}

	proc_sel = rtp_procreation_selection::create_instance(prm::find_value(evo_param, "Procreation").as< rtp_procreation_selection::Type >(), rgen);
}

/*
Want the GA to be the highest level process. The only thing it needs to know about is the genome type, so potentially it can 
remain templatized. We just call a virtual function of some object which knows about the genome representation, and this method
will return a base class pointer to a GA object set up for the given genome type. Then we just call a virtual function ga.do_generation().
From within that method, the ga will need to callback something along the lines of sys.evaluate_genome_fitnesses(genomes of entire pop).
*/

void rtp_simulation::init(boost::optional< uint32_t > seed)
{
	// Allow overriding of seed provided in params
	if(seed)
	{
		base_seed = *seed;
	}
	rgen.seed(base_seed);

	ga_rseed = boost::random::uniform_int_distribution<>()(rgen);
	trials_rseed = boost::random::uniform_int_distribution<>()(rgen);

	generation = 0;
}

genotype_diversity_measure rtp_simulation::population_genotype_diversity() const
{
	std::vector< i_genome const* > genomes(population_size);
	for(size_t n = 0; n < population_size; ++n)
	{
		genomes[n] = population[n].gn.get();
	}
	return gn_mapping->population_diversity(genomes);
}


typedef double														raw_obj_val_t;
typedef coll_obj_val_identity< raw_obj_val_t >						collective_obj_val_fn_t;
typedef collective_obj_val_fn_t::processed_obj_val_t				processed_obj_val_t;
typedef	ranking_fitness< processed_obj_val_t >						fitness_assignment_fn_t;
typedef fitness_assignment_fn_t::fitness_t							fitness_t;
//typedef fixednn_genome_mapping										genome_mapping_t;
//typedef genome_mapping_t::genome									genome_t;
typedef i_genome_mapping::cx_fn_t									rtp_crossover_t;
typedef i_genome_mapping::mut_fn_t									rtp_mutation_t;
typedef rtp_genome_wrapper											rtp_genome_t;
typedef rtp_individual												individual_t;
typedef std::vector< individual_t >									population_t;
typedef std::vector< fitness_t >									fitness_acc_t;
typedef fitness_assignment_fn_t::ProcessedAccessorDefn< fitness_acc_t >::processed_obj_val_acc_t	processed_obj_val_acc_t;
typedef collective_obj_val_fn_t::RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t		raw_obj_val_acc_t;

typedef truncation_survival_selection< population_t::iterator >		survival_sel_t;

//typedef equal_procreation_selection<>								proc_sel_t;
typedef i_procreation_selection										proc_sel_t;

//typedef basic_crossover< genome_t >					crossover_t;
typedef LinearCrossoverRate							crossover_rate_t;
//typedef basic_real_mutation< genome_t >				mutation_t;
typedef LinearMutationRate							mutation_rate_t;

typedef gen_alg<
	individual_t,
	survival_sel_t,
	rtp_crossover_t,
	rtp_mutation_t,
	crossover_rate_t,
	mutation_rate_t,
	proc_sel_t
> ga_t;
	
void rtp_simulation::run_epoch(/*observation_data_t& observations,*/ std::ostream& os)
{
	rgen.seed(ga_rseed);
	/* TODO:
	If end up having a persistent (across epochs) ga, will need to reset any rand distributions here.
	*/

	if(generation == 0)
	{
		// Init random population
		population.resize(population_size);
		for(size_t idv = 0; idv < population_size; ++idv)
		{
			i_genome* gn = gn_mapping->generate_random_genome(rgen);
			population[idv].gn.reset(gn);
			population[idv].idv.reset(a_factory->create());
		}
	}
	else
	{
		//proc_sel_t proc_sel(rgen);
		rtp_crossover_t cx_ftr = gn_mapping->get_crossover_fn(rgen);
			//(gn_mapping->get_genome_length(), rgen);
		crossover_rate_t cx_rate_ftr(0.5, 0.8);
		rtp_mutation_t mut_ftr = gn_mapping->get_mutation_fn(rgen);
			//(rgen);
		mutation_rate_t mut_rate_ftr(0.25, 0.05);
		ga_t ga(*proc_sel, cx_ftr, cx_rate_ftr, mut_ftr, mut_rate_ftr, rgen);
		ga.init_params(0 /* unused */, total_generations);
		ga.generation = generation;
		ga.population.resize(population_size);

		// Produce next generation
		for(size_t idv = 0; idv < population_size; ++idv)
		{
			ga.population[idv].genome = //*boost::static_pointer_cast< fixednn_genome_mapping::genome >
				(population[idv].gn);
			ga.population[idv].fitness = population[idv].fitness;
		}

		ga.epoch();

		for(size_t idv = 0; idv < population_size; ++idv)
		{
			//*boost::static_pointer_cast< fixednn_genome_mapping::genome >(population[idv].gn) = ga.population[idv].genome;
			population[idv].gn.reset(ga.population[idv].genome.get()->clone());	// TODO: Cloning probably unnecessary here...
		}
	}

	// Recreate the ga rand seed so that ga code will get different random numbers in next generation
	ga_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);

	// Seed for trials
	rgen.seed(trials_rseed);

	// Decode population genomes
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		gn_mapping->decode_genome(population[idv].gn.get(), population[idv].idv.get());
	}

/*	fitness_acc_t avg_fitnesses(population_size);
	processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, population_size);
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg_obj_vals[idv] = processed_obj_val_t(0.0);
	}
*/
	i_population_wide_observer::eval_data_t obj_val_eval_data = resultant_obj->initialize(population_size);

	for(size_t trial = 0; trial < trials_per_generation; ++trial)
	{
		fitness_acc_t fitnesses(population_size);

		//processed_obj_val_acc_t processed_obj_val_acc(fitnesses, population_size);
		//raw_obj_val_acc_t raw_obj_val_acc(processed_obj_val_acc, population_size);
		std::vector< i_observer::observations_t > observations(population_size);

		boost::any initial_st = system->generate_initial_state(rgen);

		for(size_t idv = 0; idv < population_size; ++idv)
		{
			system->clear_agents();
			system->register_agent(population[idv].idv);
			system->set_state(initial_st);
			obs->reset();

			bool system_active = true;
			while(system_active)
			{
				system_active = system->update(obs);
			}

			// Store observations
			observations[idv] = system->record_observations(obs);
		}

		// Invoke the collective objective value processing
		//collective_obj_val_fn_t() (raw_obj_val_acc, population_size, processed_obj_val_acc);
		resultant_obj->register_datapoint(observations, obj_val_eval_data);

/*		for(size_t idv = 0; idv < population_size; ++idv)
		{
			//avg_obj_vals[idv] += processed_obj_val_acc[idv];
			avg_obj_vals[idv] += boost::any_cast<double>(resultant_vals[idv]);
		}
*/	}

/*	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg_obj_vals[idv] = avg_obj_vals[idv] / (double)trials_per_generation;
	}
*/
	boost::optional< std::string > analysis = "";
	std::vector< boost::any > resultant_vals = resultant_obj->evaluate(obj_val_eval_data, analysis);

	fitness_acc_t avg_fitnesses(population_size);
	processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, population_size);

	// TODO: temp resolution to issue below, but ideally don't want to require this copying
	std::vector< processed_obj_val_t > unordered_obj_vals(population_size);
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg_obj_vals[idv] = 
			unordered_obj_vals[idv] = boost::any_cast<double>(resultant_vals[idv]);
			//avg_obj_vals[idv];
	}
	//

	// Finally assign population fitness based on processed values
	fitness_assignment_fn_t().reassign(avg_obj_vals, avg_fitnesses);

	for(size_t idv = 0; idv < population_size; ++idv)
	{
		population[idv].obj_value = unordered_obj_vals[idv];
		population[idv].fitness = avg_fitnesses[idv].value;
	}

	// Is training data (ie. system initial state) the same for every generation?
	bool const StaticTrainingSet = false;
	// Regardless of this setting, data will be the same for all agents within a given generation.

	// If we don't want a static training set, recreate the trials rand seed 
	if(!StaticTrainingSet)
	{
		trials_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);
	}

	++generation;

	os << "Gen " << generation << ": " << *analysis;
}
	
i_genome* rtp_simulation::get_individual_genome(size_t idx)
{
	return nullptr;
}

boost::shared_ptr< i_agent > rtp_simulation::get_fittest() const
{
	boost::optional< size_t > index;
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		if(!index || population[idv].fitness > population[*index].fitness)
		{
			index = idv;
		}
	}
	return population[*index].idv;	// TODO: Should clone!
}

boost::any rtp_simulation::get_highest_objective() const
{
	boost::optional< size_t > index;
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		if(!index || population[idv].fitness > population[*index].fitness)
		{
			index = idv;
		}
	}
	return population[*index].obj_value;
}

boost::any rtp_simulation::get_average_objective() const
{
	double avg = 0.0;
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg += population[idv].obj_value;
	}
	return avg / population_size;
}


