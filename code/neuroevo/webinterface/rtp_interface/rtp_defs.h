// rtp_defs.h

#ifndef __NE_RTP_DEFS_H
#define __NE_RTP_DEFS_H

#include <boost/random/mersenne_twister.hpp>

#include <cstdint>


namespace rtp {

	typedef uint32_t agent_id_t;
	typedef double genotype_diversity_measure;

	typedef boost::random::mt19937 rgen_t;

}


#endif


