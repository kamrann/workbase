// operators.h

#ifndef __WB_GA_OPERATORS_H
#define __WB_GA_OPERATORS_H

#include "genalg_common.h"

#include <functional>


namespace ga {

	class genome;

	struct op_update_context
	{
		size_t generation;
	};

	// Crossover function form (TODO: Most basic and restrictive, should allow variable numbers of input and output genomes)
	typedef std::function< genome(genome const&, genome const&, rgen_t&) > crossover_fn_t;

	// Mutation function form
//	typedef std::function< void(genome&, double const, rgen_t&) > mutation_fn_t;

	template < typename Gene >
	struct gene_mutation
	{
		typedef Gene gene_t;

		std::function< void(op_update_context const&) > update;
		std::function< void(gene_t&, rgen_t&) > op;

		gene_mutation()
		{}

		template < typename T >
		gene_mutation(T obj)
		{
			// TODO: This can't be efficient since introducing extra indirection
			// Note: Issue here is that the two std::function members should encapsulate the same
			// underlying object, not separate copies.
			auto sp = std::make_shared< T >(std::move(obj));

			update = [sp](op_update_context const& ctx)
			{
				(*sp)(ctx);
			};

			op = [sp](gene_t& g, rgen_t& rgen)
			{
				(*sp)(g, rgen);
			};
		}
	};

	struct mutation
	{
		std::function< void(op_update_context const&) > update;
		std::function< void(genome&, double const, rgen_t&) > op;

/*		mutation()
		{}

		template < typename T >
		mutation(T obj)
		{
			// TODO: This can't be efficient since introducing extra indirection
			// Note: Issue here is that the two std::function members should encapsulate the same
			// underlying object, not separate copies.
			auto sp = std::make_shared< T >(std::move(obj));
			
			update = [sp](op_update_context const& ctx)
			{
				(*sp)(ctx);
			};

			op = [sp](genome& gn, double const rate, rgen_t& rgen)
			{
				(*sp)(gn, rate, rgen);
			};
		}
*/	};

}


#endif


