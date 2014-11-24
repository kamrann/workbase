// fixed_binary_genome.h

#ifndef __WB_GA_FIXED_BINARY_GENOME_H
#define __WB_GA_FIXED_BINARY_GENOME_H

#include "fixed_array_genome.h"

//#include <boost/dynamic_bitset.hpp>


namespace ga {

	/*
	A genome which is a fixed length array of bits.
	*/
	class fixed_binary_genome:
		public i_genome,
		public fixed_array_genome< unsigned char >
	{
	public:
		// NOTE: Using stl vector of char for consistency, so not space optimized.
		// Unlikely will be an issue.
//		typedef boost::dynamic_bitset< unsigned long > chromosome_t;

	public:
		static fixed_binary_genome generate_random(size_t len, rgen_t& rgen);

	public:
		virtual std::unique_ptr< i_genome > clone() const override;
		virtual std::vector< unsigned char > to_binary() const override;
	};

	/*
	Default mutation operator
	*/
	struct basic_binary_mutation
	{
		basic_binary_mutation():
			dist(0.0, 1.0)
		{}

		void operator()(fixed_binary_genome& gn, double const rate, rgen_t& rgen)
		{
			for(auto& g : gn)
			{
				// Probabilistically flip the binary gene
				if(dist(rgen) < rate)
				{
					g ^= 1u;
				}
			}
		}

		std::uniform_real_distribution< double > dist;
	};
	/*
	struct 
	void basic_binary_mutation(genome& gn, double const rate, rgen_t& rgen)
	{
		basic_binary_mutation(gn.as< fixed_binary_genome >(), rate, rgen);
	}
	*/

}


#endif


