// sim.h

#ifndef __NE_SIM_H
#define __NE_SIM_H

/*
#include "systems/noughts_and_crosses/objectives/maximize_valid_moves.h"
#include "systems/noughts_and_crosses/objectives/try_to_win.h"
#include "systems/noughts_and_crosses/observers/basic_observer.h"
#include "systems/noughts_and_crosses/genetic_mappings/fixednn_sln.h"
#include "systems/fixed_neural_net.h"
*/

#include "ga/ga.h"
//#include "ga/genome.h"
#include "ga/fitness.h"
#include "ga/roulette_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
// TODO: Ideally won't need these and will supply desired types/ftrs as template parameters
#include "ga/single_objective.h"
#include "ga/pareto_multiple_objective.h"
#include "ga/collective_obj_val_ftr.h"
//#include "ga/direct_fitness.h"
#include "ga/ranking_fitness.h"
#include "ga/hard_constraint.h"
#include "ga/shared_constraints.h"

#include "doublefann.h"
#include "fann_cpp.h"

#include <Eigen/StdVector>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/smart_ptr.hpp>

#include <chrono>


template <
	typename Scenario,
	typename GeneticMapping,
	typename TrialData,
	typename ResultantObjective,
	typename CollectiveObjective,
	typename Observer
>
class simulation
{
public:
	typedef Scenario scenario_t;
	typedef GeneticMapping genetic_mapping_t;
	typedef TrialData trial_data_t;
	typedef ResultantObjective resultant_obj_fn_t;
	typedef CollectiveObjective collective_obj_val_fn_t;
	typedef Observer observer_t;
	
/*	struct trial_data_t
	{
		// was agent_state
		typename scenario_t::state	initial_st;
		typename scenario_t::state	final_st;

		// TODO: Dependent on template params as to whether is needed...????
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};
	*/

	//typedef basic_observer											observer_t;
	typedef std::vector< typename observer_t::per_trial_data >		observation_data_t;
/*
	typedef mpl::vector<
		typename problem_t::slowdown_obj_fn,
		typename problem_t::minimize_resultant_force_obj_fn
	> obj_fns_tl;
	typedef pareto_multiple_objective< trial_data_t, obj_fns_tl >	resultant_obj_fn_t;

	typedef single_objective< trial_data_t,
		try_to_win_obj_fn >
		//max_valid_moves_obj_fn >
																	resultant_obj_fn_t;
*/

	typedef typename resultant_obj_fn_t::obj_value_t				raw_obj_val_t;
	// TODO: resultant should not have ofdata_t - just provides typelist of dependencies, as does observer_t
	// then need to combine and form ofdata_t
	typedef typename resultant_obj_fn_t::ofdata_t					ofdata_t;

//	typedef coll_obj_val_identity< raw_obj_val_t >
//	typedef coll_obj_val_pareto_ranking< raw_obj_val_t >
//		collective_obj_val_fn_t;

	typedef typename collective_obj_val_fn_t::processed_obj_val_t	processed_obj_val_t;
	typedef 
		ranking_fitness< processed_obj_val_t >
		//direct_fitness< processed_obj_val_t >
																	fitness_assignment_fn_t;
	typedef typename fitness_assignment_fn_t::fitness_t				fitness_t;
	
//	typedef nac::default_fixednn_solution< typename scenario_t::system_t, typename scenario_t::solution_input > solution_t;
//	typedef fixed_neural_net< typename scenario_t::system_t, solution_t >								genetic_mapping_t;
	typedef typename genetic_mapping_t::genome						genome_t;
	typedef typename genetic_mapping_t::genome_diff_t				genome_diff_t;

	// TODO: potentially may want let the genetic_mapping specify this type, along with a method to 
	// calculate it from a given population, or population genome_diff_t values.
	typedef genome_diff_t											genotype_diversity_measure_t;
	
	typedef individual< genome_t, processed_obj_val_t, fitness_t >	individual_t;
	typedef std::vector< individual_t >								population_t;

	typedef std::vector< fitness_t >
		fitness_acc_t;
	typedef typename fitness_assignment_fn_t::template ProcessedAccessorDefn< fitness_acc_t >::processed_obj_val_acc_t
		processed_obj_val_acc_t;
	typedef typename collective_obj_val_fn_t::template RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t
		raw_obj_val_acc_t;


	typedef typename typename scenario_t::system_t::solution_result agent_decision_t;
	typedef agent_decision_t constraint_vars_t;
	
