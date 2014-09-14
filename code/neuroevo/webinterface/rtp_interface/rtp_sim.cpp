// rtp_sim.cpp

#include "rtp_sim.h"
#include "systems/rtp_system.h"
#include "systems/rtp_controller.h"
#include "rtp_observer.h"
#include "rtp_pop_wide_observer.h"
#include "rtp_genome.h"
#include "rtp_genome_mapping.h"
#include "rtp_procreation_selection.h"

#include "wt_param_widgets/pw_yaml.h"
#include "wt_param_widgets/schema_builder.h"
#include "wt_param_widgets/param_accessor.h"

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
	struct convert< rtp::rtp_procreation_selection::Type >
	{
		static Node encode(rtp::rtp_procreation_selection::Type const& rhs)
		{
			return Node(rtp::rtp_procreation_selection::Names[rhs]);
		}

		static bool decode(Node const& node, rtp::rtp_procreation_selection::Type& rhs)
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

		static std::map< std::string, rtp::rtp_procreation_selection::Type > const mapping_;
	};

	std::map< std::string, rtp::rtp_procreation_selection::Type > const convert< rtp::rtp_procreation_selection::Type >::mapping_ = {
			{ "Random", rtp::rtp_procreation_selection::Type::Random },
			{ "Equal", rtp::rtp_procreation_selection::Type::Equal },
//		{ "Roulette", rtp_procreation_selection::Type::Roulette },
	};
}


namespace rtp {

	namespace sb = prm::schema;

	prm::schema::schema_node rtp_simulation::get_evo_schema(prm::param_accessor param_vals)
	{
		auto schema = sb::list("evo_params");
		sb::label(schema, "Evolutionary Params");

		sb::append(schema, sb::integer("pop_size", 10, 1));
		sb::label(sb::last(schema), "Population Size");
		sb::append(schema, sb::integer("num_generations", 10, 1));
		sb::label(sb::last(schema), "Num Generations");
		sb::append(schema, sb::integer("num_trials", 1, 1));
		sb::label(sb::last(schema), "Trials/Generation");
		sb::append(schema, sb::integer("rand_seed", 0, 0));
		sb::label(sb::last(schema), "Random Seed");
		sb::append(schema, sb::enum_selection("procreation_type", std::vector < std::string > { begin(rtp_procreation_selection::Names), end(rtp_procreation_selection::Names) }));
		sb::label(sb::last(schema), "Procreation");

		return schema;
	}

