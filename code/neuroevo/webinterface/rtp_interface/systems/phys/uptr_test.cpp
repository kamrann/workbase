// uptr_test.cpp

#include "rtp_phys_controller.h"
#include "../../rtp_genome_mapping.h"
//
#include "rtp_phys_resultant_objective.h"
#include "rtp_phys_observers.h"
//

// Nov CTP bug
typedef std::tuple <
	std::vector< std::unique_ptr< int > >
> ProblemType;


void foo()
{
	ProblemType p;
}



