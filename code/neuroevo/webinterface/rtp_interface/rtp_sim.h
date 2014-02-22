// rtp_sim.h

#ifndef __NE_RTP_SIM_H
#define __NE_RTP_SIM_H

#include "rtp_defs.h"
#include "rtp_param.h"

#include <ostream>


class i_genome;
class i_system;

class rtp_simulation
{
public:
	rtp_simulation(rtp_param sys_param, rtp_param evo_param);

public:
	void init(size_t max_gens);
	genotype_diversity_measure population_genotype_diversity() const;
	void run_epoch(/*observation_data_t& observations,*/ std::ostream& os);
	i_genome* get_individual_genome(size_t idx);

	//virtual processed_obj_val_t get_fittest(agent_t& a) const;

private:
	i_system* m_system;
};


#endif


