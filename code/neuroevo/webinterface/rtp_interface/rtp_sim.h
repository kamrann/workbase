// rtp_sim.h

#ifndef __NE_RTP_SIM_H
#define __NE_RTP_SIM_H

#include "rtp_defs.h"
#include "rtp_param.h"

#include <ostream>


class i_genome;
class i_genome_mapping;
class i_system;
class i_agent;
class i_agent_factory;
class i_observer;

class rtp_simulation
{
public:
	rtp_simulation(rtp_param sys_param, rtp_param evo_param);

public:
	static rtp_named_param_list evo_params();

public:
	void init();
	genotype_diversity_measure population_genotype_diversity() const;
	void run_epoch(/*observation_data_t& observations,*/ std::ostream& os);
	i_genome* get_individual_genome(size_t idx);
	i_agent* get_fittest() const;
	boost::any get_highest_objective() const;
	boost::any get_average_objective() const;

//private:
public:
	i_system* system;
	i_observer* obs;
	i_genome_mapping* gn_mapping;
	i_agent_factory* a_factory;

	size_t population_size;
	size_t total_generations;
	size_t trials_per_generation;
	
	size_t generation;

	struct indiv
	{
		i_genome* gn;
		i_agent* idv;
		double obj_value;	// TODO:
		double fitness;

		indiv(): gn(nullptr), idv(nullptr), obj_value(0.0), fitness(0.0)
		{}
	};

	std::vector< indiv > population;

	rgen_t rgen;
	unsigned int ga_rseed;
	unsigned int trials_rseed;
};


#endif


