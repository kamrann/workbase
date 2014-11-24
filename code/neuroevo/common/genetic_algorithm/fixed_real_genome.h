// fixed_real_genome.h

#ifndef __WB_GA_FIXED_REAL_GENOME_H
#define __WB_GA_FIXED_REAL_GENOME_H

#include "fixed_array_genome.h"

#include <functional>
#include <algorithm>


namespace ga {

	/*
	An implementation of a genome which is a fixed length array of real-valued genes.
	*/
	class fixed_real_genome:
		public i_genome,
		public fixed_array_genome< double >
	{
	public:
		typedef double value_t;

		static size_t const BytesPerValue = sizeof(value_t);

	public:
		virtual std::unique_ptr< i_genome > clone() const override;
		virtual std::vector< unsigned char > to_binary() const override;

	public:
		// TODO: Not sure if this should be part of virtual interface or not
		static fixed_real_genome from_binary(std::vector< unsigned char > const& bytes);
	};

	/*
	Default mutation operator
	*/
//	template < typename GeneMutation >
	template < typename Gene >
	struct basic_real_mutation
	{
//		typedef GeneMutation gene_mutation_t;

		typedef Gene gene_t;
		typedef std::function< void(gene_t&, rgen_t&) > gene_mutation_t;

		basic_real_mutation(gene_mutation_t _gene_mut):
			mut_dist(0.0, 1.0),
			gene_mut(_gene_mut)
		{}

		void operator()(fixed_real_genome& gn, double const rate, rgen_t& rgen)
		{
			for(auto& g : gn)
			{
				if(mut_dist(rgen) < rate)
				{
					// Mutate this gene
					gene_mut(g, rgen);
				}
			}
		}

		std::uniform_real_distribution< double > mut_dist;
		gene_mutation_t gene_mut;
	};

}


#endif