	std::string rtp_simulation::update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix)
	{
		auto relative = std::string{ "evo_params" };
		auto path = prefix + relative;

		(*provider)[path + std::string("pop_size")] = [](prm::param_accessor)
		{
			auto s = sb::integer("pop_size", 10, 1);
			sb::label(s, "Population Size");
			return s;
		};

		(*provider)[path + std::string("num_generations")] = [](prm::param_accessor)
		{
			auto s = sb::integer("num_generations", 10, 1);
			sb::label(s, "Num Generations");
			return s;
		};

		(*provider)[path + std::string("num_trials")] = [](prm::param_accessor)
		{
			auto s = sb::integer("num_trials", 1, 1);
			sb::label(s, "Trials/Generation");
			return s;
		};

		(*provider)[path + std::string("rand_seed")] = [](prm::param_accessor)
		{
			auto s = sb::integer("rand_seed", 0, 0);
			sb::label(s, "Random Seed");
			return s;
		};

		(*provider)[path + std::string("procreation_type")] = [](prm::param_accessor)
		{
			auto s = sb::enum_selection(
				"procreation_type",
				std::vector < std::string > {
				begin(rtp_procreation_selection::Names),
					end(rtp_procreation_selection::Names)
			});
			sb::label(s, "Procreation");
			return s;
		};


		path += std::string{ "debug_options" };

		(*provider)[path + std::string{ "enable_procreation" }] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::boolean("enable_procreation", true);
			sb::label(s, "Enable Procreation");
			return s;
		};

		(*provider)[path + std::string{ "enable_trials" }] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::boolean("enable_trials", true);
			sb::label(s, "Enable System Trials");
			return s;
		};

		(*provider)[path + std::string{ "save_to_db" }] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::boolean("save_to_db", true);
			sb::label(s, "Save to Database");
			return s;
		};

		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list("debug_options");
			sb::append(s, provider->at(path + std::string("enable_procreation"))(param_vals));
			sb::append(s, provider->at(path + std::string("enable_trials"))(param_vals));
			sb::append(s, provider->at(path + std::string("save_to_db"))(param_vals));
			sb::label(s, "Debug Options");
			return s;
		};

		path.pop();


		(*provider)[path] = [=](prm::param_accessor param_vals)
		{
			auto s = sb::list(relative);
			sb::append(s, provider->at(path + std::string("pop_size"))(param_vals));
			sb::append(s, provider->at(path + std::string("num_generations"))(param_vals));
			sb::append(s, provider->at(path + std::string("num_trials"))(param_vals));
			sb::append(s, provider->at(path + std::string("rand_seed"))(param_vals));
			sb::append(s, provider->at(path + std::string("procreation_type"))(param_vals));
			sb::append(s, provider->at(path + std::string("debug_options"))(param_vals));
			sb::label(s, "Evolutionary Params");
			return s;
		};

		return relative;
	}

	rtp_simulation::rtp_simulation(prm::param_accessor sys_param, prm::param_accessor evo_param)
	{
		sys_factory = i_system::generate_system_factory(sys_param);
		agent_data = i_system::generate_agents_data(sys_param, true);

		// TODO: assuming only one evolvable controller
		m_evolvable_id = agent_data.evolved.begin()->first;
		//

		evo_param.push_relative_path(prm::qualified_path("evo_params"));

		population_size = evo_param["pop_size"].as< int >();
		total_generations = evo_param["num_generations"].as< int >();
		trials_per_generation = evo_param["num_trials"].as< int >();
		int seed = evo_param["rand_seed"].as< int >();	// TODO: uint version of param?
		if(seed == 0)
		{
			// For now, 0 represents random
			base_seed = static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count() & 0xffffffff);
		}
		else
		{
			base_seed = seed;
		}

		proc_sel = rtp_procreation_selection::create_instance(evo_param["procreation_type"][0].as< rtp_procreation_selection::Type >(), rgen);

		procreation_enabled = evo_param["enable_procreation"].as< bool >();
		trials_enabled = evo_param["enable_trials"].as< bool >();

		evo_param.pop_relative_path();
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
		return get_genome_mapping()->population_diversity(genomes);
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

	typedef gen_alg <
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
		std::cout << "Starting epoch " << (generation + 1) << std::endl;

		rgen.seed(ga_rseed);
		/* TODO:
		If end up having a persistent (across epochs) ga, will need to reset any rand distributions here.
		*/

		if(generation == 0)
		{
			// Init random population
			auto factory = get_evolved_controller_factory();

			population.resize(population_size);
			for(size_t idv = 0; idv < population_size; ++idv)
			{
				population[idv].gn = get_genome_mapping()->generate_random_genome(rgen);
				//			population[idv].idv.reset(factory->create());
			}
		}
		else if(procreation_enabled)
		{
			//proc_sel_t proc_sel(rgen);
			rtp_crossover_t cx_ftr = get_genome_mapping()->get_crossover_fn(rgen);
			//(gn_mapping->get_genome_length(), rgen);
			crossover_rate_t cx_rate_ftr(0.5, 0.8);
			rtp_mutation_t mut_ftr = get_genome_mapping()->get_mutation_fn(rgen);
			//(rgen);
			mutation_rate_t mut_rate_ftr(0.25, 0.05);
			ga_t ga(*proc_sel, cx_ftr, cx_rate_ftr, mut_ftr, mut_rate_ftr, rgen);
			ga.init_params(0 /* unused */, total_generations);
			ga.generation = generation;
			ga.population.resize(population_size);

			// Produce next generation
			for(size_t idv = 0; idv < population_size; ++idv)
			{
				ga.population[idv].genome = population[idv].gn;	// todo: this is cloning unnecessarily.
				// wtf is the deal with rtp_genome_wrap anyway?????
				ga.population[idv].fitness = population[idv].fitness;
			}

			std::cout << "Producing next generation..." << std::endl;
			ga.epoch();

			for(size_t idv = 0; idv < population_size; ++idv)
			{
				population[idv].gn = std::move(ga.population[idv].genome.get());
			}
		}

		// Recreate the ga rand seed so that ga code will get different random numbers in next generation
		ga_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);

		boost::optional< std::string > analysis = "";
		std::vector< uint32_t > trial_seeds(trials_per_generation);
		if(trials_enabled)
		{
			std::cout << "Commencing trials" << std::endl;

			// Seed for trials
			rgen.seed(trials_rseed);

			// Decode population genomes
			/*	for(size_t idv = 0; idv < population_size; ++idv)
				{
				get_genome_mapping()->decode_genome(population[idv].gn.get(), population[idv].idv.get());
				}
				*/

			/*	fitness_acc_t avg_fitnesses(population_size);
				processed_obj_val_acc_t avg_obj_vals(avg_fitnesses, population_size);
				for(size_t idv = 0; idv < population_size; ++idv)
				{
				avg_obj_vals[idv] = processed_obj_val_t(0.0);
				}
				*/
			i_population_wide_observer::eval_data_t obj_val_eval_data = get_resultant_objective()->initialize(population_size);

			i_system::perf_data_t perf_data;
			for(size_t trial = 0; trial < trials_per_generation; ++trial)
			{
				fitness_acc_t fitnesses(population_size);

				//processed_obj_val_acc_t processed_obj_val_acc(fitnesses, population_size);
				//raw_obj_val_acc_t raw_obj_val_acc(processed_obj_val_acc, population_size);
				std::vector< i_observer::observations_t > observations(population_size);

				// Ensure that the same random numbers are generated for all runs within a given trial
				boost::random::uniform_int_distribution< unsigned long > dist;
				trial_seeds[trial] = dist(rgen);
				//		boost::any initial_st = system->generate_initial_state(rgen);

				std::cout << "Starting trial #" << (trial + 1) << " [rseed=" << trial_seeds[trial] << "]..." << std::endl;

				for(size_t idv = 0; idv < population_size; ++idv)
				{
					//
					auto system = sys_factory->create_system();
					system->set_random_seed(trial_seeds[trial]);
					system->initialize();

					for(auto const& agent : agent_data.agents)
					{
						auto agent_id = system->register_agent(agent.spec);// , std::move(controller));

						auto controller = std::unique_ptr< i_controller >(
							agent_data.controller_factories[agent.controller_id]->create(&system->get_agent(agent_id))
							);
						if(agent.controller_id == m_evolvable_id)
						{
							// TODO:xx Should just do the decoding once per individual, not repeat every trial
							// Currently complicated slightly by needing an agent to construct the controller.
							// Ideally the core of a controller, which is all we are concerned with from the
							// evolving point of view, should be distinct from anything relating to providing an
							// inputs name/ids mapping. From this perspective, we don't care exactly what agent type
							// the controller will be attached to.
							get_genome_mapping()->decode_genome(population[idv].gn.get(), controller.get());
						}
						system->register_agent_controller(agent_id, std::move(controller));
					}
					//
					/*
								system->clear_agents();
								system->register_agent(population[idv].idv);
								system->set_state(initial_st);
								*/
					auto obs = get_observer();
					obs->reset();

					bool system_active = true;
					while(system_active)
					{
						system_active = system->update(obs);
					}

					// Store observations
					observations[idv] = system->record_observations(obs);

					system->concatenate_performance_data(perf_data);
				}

				// Invoke the collective objective value processing
				//collective_obj_val_fn_t() (raw_obj_val_acc, population_size, processed_obj_val_acc);
				get_resultant_objective()->register_datapoint(observations, obj_val_eval_data);

				/*		for(size_t idv = 0; idv < population_size; ++idv)
						{
						//avg_obj_vals[idv] += processed_obj_val_acc[idv];
						avg_obj_vals[idv] += boost::any_cast<double>(resultant_vals[idv]);
						}
						*/
				std::cout << "...trial completed." << std::endl;
			}

			/*	for(size_t idv = 0; idv < population_size; ++idv)
				{
				avg_obj_vals[idv] = avg_obj_vals[idv] / (double)trials_per_generation;
				}
				*/
			std::vector< boost::any > resultant_vals = get_resultant_objective()->evaluate(obj_val_eval_data, analysis);

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
			bool const StaticTrainingSet = true;
			// Regardless of this setting, data will be the same for all agents within a given generation.

			// If we don't want a static training set, recreate the trials rand seed 
			if(!StaticTrainingSet)
			{
				trials_rseed = boost::random::uniform_int_distribution< uint32_t >()(rgen);
			}


			i_system::output_performance_data(perf_data, std::cout);
		}

		++generation;

		os << "Gen " << generation << ": " << *analysis;
		os << " { ";
		for(auto const& s : trial_seeds)
		{
			os << s << " ";
		}
		os << "}";
		os << std::endl;
	}

	i_genome* rtp_simulation::get_individual_genome(size_t idx)
	{
		return nullptr;
	}
	/*
	boost::shared_ptr< i_controller > rtp_simulation::get_fittest() const
	{
	boost::optional< size_t > index;
	for(size_t idv = 0; idv < population_size; ++idv)
	{
	if(!index || population[idv].fitness > population[*index].fitness)
	{
	index = idv;
	}
	}

	// TODO: This sucks, see also note TODO:xx
	// Need to be able to create core controller without needing agent instance
	auto temp = agent_data.agents.front().spec
	auto controller = get_evolved_controller_factory()->create()
	return population[*index].idv;	// TODO: Should clone!
	}
	*/

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

}

