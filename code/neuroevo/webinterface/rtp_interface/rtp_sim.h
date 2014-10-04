// rtp_sim.h

#ifndef __NE_RTP_SIM_H
#define __NE_RTP_SIM_H

#include "rtp_defs.h"
#include "systems/rtp_system.h"
#include "systems/rtp_system_agents_data.h"

#include "wt_param_widgets/pw_fwd.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <vector>
#include <ostream>


namespace rtp {

	class i_genome;
	class i_genome_mapping;
	class i_system_factory;
	class i_controller;
	class i_controller_factory;
	class i_observer;
	class i_population_wide_observer;
	class i_procreation_selection;

	namespace YAML {
		class Node;
	}

	class rtp_simulation
	{
	public:
		rtp_simulation(prm::param_accessor sys_param, prm::param_accessor evo_param);

	public:
		static prm::schema::schema_node get_evo_schema(prm::param_accessor param_vals);
		static std::string update_schema_providor(prm::schema::schema_provider_map_handle provider, prm::qualified_path const& prefix);

	public:
		void init(boost::optional< uint32_t > seed = boost::none);
		genotype_diversity_measure population_genotype_diversity() const;
		void run_epoch(/*observation_data_t& observations,*/ std::ostream& os);
		i_genome* get_individual_genome(size_t idx);
		//	boost::shared_ptr< i_controller > get_fittest() const;
		boost::any get_highest_objective() const;
		boost::any get_average_objective() const;

	protected:
		// Temp helpers while assuming only one evolvable controllers
		unsigned long m_evolvable_id;

		i_genome_mapping* get_genome_mapping() const
		{
			return agent_data.evolved.at(m_evolvable_id).genome_mapping.get();
		}

		i_controller_factory* get_evolved_controller_factory() const
		{
			return agent_data.controller_factories.at(m_evolvable_id).get();
		}

		i_population_wide_observer* get_resultant_objective() const
		{
			return agent_data.evolved.at(m_evolvable_id).resultant_obj.get();
		}

		i_observer* get_observer() const
		{
			return agent_data.evolved.at(m_evolvable_id).observer.get();
		}
		////////

		//private:
	public:
		bool procreation_enabled;
		bool trials_enabled;

		std::unique_ptr< i_system_factory > sys_factory;
		agents_data agent_data;
		/*	i_observer* obs;
			i_population_wide_observer* resultant_obj;
			i_genome_mapping* gn_mapping;
			i_controller_factory* a_factory;
			*/
		std::unique_ptr< i_procreation_selection > proc_sel;

		size_t population_size;
		size_t total_generations;
		size_t trials_per_generation;

		// Is training data (ie. system initial state) the same for every generation?
		// Regardless of this setting, data will be the same for all agents within a given generation.
		bool static_training_set;

		size_t generation;

		struct indiv
		{
			std::unique_ptr< i_genome > gn;
			//		std::unique_ptr< i_controller > idv;
			double obj_value;	// TODO:
			double fitness;

			indiv(): gn(nullptr), /*idv(nullptr),*/ obj_value(0.0), fitness(0.0)
			{}

			indiv(indiv&& rhs):
				gn(std::move(rhs.gn)),
				obj_value(rhs.obj_value),
				fitness(rhs.fitness)
			{}

			indiv& operator= (indiv&& rhs)
			{
				gn = std::move(rhs.gn);
				obj_value = rhs.obj_value;
				fitness = rhs.fitness;
				return *this;
			}
		};

		std::vector< indiv > population;

		rgen_t rgen;
		unsigned int base_seed;
		unsigned int ga_rseed;
		unsigned int trials_rseed;

		std::chrono::high_resolution_clock::duration
			m_ga_update_time,
			m_trials_update_time;
	};

}


#endif


