// rtp_genome.h

#ifndef __NE_RTP_GENOME_H
#define __NE_RTP_GENOME_H

#include <vector>


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
//	virtual void to_binary(std::vector< unsigned char >& bytes) = 0;
//	virtual bool from_binary(std::vector< unsigned char > const& bytes) = 0;
};

class i_agent;

class i_genome_mapping
{
public:
	typedef double genome_diff_t;

public:
	virtual genome_diff_t genome_difference(i_genome const* ign1, i_genome const* ign2) = 0;
	virtual size_t get_genome_length() = 0;
	virtual i_genome* generate_random_genome(rgen_t& rgen) = 0;
	virtual void decode_genome(i_genome const* ign, i_agent* ia) = 0;
};


#endif


