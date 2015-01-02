// genalg.cpp

#include "genalg.h"
#include "problem_domain.h"
#include "fitness_assignment.h"

#include <boost/optional.hpp>


namespace ga {

	genalg::genalg(
		i_problem_domain const& domain):
		initial_pop_size_(0),
		proc_(rgen_),
		domain_(domain)
	{
		proc_.crossover_fn() = domain_.crossover_op();
		proc_.mutation_fn() = domain_.mutation_op();
		proc_.rectification_fn() = [this](ga::genome& gn){ return domain_.rectify_genome(gn); };
	}

	void genalg::set_initial_population_size(size_t sz)
	{
		initial_pop_size_ = sz;
	}

	void genalg::set_crossover_rate_fn(crossover_rate_fn_t fn)
	{
		proc_.crossover_rate_fn() = fn;
	}

	void genalg::set_mutation_rate_fn(mutation_rate_fn_t fn)
	{
		proc_.mutation_rate_fn() = fn;
	}

	void genalg::set_trials_configuration(trials_cfg cfg)
	{
		trials_ = cfg;
	}

	void genalg::set_fitness_assignment(fitness_assignment_fn_t fn)
	{
		fitness_assign_ = fn;
	}

	void genalg::reset(/*size_t initial_pop_size,*/ unsigned int rseed)
	{
		rgen_.seed(rseed);

		// Initialize first generation
		pop_.resize(initial_pop_size_);
		for(auto& idv : pop_)
		{
			idv.gn = domain_.create_random_genome(rgen_);
		}

		proc_.reset();
	}

	void genalg::epoch()
	{
		// Create next generation
		auto next = proc_.next_generation(pop_);
		pop_ = std::move(next);

		// TODO: This is weird
		auto seeder = std::uniform_int_distribution < unsigned long > {};
		auto next_seed = seeder(rgen_);

		// Evaluate individuals
		std::vector< std::vector< objective_value > > trial_obj_vals{ pop_.size(), {} };
//		for(auto& idv : pop_)
		for(size_t i = 0; i < pop_.size(); ++i)
		{
			rgen_.seed(next_seed);	// Same seed for every individual - todo: currently this means NOT a static training set
			trial_obj_vals[i] = domain_.evaluate_genome(pop_[i].gn, rgen_, trials_.num_trials);
//			idv.obj_val = domain_.evaluate_genome(idv.gn);
		}

		rgen_.seed(next_seed);

		/* TODO: Assuming not pareto. For pareto, need to mix in all idv trial results, keeping a tag to the individual
		and then do rank/domination calculation. Can then use avg/worst on the resulting rankings */
		for(size_t i = 0; i < pop_.size(); ++i)
		{
			switch(trials_.merging)
			{
				case trials_cfg::MergeMethod::Average:
				{
					double val = 0.0;
					for(auto const& tres : trial_obj_vals[i])
					{
						val += tres.as_real();
					}
					pop_[i].obj_val = objective_value{ val / trials_.num_trials };
				}
				break;

				case trials_cfg::MergeMethod::Worst:
				{
					boost::optional< double > val;
					for(auto const& tres : trial_obj_vals[i])
					{
						if(!val || tres.as_real() < *val)
						{
							val = tres.as_real();
						}
					}
					pop_[i].obj_val = objective_value{ *val };
				}
				break;
			}
		}

		fitness_assign_(pop_);

		// TODO: May not necessarily want to always sort here.
		// At any rate if we do, then shoudn't do again within ga::epoch (eg. survival selection)
		std::sort(std::begin(pop_), std::end(pop_), genetic_population::individual::fitness_compare{});
	}

	size_t genalg::generation() const
	{
		return proc_.m_generation;
	}

	genetic_population const& genalg::population() const
	{
		return pop_;
	}

	i_problem_domain const& genalg::domain() const
	{
		return domain_;
	}

}




