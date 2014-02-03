// sim.h

#ifndef __SHIP_SIM_H
#define __SHIP_SIM_H

#include "thrusters/thruster.h"
#include "thrusters/thruster_presets.h"

#include "ga/ga.h"
#include "ga/genome.h"
#include "ga/fitness.h"
#include "ga/roulette_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
// TODO: Ideally won't need these and will supply desired types/ftrs as template parameters
#include "ga/single_objective.h"
#include "ga/pareto_multiple_objective.h"
#include "ga/collective_obj_val_ftr.h"
#include "ga/ranking_fitness.h"
#include "ga/hard_constraint.h"
#include "ga/shared_constraints.h"

#include "doublefann.h"
#include "fann_cpp.h"

#include <Eigen/StdVector>

#include <boost/random/mersenne_twister.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/smart_ptr.hpp>

#include <chrono>


template < typename ProblemType >
class simulation
{
public:
	typedef ProblemType problem_t;
	static const WorldDimensionality dim = problem_t::dim;
	typedef typename problem_t::dim_traits dim_traits;

	typedef thruster_config< dim > thruster_config_t;
	
	struct trial_data_t
	{
		typename problem_t::agent_state		initial_st;
		typename problem_t::agent_state		final_st;

		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};

	//typedef typename problem_t::template slowdown_fitness_fn< trial_data_t > fitness_fn;
/*
	typedef typename problem_t::slowdown_obj_fn						obj_val_fn_t;
	typedef single_objective< trial_data_t, obj_val_fn_t >			resultant_obj_fn_t;
*/
	typedef mpl::vector<
		typename problem_t::slowdown_obj_fn,
		typename problem_t::minimize_resultant_force_obj_fn
	> obj_fns_tl;
	typedef pareto_multiple_objective< trial_data_t, obj_fns_tl >	resultant_obj_fn_t;

	typedef typename resultant_obj_fn_t::obj_value_t				raw_obj_val_t;
	typedef typename resultant_obj_fn_t::ofdata_t					ofdata_t;
/*
	typedef coll_obj_val_identity< raw_obj_val_t >					collective_obj_val_fn_t;
*/
	typedef coll_obj_val_pareto_ranking< raw_obj_val_t >			collective_obj_val_fn_t;

	typedef typename collective_obj_val_fn_t::processed_obj_val_t	processed_obj_val_t;
	typedef ranking_fitness< processed_obj_val_t >					fitness_assignment_fn_t;
	typedef typename fitness_assignment_fn_t::fitness_t				fitness_t;
	typedef fixed_real_genome										genome_t;
	typedef individual< genome_t, processed_obj_val_t, fitness_t >	individual_t;
	typedef std::vector< individual_t >								population_t;

	typedef std::vector< fitness_t >
		fitness_acc_t;
	typedef typename fitness_assignment_fn_t::template ProcessedAccessorDefn< fitness_acc_t >::processed_obj_val_acc_t
		processed_obj_val_acc_t;
	typedef typename collective_obj_val_fn_t::template RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t
		raw_obj_val_acc_t;


	typedef typename problem_t::agent_decision constraint_vars_t;
	
	enum {
		PopulationSize = 40,
		TrialsPerEpoch = 25,
	};

	boost::shared_ptr< thruster_config_t >				t_cfg;
	
	typedef typename problem_t::template nn_agent_impl< 2, 0 >	agent_t;

	//typedef basic_real_fitness							fitness_t;

	//typedef std::vector< individual< genome_t, fitness_t > >			population_t;
	typedef truncation_survival_selection< typename population_t::iterator >		survival_sel_t;

	typedef //roulette_procreation_selection< fitness_t >
		//random_procreation_selection<>
		equal_procreation_selection<>		proc_sel_t;
	
	typedef basic_crossover< genome_t >					crossover_t;
	typedef FixedCrossoverRate							crossover_rate_t;
	typedef basic_real_mutation< genome_t >				mutation_t;
	typedef FixedMutationRate							mutation_rate_t;

