// fixed_binary_genome.cpp

#include "fixed_binary_genome.h"


namespace ga {

	fixed_binary_genome fixed_binary_genome::generate_random(size_t len, rgen_t& rgen)
	{
		std::uniform_int_distribution< unsigned int/*gene_t*/ > dist{ 0, 1 };
		fixed_binary_genome gn;
		gn.resize(len);
		for(auto& g : gn)
		{
			g = dist(rgen);
		}
		return gn;
	}

	std::unique_ptr< i_genome > fixed_binary_genome::clone() const
	{
		return std::make_unique< fixed_binary_genome >(*this);
	}

	std::vector< unsigned char > fixed_binary_genome::to_binary() const
	{
		// Just copy the underlying array
		return m_genes;
	}

}



