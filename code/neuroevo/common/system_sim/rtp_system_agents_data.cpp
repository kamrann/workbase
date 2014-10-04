// rtp_system_agents_data.cpp

#include "rtp_system_agents_data.h"
#if 0
#include "rtp_controller.h"
#include "../rtp_genome_mapping.h"
#include "../rtp_pop_wide_observer.h"
#include "rtp_observer.h"


namespace sys {

	agents_data::evolved_data::evolved_data()
	{}

	agents_data::evolved_data::evolved_data(evolved_data&& rhs):
		genome_mapping(std::move(rhs.genome_mapping)),
		resultant_obj(std::move(rhs.resultant_obj)),
		observer(std::move(rhs.observer))
	{}

	agents_data::evolved_data& agents_data::evolved_data::operator= (evolved_data&& rhs)
	{
		genome_mapping = std::move(rhs.genome_mapping);
		resultant_obj = std::move(rhs.resultant_obj);
		observer = std::move(rhs.observer);
		return *this;
	}

	agents_data::evolved_data::~evolved_data()
	{}


	agents_data::agents_data()
	{}

	agents_data::agents_data(agents_data&& rhs):
		agents(std::move(rhs.agents)),
		controller_factories(std::move(rhs.controller_factories)),
		evolved(std::move(rhs.evolved))
	{}

	agents_data& agents_data::operator= (agents_data&& rhs)
	{
		agents = std::move(rhs.agents);
		controller_factories = std::move(rhs.controller_factories);
		evolved = std::move(rhs.evolved);
		return *this;
	}

	agents_data::~agents_data()
	{}

}

#endif