	proc_sel_t			proc_sel;
	crossover_t			cx_ftr;
	crossover_rate_t	cx_rate_ftr;
	mutation_t			mut_ftr;
	mutation_rate_t		mut_rate_ftr;
	
	gen_alg<
//		genome_t,
//		fitness_t,
		individual_t,
		survival_sel_t,
		crossover_t,
		mutation_t,
		crossover_rate_t,
		mutation_rate_t,
		proc_sel_t
	> ga;

	// TODO: Where should this go? Probably eventually want to make compile time based also.
	typedef hard_constraint< constraint_vars_t > hard_constraint_t;

	std::vector< boost::shared_ptr< hard_constraint_t > > hard_constraints;
	//

	boost::random::mt19937	rgen;
	unsigned int			ga_epoch_rseed;
	unsigned int			trials_rseed;

public:
	simulation():
		proc_sel(rgen),
		cx_ftr(0, rgen),
		cx_rate_ftr(),
		mut_ftr(rgen),
		mut_rate_ftr(),
		ga(proc_sel, cx_ftr, cx_rate_ftr, mut_ftr, mut_rate_ftr, rgen),
		ga_epoch_rseed(0),
		trials_rseed(0)
	{}

public:
	void initialise_nn_from_genome(
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

	template < typename Iterator >
	void initialise_agents_from_genomes(Iterator agent_it)
	{
		for(size_t i = 0; i < PopulationSize; ++i, ++agent_it)
		{
			initialise_nn_from_genome(agent_it->nn, ga.population[i].genome);
		}
	}

	void init()
	{
		//rgen.seed();
		rgen.seed(static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
		
		ga_epoch_rseed = boost::random::uniform_int_distribution<>()(rgen);
		trials_rseed = boost::random::uniform_int_distribution<>()(rgen);

		// Set up thrusters at corners of square
		t_cfg = boost::shared_ptr< thruster_config< dim > >(new thruster_config< dim >(thruster_presets::square_minimal()));	// TODO: Need way of specifying, probably through template param

		size_t const num_thrusters = t_cfg->num_thrusters();

		// GA specific initialisation
		size_t const NumWeights = agent_t::get_genome_length(num_thrusters);

		// Init genetic algorithm class
		cx_rate_ftr.rate = 0.7;
		mut_rate_ftr.rate = 0.15;

		cx_ftr.genome_length = NumWeights;

		ga.init_params(NumWeights);//, 1000); // TODO: max gens hard code copied from num epochs in fanntest.cpp... //0.7, 0.15);
		ga.init_random_population(PopulationSize);

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

	void run_epoch(std::ostream& os)
	{
		rgen.seed(//static_cast< uint32_t >(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff));
			ga_epoch_rseed + ga.generation);
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
			os << "Solution population validity: " << (100.0 * num_valid / PopulationSize) << std::endl;
#endif
		}
		else
		{
			++ga.generation;
		}

		rgen.seed(trials_rseed);

		// Construct the decision making agents, then initialise their parameters by decoding the population genomes
		size_t const num_thrusters = t_cfg->num_thrusters();
		std::vector< agent_t > agents(PopulationSize, agent_t(num_thrusters));
		initialise_agents_from_genomes(agents.begin());

		// TODO: This needs to be handled by template related to fitness type, as can't necessarily take average.
		// For Pareto, maybe something along lines of getting Pareto non-dominated fitnesses w.r.t. other trials of same agent??
		fitness_acc_t avg_fitnesses(PopulationSize);
		//std::vector< processed_obj_val_t > avg_obj_vals(PopulationSize, processed_obj_val_t(0.0));
		processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, PopulationSize);
		for(size_t i = 0; i < PopulationSize; ++i)
		{
			avg_obj_vals[i] = processed_obj_val_t(0.0);
		}
		std::vector< bool > failed_hard_constraint(PopulationSize, false);

		// Run the trials
		for(size_t iinst = 0; iinst < TrialsPerEpoch; ++iinst)
		{
			// Initialise the environment
			problem_t::state st;
			problem_t::init_state(t_cfg, PopulationSize, st, rgen);

			std::vector< trial_data_t, Eigen::aligned_allocator< trial_data_t > > trial_data(PopulationSize);
			std::vector< ofdata_t > of_data(PopulationSize);

			for(size_t i = 0; i < PopulationSize; ++i)
			{
				trial_data[i].initial_st = st.agents[i];
			}

			// Update step loop
			for(size_t itime = 0; itime < 100; ++itime)	// TODO: Timestep limit???
			{
				std::vector< problem_t::agent_decision > agent_decisions;
				//for(agent_t const& a: agents)
				for(size_t iagent = 0; iagent < PopulationSize; ++iagent)
				{
					// Make agent decision
					agent_t /*const*/ & a = agents[iagent];
					agent_decisions.push_back(a(st.get_sensor_state(iagent)));
				}

				// Update the state of the environment and all agents
				problem_t::update_state(st, &agent_decisions[0]);

				// Allow fitness guaging criteria to be updated and check constraints
				for(size_t iagent = 0; iagent < PopulationSize; ++iagent)
				{
					of_data[iagent].update(agent_decisions[iagent], st.agents[iagent], st);

					if(!failed_hard_constraint[iagent])
					{
						for(auto const& con: hard_constraints)
						{
							if(!con->test_satisfied(agent_decisions[iagent]))
							{
								failed_hard_constraint[iagent] = true;
								break;
							}
						}
					}
				}
			}

			for(size_t i = 0; i < PopulationSize; ++i)
			{
				trial_data[i].final_st = st.agents[i];
			}

			// TODO: This is a hack, since we're not actually gonna calculate fitness until later anyway...
			fitness_acc_t fitnesses(PopulationSize);

			processed_obj_val_acc_t processed_obj_val_acc(fitnesses, PopulationSize);
			raw_obj_val_acc_t raw_obj_val_acc(processed_obj_val_acc, PopulationSize);

			// Evaluate agent objective function
			resultant_obj_fn_t obj_fn;
			for(size_t i = 0; i < PopulationSize; ++i)
			{
				of_data[i].finalise();

				raw_obj_val_acc[i] = obj_fn.evaluate(of_data[i], trial_data[i]);
			}

			// Invoke the collective objective value processing
			collective_obj_val_fn_t() (raw_obj_val_acc, PopulationSize, processed_obj_val_acc);

			for(size_t i = 0; i < PopulationSize; ++i)
			{
				avg_obj_vals[i] += processed_obj_val_acc[i];
			}
		}

		// TODO: See note above
		for(size_t i = 0; i < PopulationSize; ++i)
		{
			avg_obj_vals[i] = avg_obj_vals[i] / (double)TrialsPerEpoch;
		}

		// Finally assign population fitness based on processed values
		fitness_assignment_fn_t().reassign(avg_obj_vals, avg_fitnesses);

		for(size_t i = 0; i < PopulationSize; ++i)
		{
			ga.population[i].obj_value = avg_obj_vals[i];
			ga.population[i].fitness = avg_fitnesses[i];

			if(failed_hard_constraint[i])
			{
				ga.population[i].fitness.unfit = true;
			}
		}


		// TODO: TEMP
//		ga.sort_population_by_genome();
		//
//		ga.output_individuals(std::cout);

		// Analyse population fitness
		os << ("Epoch ") << ga.generation << ": ";
		//std::string fitness_output = fitness_t::analyse_population_fitness(ga.population.begin(), ga.population.end());
		//std::cout << fitness_output << std::endl;

		os << analyse_population_objective_values(avg_obj_vals, PopulationSize) << std::endl;
	}

	processed_obj_val_t get_fittest(FANN::neural_net& nn) const
	{
		processed_obj_val_t highest_ov = ga.population[0].obj_value;
		fitness_t highest_f = ga.population[0].fitness;
		size_t index = 0;
		for(size_t i = 1; i < PopulationSize; ++i)
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

		agent_t a(t_cfg->num_thrusters());
		initialise_nn_from_genome(a.nn, ga.population[index].genome);
		nn = a.nn;

		return highest_ov;
	}
};


#endif


