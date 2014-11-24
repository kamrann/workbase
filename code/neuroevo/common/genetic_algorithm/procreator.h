// procreator.h

#ifndef __WB_GA_PREOCREATOR_H
#define __WB_GA_PREOCREATOR_H

#include "genalg_common.h"
#include "genome.h"
#include "rate_function.h"
#include "operators.h"
#include "rectification.h"

#include <functional>


namespace ga {

	class genetic_population;

	//
	typedef rate_fn_t crossover_rate_fn_t;
	typedef rate_fn_t mutation_rate_fn_t;

	/*
	Provides functionality to generate the next generation of genomes.
	*/
	class procreator
	{
	public:
		procreator(rgen_t& rgen);

	public:
		crossover_rate_fn_t& crossover_rate_fn();
		mutation_rate_fn_t& mutation_rate_fn();
		crossover_fn_t& crossover_fn();
		mutation& mutation_fn();
		rectification_fn_t& rectification_fn();

		void reset();

		/*
		Prior to being called, the objective values of all members of the existing population should be initialized.
		*/
		genetic_population next_generation(
			genetic_population const& existing
			);

	private:
		// todo: parameters
		crossover_rate_fn_t m_cx_rate_fn;
		mutation_rate_fn_t m_mut_rate_fn;
		crossover_fn_t m_cx_fn;
		mutation m_mut_fn;
		rectification_fn_t m_rect_fn;
		rgen_t& m_rgen;

	public:	// TODO: Probably better off in genalg, and passed into next_generation()
		size_t m_generation;
	};

}


#endif


