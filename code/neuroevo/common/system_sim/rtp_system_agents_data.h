// rtp_system_agents_data.h

#ifndef __NE_RTP_SYSTEM_AGENTS_DATA_H
#define __NE_RTP_SYSTEM_AGENTS_DATA_H

#include "rtp_system_agent_specification.h"

#include <vector>
#include <map>
#include <memory>


namespace sys {

	class i_controller_factory;
	class i_genome_mapping;
	class i_population_wide_observer;
	class i_observer;

#if 0
	struct agents_data
	{
		struct agent_instance_data
		{
			agent_specification spec;
			size_t controller_id;
		};

		std::vector< agent_instance_data > agents;
		std::map< size_t, std::unique_ptr< i_controller_factory > > controller_factories;

		struct evolved_data
		{
			std::unique_ptr< i_genome_mapping > genome_mapping;
			std::unique_ptr< i_population_wide_observer > resultant_obj;
			std::unique_ptr< i_observer > observer;

			evolved_data();
			evolved_data(evolved_data&& rhs);
			evolved_data& operator= (evolved_data&& rhs);
			~evolved_data();
		};

		std::map< size_t, evolved_data > evolved;

		// TODO: wtf are the compiler generated ones not working??
		agents_data();
		agents_data(agents_data&& rhs);
		agents_data& operator= (agents_data&& rhs);
		~agents_data();
	};
#endif
}


#endif

