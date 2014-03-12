// ga.h

#ifndef __GENETIC_ALG_H
#define __GENETIC_ALG_H

#include "truncation_survival_selection.h"
#include "crossover.h"
#include "mutation.h"
#include "basic_rate_functors.h"
#include "random_procreation_selection.h"
#include "procreation.h"
#include "individual.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include <vector>
#include <ostream>


template <
//	typename Genome,
//	typename FitnessType,
	typename Individual,
	typename SurvivalSelection,// = truncation_survival_selection< std::vector< individual< Genome, FitnessType > >::iterator,
	typename CrossoverFtr,// = basic_crossover< Individual::genome_t >,
	typename MutationFtr,// = basic_real_mutation< Individual::genome_t >,
	typename CrossoverRateFtr = FixedCrossoverRate,
	typename MutationRateFtr = FixedMutationRate,
	typename ProcreationSelection = random_procreation_selection<>,
	template <
		typename _ProcreationSelection,
		typename _CrossoverFtr,
		typename _MutationFtr,
		typename _CrossoverRateFtr,
		typename _MutationRateFtr,
		typename _RGen
	> class ProcreationT = basic_procreation
>
struct gen_alg
{
//	typedef Genome								genome_t;
//	typedef FitnessType							fitness_t;
//	typedef individual< genome_t, fitness_t >	individual_t;
	typedef Individual							individual_t;
	typedef typename individual_t::genome_t		genome_t;
	typedef std::vector< individual_t >			population_t;
	typedef std::vector< genome_t >				genome_population_t;
	typedef SurvivalSelection					survival_sel_t;
	typedef CrossoverFtr						cx_ftr_t;
	typedef MutationFtr							mut_ftr_t;
	typedef CrossoverRateFtr					cx_rate_ftr_t;
	typedef MutationRateFtr						mut_rate_ftr_t;
	typedef ProcreationSelection				proc_sel_t;

	typedef boost::random::mt19937				rgen_t;
	typedef ProcreationT< 
		proc_sel_t,
		cx_ftr_t,
		mut_ftr_t,
		cx_rate_ftr_t,
		mut_rate_ftr_t,
		rgen_t >	procreation_t;	// TODO: Really need to enforce the RGen type to be consistent among all template
													// parameter types, but not sure how to do easily

	size_t				genome_length;	// TODO: This is making assumption about genome types...
	population_t		population;

	proc_sel_t&			proc_sel;
	cx_ftr_t&			cx_ftr;
	cx_rate_ftr_t&		cx_rate_ftr;
	mut_ftr_t&			mut_ftr;
	mut_rate_ftr_t&		mut_rate_ftr;

	size_t		generation;
	size_t		max_generations;

	boost::random::mt19937&						rgen;
//	boost::random::uniform_real_distribution<>	rdist_pve_clamped;
//	boost::random::uniform_real_distribution<>	rdist_clamped;


	gen_alg(
		proc_sel_t& _proc_sel,
		cx_ftr_t& _cx,
		cx_rate_ftr_t& _cx_rate,
		mut_ftr_t& _mut,
		mut_rate_ftr_t& _mut_rate,
		boost::random::mt19937& rg
		):
		proc_sel(_proc_sel),
		cx_ftr(_cx),
		cx_rate_ftr(_cx_rate),
		mut_ftr(_mut),
		mut_rate_ftr(_mut_rate),
		genome_length(1),
		generation(0),
//		rdist_pve_clamped(0.0, 1.0),
//		rdist_clamped(-1.0, 1.0),
		rgen(rg)
	{}

	void init_params(size_t gn_length = 1, size_t max_gens = 1)
	{
		genome_length = gn_length;
		max_generations = max_gens;
	}

	template < typename RandomGenomeFtr >
	void init_random_population(size_t pop_size, RandomGenomeFtr const& ftr)
	{
		population.clear();
		population.resize(pop_size, individual_t());

		for(individual_t& idv: population)
		{
//			idv.genome.set_random(genome_length, rgen);
			idv.genome = ftr(rgen);
		}
	}

	size_t epoch()
	{
		// TODO: For now, assuming that fitnesses are calculated externally to this gen_alg class.
		// Will probably want to change this??

		// TODO: Currently maintain a fixed population size, but in future this should be variable
		population_t::iterator valid_end =
			population.end();
/* TODO:
			std::remove_if(population.begin(), population.end(), [](individual_t const& idv) { return idv.fitness.unfit; });
*/
		size_t const ValidPopulationSize = valid_end - population.begin();//population.size();

		// First, survival selection to determine which individuals will become progenitors of the next generation
		// TODO: Number of individuals that should make it to parenthood - where and how should this be defined??
		int const ParenthoodFraction = 2; // Half
		size_t const NumProgenitors = std::max< size_t >(ValidPopulationSize / ParenthoodFraction, std::min< size_t >(ValidPopulationSize, 1));
		population_t progenitors(NumProgenitors);
		survival_sel_t survival_sel(population.begin(), valid_end);
		survival_sel.select_individuals(NumProgenitors, progenitors.begin());

		// Next, procreation
		procreation_t procreation(proc_sel, cx_rate_ftr, cx_ftr, mut_rate_ftr, mut_ftr, rgen, (double)generation / max_generations);
		bool const ParentSurvival = true;	// False = comma-selection, True = plus-selection
		//population_t next_gen(ValidPopulationSize);
		genome_population_t next_gen(ValidPopulationSize);
		if(ParentSurvival)
		{
			// Plus-selection: Copy over progenitors, then top up population size with children
			std::transform(progenitors.begin(), progenitors.end(), next_gen.begin(),
				[](individual_t const& idv)
			{
				return idv.genome;
			}
			);
			procreation.generate_offspring(progenitors.begin(), progenitors.end(), ValidPopulationSize - NumProgenitors, next_gen.begin() + NumProgenitors);
		}
		else
		{
			// Comma-selection: Generate an entirely new population of offspring
			procreation.generate_offspring(progenitors.begin(), progenitors.end(), ValidPopulationSize, next_gen.begin());
		}

		// For every invalid individual, generate a random solution to replace them
/* TODO: !!!!!!!!!!!!!!!!!!!!!! Maybe need to have genetic_mapping as template parameter.
		for(size_t i = 0; i < population.size() - ValidPopulationSize; ++i)
		{
			individual_t idv;
			idv.genome.set_random(genome_length, rgen);
			next_gen.push_back(idv);
		}
		*/

		size_t pop_size = population.size();
		for(size_t i = 0; i < pop_size; ++i)
		{
			population[i].genome = next_gen[i];
		}
		++generation;

		return ValidPopulationSize;
	}

	struct genome_sort_ftr
	{
		inline bool operator() (individual_t const& idv1, individual_t const& idv2) const
		{
			return idv1.genome < idv2.genome;
		}
	};

	void sort_population_by_genome()
	{
		std::sort(population.begin(), population.end(), genome_sort_ftr());
	}

	void output_individuals(std::ostream& out)
	{
		for(individual_t const& idv: population)
		{
			out << idv.fitness << ": " << idv.genome << std::endl;
		}
	}

	void output_population_genomes(std::ostream& out)
	{
		for(individual_t const& idv: population)
		{
			out << idv.genome << std::endl;
		}
	}
};


#endif


