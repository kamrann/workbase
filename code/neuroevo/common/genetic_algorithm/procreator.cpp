// procreator.cpp

#include "procreator.h"
#include "genetic_population.h"

#include "ga/ga.h"
#include "ga/fitness.h"
#include "ga/roulette_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
#include "ga/single_objective.h"
#include "ga/pareto_multiple_objective.h"
#include "ga/collective_obj_val_ftr.h"
#include "ga/ranking_fitness.h"

#include <random>
#include <functional>


namespace ga {

	typedef std::default_random_engine rgen_t;

	typedef double														raw_obj_val_t;
	typedef coll_obj_val_identity< raw_obj_val_t >						collective_obj_val_fn_t;
	typedef collective_obj_val_fn_t::processed_obj_val_t				processed_obj_val_t;
	typedef	ranking_fitness< processed_obj_val_t >						fitness_assignment_fn_t;
	typedef fitness_assignment_fn_t::fitness_t							fitness_t;

	typedef std::function< void(std::vector< genome const* >::const_iterator parents, std::vector< genome >::iterator offspring) > crossover_t;
	typedef std::function< void(genome& child, double rate) > mutation_t;

	typedef individual< genome, double >								individual_t;
	typedef std::vector< individual_t >									population_t;
	typedef std::vector< fitness_t >									fitness_acc_t;
	typedef fitness_assignment_fn_t::ProcessedAccessorDefn< fitness_acc_t >::processed_obj_val_acc_t	processed_obj_val_acc_t;
	typedef collective_obj_val_fn_t::RawAccessorDefn< processed_obj_val_acc_t >::raw_obj_val_acc_t		raw_obj_val_acc_t;

	typedef truncation_survival_selection< population_t::iterator >		survival_sel_t;

	typedef equal_procreation_selection< rgen_t >						proc_sel_t;

	typedef gen_alg <
		individual_t,
		survival_sel_t,
		crossover_t,
		mutation_t,
		rectification_fn_t,
		crossover_rate_fn_t,
		mutation_rate_fn_t,
		rate_context,
		proc_sel_t
	> ga_t;


	procreator::procreator(rgen_t& rgen):
		m_rgen(rgen)
	{
		m_generation = 0;
	}

	mutation& procreator::mutation_fn()
	{
		return m_mut_fn;
	}

	crossover_fn_t& procreator::crossover_fn()
	{
		return m_cx_fn;
	}

	mutation_rate_fn_t& procreator::mutation_rate_fn()
	{
		return m_mut_rate_fn;
	}

	crossover_rate_fn_t& procreator::crossover_rate_fn()
	{
		return m_cx_rate_fn;
	}

	rectification_fn_t& procreator::rectification_fn()
	{
		return m_rect_fn;
	}

	void procreator::reset()
	{
		// TODO: think need to reset all distributions within various functors for determinism

		m_generation = 0;
	}

	genetic_population procreator::next_generation(
		genetic_population const& existing
		)
	{
		crossover_t cx_ftr = [this](std::vector< genome const* >::const_iterator parents, std::vector< genome >::iterator offspring)
		{
			auto p1 = *(parents + 0);
			auto p2 = *(parents + 1);
			*offspring = m_cx_fn(*p1, *p2, m_rgen);
		};
		mutation_t mut_ftr = [this](genome& child, double rate)
		{
			m_mut_fn.op(child, rate, m_rgen);
		};

		m_mut_fn.update(op_update_context{ m_generation });

		proc_sel_t proc_sel{ m_rgen };
		rate_context rate_ctx;
		rate_ctx.generation = m_generation;
		ga_t ga(proc_sel, cx_ftr, m_cx_rate_fn, mut_ftr, m_mut_rate_fn, m_rect_fn, rate_ctx, m_rgen);
		ga.init_params(0 /* unused */, 1 /* TODO: ? */);
		ga.generation = m_generation;// TODO: still used?
		auto const pop_size = existing.size();
		ga.population.resize(pop_size);

		// Produce next generation
		for(size_t idv = 0; idv < pop_size; ++idv)
		{
			ga.population[idv].genome = existing[idv].gn;
			ga.population[idv].fitness = existing[idv].fitness;
		}

		ga.epoch();

		genetic_population next;
		next.resize(pop_size);
		for(size_t idv = 0; idv < pop_size; ++idv)
		{
			next[idv].gn = std::move(ga.population[idv].genome);
		}

		++m_generation;

		return std::move(next);
	}

}




