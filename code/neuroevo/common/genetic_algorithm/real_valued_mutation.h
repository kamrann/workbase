// real_valued_mutation.h
/*
Gene level mutation for real-valued genes
*/

#ifndef __WB_GA_REAL_VALUED_MUTATION_H
#define __WB_GA_REAL_VALUED_MUTATION_H

#include "operators.h"
#include "interpolated_function.h"

#include <random>


namespace ga {

	template < typename Rep >
	struct real_valued_gene
	{
		typedef Rep gene_t;

		/*
		Basic mutation, perturbing by a linearly distributed amount up to some maximum perturbation threshold
		*/
		struct linear_mutation
		{
			linear_mutation(gene_t max_perturbation):
				perturb_dist(-max_perturbation, max_perturbation)
			{}

			inline void operator() (op_update_context const& ctx)
			{}

			inline void operator() (gene_t& g, rgen_t& rgen)
			{
				// Perturb the gene
				g += perturb_dist(rgen);
			}

			std::uniform_real_distribution< gene_t > perturb_dist;
		};

		/*
		Gaussian mutation, perturbing using a gaussian distribution with a given standard deviation
		*/
		struct gaussian_mutation
		{
			gaussian_mutation(gene_t stddev):
				perturb_dist(0.0, stddev)
			{}

			inline void operator() (op_update_context const& ctx)
			{}

			inline void operator()(gene_t& g, rgen_t& rgen)
			{
				// Perturb the gene
				g += perturb_dist(rgen);
			}

			std::normal_distribution< gene_t > perturb_dist;
		};

		/*
		Variable gaussian mutation, perturbing using a gaussian distribution with a varying standard deviation
		*/
		struct variable_gaussian_mutation:
			public interpolated_function< double, size_t >
		{
			typedef interpolated_function base_t;

/*			variable_gaussian_mutation(gene_t stddev):
				perturb_dist(stddev)
			{}
*/
			using base_t::base_t;

			inline void operator() (op_update_context const& ctx)
			{
				// Adjust the gaussian distrubution based on generation
				auto stddev = base_t::operator() (ctx.generation);
				perturb_dist = std::normal_distribution < gene_t > { 0.0, stddev };
			}

			inline void operator()(gene_t& g, rgen_t& rgen)
			{
				// Perturb the gene
				g += perturb_dist(rgen);
			}

			std::normal_distribution< gene_t > perturb_dist;
		};

		/* 
		struct bounded_real_mutation
		{
			bounded_real_mutation(double lower, double upper, double max_perturbation):
				_lower(lower),
				_upper(upper),
				mut_dist(0.0, 1.0),
				perturb_dist(-max_perturbation, max_perturbation)
			{}

			void operator()(fixed_real_genome& gn, double const rate, rgen_t& rgen)
			{
				for(auto& g : gn)
				{
					if(mut_dist(rgen) < rate)
					{
						// Perturb the gene
						g += perturb_dist(rgen);
						g = std::max(g, _lower);
						g = std::min(g, _upper);
					}
				}
			}

			double _lower, _upper;
			std::uniform_real_distribution< double > mut_dist, perturb_dist;
		};
		*/

	};

}


#endif