	enum {
		DefaultPopulationSize =
#ifdef _DEBUG
		5
#else
		20
#endif
		,
		
		DefaultTrialsPerEpoch = 1,
	};

	//boost::shared_ptr< thruster_config_t >				t_cfg;
	
	typedef typename genetic_mapping_t::solution			agent_t;

	typedef truncation_survival_selection< typename population_t::iterator >		survival_sel_t;

	typedef //roulette_procreation_selection< fitness_t >
		//random_procreation_selection<>
		equal_procreation_selection<>		proc_sel_t;
	
	typedef basic_crossover< genome_t >					crossover_t;
	typedef LinearCrossoverRate							crossover_rate_t;
	typedef basic_real_mutation< genome_t >				mutation_t;
	typedef LinearMutationRate							mutation_rate_t;

	proc_sel_t			proc_sel;
	crossover_t			cx_ftr;
	crossover_rate_t	cx_rate_ftr;
	mutation_t			mut_ftr;
	mutation_rate_t		mut_rate_ftr;
	
	typedef gen_alg<
		//		genome_t,
		//		fitness_t,
		individual_t,
		survival_sel_t,
		crossover_t,
		mutation_t,
		crossover_rate_t,
		mutation_rate_t,
		proc_sel_t
	> ga_t;
	
	ga_t ga;

	// TODO: Where should this go? Probably eventually want to make compile time based also.
	typedef hard_constraint< constraint_vars_t > hard_constraint_t;

	std::vector< boost::shared_ptr< hard_constraint_t > > hard_constraints;
	//

	size_t population_size;
	size_t trials_per_epoch;

	boost::random::mt19937	rgen;
	unsigned int			ga_epoch_rseed;
	unsigned int			trials_rseed;

public:
	simulation(size_t pop_size = DefaultPopulationSize, size_t trials = DefaultTrialsPerEpoch):
		population_size(pop_size),
		trials_per_epoch(trials),
		proc_sel(rgen),
		cx_ftr(0, rgen),
		cx_rate_ftr(0.5, 0.8),
		mut_ftr(rgen),
		mut_rate_ftr(0.25, 0.05),
		ga(proc_sel, cx_ftr, cx_rate_ftr, mut_ftr, mut_rate_ftr, rgen),
		ga_epoch_rseed(0),
		trials_rseed(0)
	{}

public:
/*	void initialise_nn_from_genome(
		FANN::neural_net& nn,
		genome_t const& gn) const
	{
		size_t const NumWeights = gn.length();

		std::vector< FANN::connection > conns(NumWeights);
		nn.get_connection_array(&conns[0]);
		for(size_t c = 0; c < NumWeights; ++c)
		{
			conns[c].weight = gn[c];
		}
		nn.set_weight_array(&conns[0], NumWeights);
	}
*/
	template < typename Iterator >
	void initialise_agents_from_genomes(Iterator agent_it)
	{
		for(size_t i = 0; i < population_size; ++i, ++agent_it)
		{
			//initialise_nn_from_genome(agent_it->nn, ga.population[i].genome);
			*agent_it = genetic_mapping_t::decode_genome(ga.population[i].genome);
		}
	}

	void init(size_t max_gens)
	{
		//rgen.seed();
		rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		
		ga_epoch_rseed = boost::random::uniform_int_distribution<>()(rgen);
		trials_rseed = boost::random::uniform_int_distribution<>()(rgen);

		// Set up thrusters at corners of square
		//t_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(thruster_presets::square_minimal()));	// TODO: Need way of specifying, probably through template param

		//size_t const num_thrusters = t_cfg->num_thrusters();

		// GA specific initialisation
		size_t const GenomeLength = genetic_mapping_t::get_genome_length();
			//agent_t::get_genome_length(num_thrusters);

		// Init genetic algorithm class
		//cx_rate_ftr.rate = 0.7;
		//mut_rate_ftr.rate = 0.15;

		cx_ftr.genome_length = GenomeLength;

		ga.init_params(GenomeLength, max_gens);

		ga.init_random_population(population_size,
			[](typename ga_t::rgen_t& _rgen){
			return genetic_mapping_t::generate_random_genome(_rgen);
		});

		/*/
		dim_traits::position_t c_of_m = zero_val< dim_traits::position_t >();	// TODO: Sync with somewhere - where is this specified??
		hard_constraints.push_back(boost::shared_ptr< hard_constraint_t >(
			new opposed_thrusters_constraint< dim >(*t_cfg, c_of_m)));
		hard_constraints.push_back(boost::shared_ptr< hard_constraint_t >(
			new no_force_constraint< dim >(*t_cfg, c_of_m)));
		/*/
	}

