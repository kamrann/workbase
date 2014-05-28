// rtp_sat_evolvable_controller.cpp

#include "rtp_sat_evolvable_controller.h"
#include "controllers/mlp.h"


namespace rtp_sat
{
	std::string const evolvable_controller::Names[] = {
		"MLP",
	};

	evolvable_controller::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	rtp_param_type* evolvable_controller::params(Type ea_type)
	{
		switch(ea_type)
		{
			case MLP:
			return new mlp_controller::param_type();

			default:
			assert(false);
			return nullptr;
		}
	}

	std::tuple< i_genome_mapping*, i_agent_factory* > evolvable_controller::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case MLP:
			return mlp_controller::create_instance_evolvable(param);

			default:
			assert(false);
			return std::tuple< i_genome_mapping*, i_agent_factory* >(nullptr, nullptr);
		}
	}
}


