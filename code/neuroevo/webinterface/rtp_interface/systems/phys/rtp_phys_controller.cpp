// rtp_phys_controller.cpp

#include "rtp_phys_controller.h"
#include "controllers/passive.h"
#include "controllers/interactive.h"


namespace rtp_phys
{
	std::string const i_phys_controller::Names[] = {
		"Passive",
		"Interactive",
	};

	i_phys_controller::enum_param_type::enum_param_type()
	{
		for(size_t i = 0; i < Type::Count; ++i)
		{
			add_item(Names[i], (Type)i);
		}
		set_default_index(0);
	}

	rtp_param_type* i_phys_controller::params(Type type)
	{
		switch(type)
		{
			case i_phys_controller::Passive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());

			case i_phys_controller::Interactive:
			// TODO:
			return new rtp_staticparamlist_param_type(rtp_named_param_list());

			default:
			return nullptr;
		}
	}

	i_phys_controller* i_phys_controller::create_instance(Type type, rtp_param param)
	{
		switch(type)
		{
			case Passive:
			return new passive_controller(/*sub*/);

			case Interactive:
			return new interactive_controller(/*sub*/);

			default:
			return nullptr;
		}
	}
}


