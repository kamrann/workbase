// rtp_sim.cpp

#include "rtp_sim.h"
#include "systems/rtp_system.h"
#include "rtp_observer.h"
#include "params/integer_par.h"

#include "ga/ga.h"
#include "ga/fitness.h"
#include "ga/roulette_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
#include "ga/single_objective.h"
#include "ga/pareto_multiple_objective.h"
#include "ga/collective_obj_val_ftr.h"
#include "ga/ranking_fitness.h"

#include <chrono>


rtp_named_param_list rtp_simulation::evo_params()
{
	rtp_named_param_list p;
	p.push_back(rtp_named_param(new rtp_integer_param_type(10, 1), "Population Size"));
	p.push_back(rtp_named_param(new rtp_integer_param_type(10, 1), "Num Generations"));
	p.push_back(rtp_named_param(new rtp_integer_param_type(1, 1), "Trials/Generation"));
	return p;
}

rtp_simulation::rtp_simulation(rtp_param sys_param, rtp_param evo_param)
{
	std::tie(system, gn_mapping, a_factory, obs) = i_system::create_instance(sys_param, true);

	rtp_param_list evo_parameters = boost::any_cast<rtp_param_list>(evo_param);
	population_size = boost::any_cast< int >(evo_parameters[0]);
	total_generations = boost::any_cast< int >(evo_parameters[1]);
	trials_per_generation = boost::any_cast< int >(evo_parameters[2]);
}

/*
Want the GA to be the highest level process. The only thing it needs to know about is the genome type, so potentially it can 
remain templatized. We just call a virtual function of some object which knows about the genome representation, and this method
will return a base class pointer to a GA object set up for the given genome type. Then we just call a virtual function ga.do_generation().
From within that method, the ga will need to callback something along the lines of sys.evaluate_genome_fitnesses(genomes of entire pop).
*/

void rtp_simulation::init()
{
	rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));

	ga_rseed = boost::random::uniform_int_distribution<>()(rgen);
	trials_rseed = boost::random::uniform_int_distribution<>()(rgen);

	generation = 0;
}

genotype_diversity_measure rtp_simulation::population_genotype_diversity() const
{
	return genotype_diversity_measure();
}

// TEMP
#include "systems/sat/scenarios/full_stop/agents.h"
#include "rtp_fixednn_genome_mapping.h"

typedef double														raw_obj_val_t;
typedef coll_obj_val_identity< raw_obj_val_t >						collective_obj_val_fn_t;
typedef collective_obj_val_fn_t::processed_obj_val_t				processed_obj_val_t;
typedef	ranking_fitness< processed_obj_val_t >						fitness_assignment_fn_t;
typedef fitness_assignment_fn_t::fitness_t							fitness_t;
typedef fixednn_genome_mapping										genome_mapping_t;
typedef genome_mapping_t::genome									genome_t;
typedef individual< genome_t, processed_obj_val_t, fitness_t >		individual_t;
typedef std::vector< individual_t >									population_t;
typedef std::vector< fitness_t >									fitness_acc_t;
typedef fitness_assignment_fn_t::ProcessedAccessorDefn< fitness_acc_t >::processed_obj_val_acc_t	processed_obj_val_acc_t;
typedef collective_obj_val_fn_t::RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t		raw_obj_val_acc_t;

typedef truncation_survival_selection< population_t::iterator >		survival_sel_t;

typedef equal_procreation_selection<>								proc_sel_t;

typedef basic_crossover< genome_t >					crossover_t;
typedef LinearCrossoverRate							crossover_rate_t;
typedef basic_real_mutation< genome_t >				mutation_t;
typedef LinearMutationRate							mutation_rate_t;

typedef gen_alg<
	individual_t,
	survival_sel_t,
	crossover_t,
	mutation_t,
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
			population[idv].gn = gn;
			population[idv].idv = a_factory->create();
		}
	}
	else
	{
		proc_sel_t proc_sel(rgen);
		crossover_t cx_ftr(gn_mapping->get_genome_length(), rgen);
		crossover_rate_t cx_rate_ftr(0.5, 0.8);
		mutation_t mut_ftr(rgen);
		mutation_rate_t mut_rate_ftr(0.25, 0.05);
		ga_t ga(proc_sel, cx_ftr, cx_rate_ftr, mut_ftr, mut_rate_ftr, rgen);
		ga.init_params(0 /* unused */, total_generations);
		ga.generation = generation;
		ga.population.resize(population_size);

		// Produce next generation
		for(size_t idv = 0; idv < population_size; ++idv)
		{
			ga.population[idv].genome = *(fixednn_genome_mapping::genome*)population[idv].gn;
			ga.population[idv].fitness = population[idv].fitness;
		}

		ga.epoch();

		for(size_t idv = 0; idv < population_size; ++idv)
		{
			*(fixednn_genome_mapping::genome*)population[idv].gn = ga.population[idv].genome;
		}
	}

	// Recreate the ga rand seed so that ga code will get different random numbers in next generation
	ga_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);

	// Seed for trials
	rgen.seed(trials_rseed);

	// Decode population genomes
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		gn_mapping->decode_genome(population[idv].gn, population[idv].idv);
	}

	fitness_acc_t avg_fitnesses(population_size);
	processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, population_size);
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg_obj_vals[idv] = processed_obj_val_t(0.0);
	}

	for(size_t trial = 0; trial < trials_per_generation; ++trial)
	{
		fitness_acc_t fitnesses(population_size);

		processed_obj_val_acc_t processed_obj_val_acc(fitnesses, population_size);
		raw_obj_val_acc_t raw_obj_val_acc(processed_obj_val_acc, population_size);

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

			// Evaluate agent performance
			boost::any observations = system->record_observations(obs);
			raw_obj_val_acc[idv] = boost::any_cast< double >(observations);
		}

		// Invoke the collective objective value processing
		collective_obj_val_fn_t() (raw_obj_val_acc, population_size, processed_obj_val_acc);

		for(size_t idv = 0; idv < population_size; ++idv)
		{
			avg_obj_vals[idv] += processed_obj_val_acc[idv];
		}
	}

	for(size_t idv = 0; idv < population_size; ++idv)
	{
		avg_obj_vals[idv] = avg_obj_vals[idv] / (double)trials_per_generation;
	}

	// TODO: temp resolution to issue below, but ideally don't want to require this copying
	std::vector< processed_obj_val_t > unordered_obj_vals(population_size);
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		unordered_obj_vals[idv] = avg_obj_vals[idv];
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
}
	
i_genome* rtp_simulation::get_individual_genome(size_t idx)
{
	return nullptr;
}

i_agent* rtp_simulation::get_fittest() const
{
	boost::optional< size_t > index;
	for(size_t idv = 0; idv < population_size; ++idv)
	{
		if(!index || population[idv].fitness > population[*index].fitness)
		{
			index = idv;
		}
	}
	return population[*index].idv;	// TODO: Clone? Or use shared pointers?
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


