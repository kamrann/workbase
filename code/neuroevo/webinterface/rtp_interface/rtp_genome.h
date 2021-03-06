// rtp_genome.h

#ifndef __NE_RTP_GENOME_H
#define __NE_RTP_GENOME_H

#include <vector>


namespace rtp {

	class i_gen_alg;

	// This specifies only the low-level genome properties, such as storage type and crossover/mutation ops.
	// It does not specify anything domain specific such as the genotype -> phenotype mapping.
	class i_genome_representation
	{
	public:
		virtual i_gen_alg* create_gen_alg();
	};

	class i_genome
	{
	public:
		virtual void to_binary(std::vector< unsigned char >& bytes) = 0;
		virtual bool from_binary(std::vector< unsigned char > const& bytes) = 0;
		virtual std::unique_ptr< i_genome > clone() const = 0;

	public:
		virtual ~i_genome()
		{}
	};

}


#endif


