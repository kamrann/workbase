// rtp_genome_mapping.h

#ifndef __NE_RTP_GENOME_MAPPING_H
#define __NE_RTP_GENOME_MAPPING_H

#include "rtp_defs.h"

#include "ga/rtp_interface/rtp_genome_wrap.h"

#include <functional>
#include <vector>


namespace rtp {

	class i_genome;
	class i_controller;

	class i_genome_mapping
	{
	public:
		typedef double genome_diff_t;
		typedef double diversity_t;

	public:
		virtual genome_diff_t genome_difference(i_genome const* ign1, i_genome const* ign2) const = 0;
		virtual diversity_t population_diversity(std::vector< i_genome const* > const& pop_genomes) const = 0;
		virtual size_t get_genome_length() const = 0;
		virtual std::unique_ptr< i_genome > load_genome_from_binary(std::vector< unsigned char > const& data) = 0;
		virtual std::unique_ptr< i_genome > generate_random_genome(rgen_t& rgen) const = 0;
		virtual void decode_genome(i_genome const* ign, i_controller* ic) const = 0;

		typedef std::function< void(std::vector< rtp_genome_wrapper const* >::const_iterator parents, std::vector< rtp_genome_wrapper >::iterator offspring) > cx_fn_t;
		virtual cx_fn_t get_crossover_fn(rgen_t& rgen) const = 0;

		typedef std::function< void(rtp_genome_wrapper& child, double rate) > mut_fn_t;
		virtual mut_fn_t get_mutation_fn(rgen_t& rgen) const = 0;

	public:
		virtual ~i_genome_mapping() {}
	};

}


#endif