	template < typename Accessor >
	static std::string analyse_population_objective_values(Accessor& obj_vals, size_t const count)
	{
		double average = 0.0, best = -std::numeric_limits< double >::max();
		for(size_t i = 0; i < count; ++i)
		{
			double v = obj_vals[i];
			average += v;
			if(v > best)
			{
				best = v;
			}
		}
		average /= count;

		std::stringstream ss;
		std::fixed(ss);
		ss.precision(4);

		ss << "Average = " << average << "; Best = " << best;
		return ss.str();
	}

	genotype_diversity_measure_t population_genotype_diversity() const
	{
		genotype_diversity_measure_t diversity = 0.0;
		for(size_t i = 0; i < population_size - 1; ++i)
		{
			for(size_t j = i + 1; j < population_size; ++j)
			{
				genome_diff_t diff = genetic_mapping_t::genome_difference(ga.population[i].genome, ga.population[j].genome);
				diversity += diff;
			}
		}
		return diversity / (population_size * (population_size - 1));
	}

	void run_epoch(observation_data_t& observations, std::ostream& os)
	{
		//rgen.seed(ga_epoch_rseed + ga.generation);
		rgen.seed(ga_epoch_rseed);

		// TODO: Maybe better for these distribution objects not to persist across epochs...
//		cx_ftr.rdist.reset();
		mut_ftr.rdist_clamped.reset();
		mut_ftr.rdist_01.reset();
//		ga.rdist_clamped.reset();
//		ga.rdist_pve_clamped.reset();

		if(ga.generation > 0)
		{
			// Create next generation
			size_t num_valid = ga.epoch();

#if 0
			os << "Solution population validity: " << (100.0 * num_valid / population_size) << std::endl;
#endif
		}
		else
		{
			++ga.generation;
		}

		// Recreate the ga rand seed so that ga code will get different random numbers in next generation
		ga_epoch_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);

		//rgen.seed(trials_rseed + StaticTrainingSet ? 0 : ga.generation); this doesn't seem to work
		rgen.seed(trials_rseed);

		// Construct the decision making agents, then initialise their parameters by decoding the population genomes
		//size_t const num_thrusters = t_cfg->num_thrusters();
		std::vector< agent_t > agents(population_size, agent_t());
		initialise_agents_from_genomes(agents.begin());

