// rtp_phys_agent_body_spec.cpp

#include "rtp_phys_agent_body_spec.h"
#include "bodies/test_body.h"
#include "bodies/test_quadruped_body.h"
#include "bodies/test_biped_body.h"


namespace rtp_phys {

	std::string const agent_body_spec::Names[] = {
		"Test Creature",
		"Test Quadruped",
		"Test Biped",
	};

	agent_body_spec::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	agent_body_spec* agent_body_spec::create_instance(rtp_param param)
	{
		Type type = boost::any_cast<Type>(param);
		switch(type)
		{
			case TestCreature:
			return test_body::spec::create_instance(rtp_param());

			case TestQuadruped:
			return test_quadruped_body::spec::create_instance(rtp_param());

			case TestBiped:
			return test_biped_body::spec::create_instance(rtp_param());

			default:
			return nullptr;
		}
	}

	agent_body_spec::agent_body_spec(Type t): m_type(t)
	{}
}



