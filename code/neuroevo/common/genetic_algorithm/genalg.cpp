// genalg.cpp

#include "genalg.h"
#include "problem_domain.h"
#include "fitness_assignment.h"


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

		// Evaluate individuals
		for(auto& idv : pop_)
		{
			idv.obj_val = domain_.evaluate_genome(idv.gn);
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




