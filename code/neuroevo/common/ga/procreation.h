// procreation.h

#ifndef __PROCREATION_H
#define __PROCREATION_H

#include <boost/random/uniform_01.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_ptr.hpp>

#include <array>
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

/*
A procreation class must provide the following method to generate offspring from a parent population:

void generate_offspring(
	InputIterator parents_start,
	InputIterator parents_end,
	size_t num_offspring, 
	OutputIterator offspring_start)
*/

template <
	typename ProcreationSelection,
	typename CrossoverFtr,
	typename MutationFtr,
	typename RectificationFtr,
	typename CrossoverRateFtr,
	typename MutationRateFtr,
	typename RateContext,
	typename RGen = boost::random::mt19937
>
class basic_procreation
{
private:
	typedef ProcreationSelection	procreation_sel_t;
	// TODO: Build rates into cx and mut functors?
	typedef CrossoverRateFtr		crossover_rate_ftr_t;
	typedef CrossoverFtr			crossover_ftr_t;
	typedef MutationRateFtr			mutation_rate_ftr_t;
	typedef MutationFtr				mutation_ftr_t;
	typedef RectificationFtr		rectification_ftr_t;
	typedef RateContext				rate_context_t;

private:
	RGen&										rgen;

public:
	procreation_sel_t&		proc_sel;
	crossover_rate_ftr_t&	cx_rate_ftr;
	crossover_ftr_t&		cx_ftr;
	mutation_rate_ftr_t&	mut_rate_ftr;
	mutation_ftr_t&			mut_ftr;
	rectification_ftr_t&	rect_ftr;
	rate_context_t			rate_ctx;

public:
	basic_procreation(
		procreation_sel_t& _proc_sel,
		crossover_rate_ftr_t& _cx_rate,
		crossover_ftr_t& _cx,
		mutation_rate_ftr_t& _mut_rate,
		mutation_ftr_t& _mut,
		rectification_ftr_t& _rect,
		RGen& rg,
		rate_context_t _rate_ctx): proc_sel(_proc_sel), cx_rate_ftr(_cx_rate), cx_ftr(_cx), mut_rate_ftr(_mut_rate), mut_ftr(_mut), rect_ftr(_rect), rgen(rg), rate_ctx(_rate_ctx)
	{}

public:
	template < typename InputIterator, typename OutputIterator >
	void generate_offspring(InputIterator parents_start, InputIterator parents_end, size_t num_offspring, OutputIterator offspring)
	{
		// Size of the parent population
		size_t const num_parents_total = parents_end - parents_start;

		// Get the number of parents needed for a single procreation
		size_t const num_parents_cx = 2; // TODO: rtp cx fn cannot be simple boost function if we want this reenabled...
			// Will need to create wrapper for it with op() and also .get_num_parents()
			//cx_ftr.get_num_parents();
		bool const can_crossover = num_parents_total >= num_parents_cx;

		typedef typename OutputIterator::value_type genome_t;// individual_t;

		proc_sel.preprocess_population(parents_start, parents_end);

		boost::random::uniform_01<>	rdist_0_1;
		while(num_offspring-- > 0)
		{
			// First determine whether or not to create the child through crossover
			if(can_crossover && rdist_0_1(rgen) < cx_rate_ftr(rate_ctx, rgen))
			{
				// Select the parents for the crossover operation
				std::vector< genome_t const* > parents(num_parents_cx, nullptr);
				proc_sel.select_parents(num_parents_cx, parents_start, parents_end, parents.begin());

				// Generate the child through crossover
				cx_ftr(parents.begin(), offspring);
			}
			else
			{
				// Not using crossover, so select a single individual from the parent population
				std::vector< genome_t const* > parent(1, nullptr);
				proc_sel.select_parents(1, parents_start, parents_end, parent.begin());

				// And clone it
				*offspring = *parent[0];
			}

			// Now apply mutation
			double mutation_rate = mut_rate_ftr(rate_ctx, rgen);
			mut_ftr(*offspring, mutation_rate);

			// Finally rectification
			rect_ftr(*offspring);
			// TODO: deal with failure (eg. by repeatedly mutating/cx+mutating until a valid solution is found)

			++offspring;
		}
	}
};


#endif


