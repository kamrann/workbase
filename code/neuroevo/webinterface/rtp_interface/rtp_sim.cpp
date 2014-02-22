// rtp_sim.cpp

#include "rtp_sim.h"
#include "systems/rtp_system.h"


rtp_simulation::rtp_simulation(rtp_param sys_param, rtp_param evo_param)
{
	m_system = i_system::create_instance(sys_param);

	// TODO: store evo params
}

/*
Want the GA to be the highest level process. The only thing it needs to know about is the genome type, so potentially it can 
remain templatized. We just call a virtual function of some object which knows about the genome representation, and this method
will return a base class pointer to a GA object set up for the given genome type. Then we just call a virtual function ga.do_generation().
From within that method, the ga will need to callback something along the lines of sys.evaluate_genome_fitnesses(genomes of entire pop).
*/

void rtp_simulation::init(size_t max_gens)
{
	
}

genotype_diversity_measure rtp_simulation::population_genotype_diversity() const
{
	return genotype_diversity_measure();
}
	
void rtp_simulation::run_epoch(/*observation_data_t& observations,*/ std::ostream& os)
{

}
	
i_genome* rtp_simulation::get_individual_genome(size_t idx)
{
	return nullptr;
}