		// TODO: This needs to be handled by template related to fitness type, as can't necessarily take average.
		// For Pareto, maybe something along lines of getting Pareto non-dominated fitnesses w.r.t. other trials of same agent??
		fitness_acc_t avg_fitnesses(population_size);
		//std::vector< processed_obj_val_t > avg_obj_vals(population_size, processed_obj_val_t(0.0));
		processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, population_size);
		for(size_t i = 0; i < population_size; ++i)
		{
			avg_obj_vals[i] = processed_obj_val_t(0.0);
		}
		std::vector< bool > failed_hard_constraint(population_size, false);

		// Initialize observations
		size_t observation_idx = observations.size();
		observations.resize(observations.size() + trials_per_epoch * population_size);

		// TODO: Will need to be modified to support systems will multiple agents interacting simultaneously
		// Run the trials
		for(size_t iinst = 0; iinst < trials_per_epoch; ++iinst)
		{
			// TODO: This is a hack, since we're not actually gonna calculate fitness until later anyway...
			fitness_acc_t fitnesses(population_size);

			processed_obj_val_acc_t processed_obj_val_acc(fitnesses, population_size);
			raw_obj_val_acc_t raw_obj_val_acc(processed_obj_val_acc, population_size);

			// Initialise the system
			scenario_t::state trial_initial_st;
			scenario_t::init_state(1, trial_initial_st, rgen, std::pair< size_t, size_t >(iinst, trials_per_epoch));	// TODO: assuming single agent system for now

			for(size_t iInd = 0; iInd < population_size; ++iInd)
			{
				// Use the same initial state for every agent's simulation on this trial
				scenario_t::state st = trial_initial_st;
				
				size_t const iAgent = 0;

				trial_data_t trial_data;
				trial_data.initial_st = st;//st.agents[iAgent];
				ofdata_t of_data;

				// Update step loop
				bool update_result = true;
				while(update_result)
				{
					// Make agent decision
					agent_t /*const*/ & a = agents[iInd];
					agent_decision_t agent_decision = a(st.get_sensor_state(iAgent));

					// Update the state of the environment and all agents
					typename scenario_t::system_update_params sup;
					scenario_t::register_solution_decision(agent_decision, sup);
					update_result = scenario_t::update(st, sup);

					// Allow fitness guaging criteria to be updated and check constraints
					of_data.update(agent_decision, st.agents[iAgent], st);

					if(!failed_hard_constraint[iInd])
					{
						for(auto const& con : hard_constraints)
						{
							if(!con->test_satisfied(agent_decision))
							{
								failed_hard_constraint[iInd] = true;
								break;
							}
						}
					}
				}

				trial_data.final_st = st;// st.agents[iAgent];

				// Evaluate agent objective function
				resultant_obj_fn_t obj_fn;
				of_data.finalise();
				raw_obj_val_acc[iInd] = obj_fn.evaluate(of_data, trial_data);

				// Store observations
				observations[observation_idx] = observer_t::record_observations(of_data, trial_data);
				++observation_idx;
			}
			
			// Invoke the collective objective value processing
			collective_obj_val_fn_t() (raw_obj_val_acc, population_size, processed_obj_val_acc);

			for(size_t iInd = 0; iInd < population_size; ++iInd)
			{
				avg_obj_vals[iInd] += processed_obj_val_acc[iInd];
			}
		}

		// TODO: See note above
		for(size_t iInd = 0; iInd < population_size; ++iInd)
		{
			avg_obj_vals[iInd] = avg_obj_vals[iInd] / (double)trials_per_epoch;
		}

		// TODO: temp resolution to issue below, but ideally don't want to require this copying
		std::vector< processed_obj_val_t > unordered_obj_vals(population_size);
		for(size_t i = 0; i < population_size; ++i)
		{
			unordered_obj_vals[i] = avg_obj_vals[i];
		}
		//

		// Finally assign population fitness based on processed values
		fitness_assignment_fn_t().reassign(avg_obj_vals, avg_fitnesses);

		for(size_t iInd = 0; iInd < population_size; ++iInd)
		{
			// TODO: !!!!!!!!!!!!!! THIS IS WRONG. avg_obj_vals has (potentially) been sorted by the above
			// fitness assignment function, so indices will be misaligned
			ga.population[iInd].obj_value = unordered_obj_vals[iInd];
				//avg_obj_vals[iInd];

			// This however is correct
			ga.population[iInd].fitness = avg_fitnesses[iInd];

			if(failed_hard_constraint[iInd])
			{
				ga.population[iInd].fitness.unfit = true;
			}
		}

		bool const StaticTrainingSet = false;	// Is training data (ie. system initial state) the same for every generation?
		// Regardless of this setting, data will be the same for all agents within a given generation.		}

		// If we don't want a static training set, recreate the trials rand seed 
		if(!StaticTrainingSet)
		{
			trials_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);
		}

		// TODO: TEMP
//		ga.sort_population_by_genome();
		//
//		ga.output_individuals(std::cout);

		// Analyse population fitness
		os << ("Epoch ") << ga.generation << ": ";
		//std::string fitness_output = fitness_t::analyse_population_fitness(ga.population.begin(), ga.population.end());
		//std::cout << fitness_output << std::endl;

		os << analyse_population_objective_values(avg_obj_vals, population_size) << std::endl;
	}

	processed_obj_val_t get_fittest(agent_t& a) const
	{
		processed_obj_val_t highest_ov = ga.population[0].obj_value;
		fitness_t highest_f = ga.population[0].fitness;
		size_t index = 0;
		for(size_t i = 1; i < population_size; ++i)
		{
			if(!ga.population[i].fitness.unfit)
			{
				processed_obj_val_t ov = ga.population[i].obj_value;
				fitness_t f = ga.population[i].fitness;

				// TODO: if we keep this representation, build unfit into fitness type ordering operators
				if(!f.unfit && highest_f.unfit || f.unfit == highest_f.unfit && ov > highest_ov)
				{
					highest_ov = ov;
					highest_f = f;
					index = i;
				}
			}
		}

		a = genetic_mapping_t::decode_genome(ga.population[index].genome);
		return highest_ov;
	}
};


#endif


