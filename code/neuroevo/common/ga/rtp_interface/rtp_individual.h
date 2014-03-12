// rtp_individual.h

#ifndef __GA_RTP_INDIVIDUAL_H
#define __GA_RTP_INDIVIDUAL_H

#include "rtp_genome_wrap.h"

#include "../individual.h"

//#include <boost/any.hpp>


typedef individual<
	rtp_genome_wrapper,
	double	// TODO: boost::any, with a fitness comparison interface?
> rtp_individual;


#endif


