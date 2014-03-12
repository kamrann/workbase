// rtp_pop_wide_observer.cpp

#include "rtp_pop_wide_observer.h"


std::string const i_population_wide_observer::Names[] = {
	"Single Objective",
	"Pareto Multiple Objective",
};

i_population_wide_observer::enum_param_type::enum_param_type()
{
	for(size_t i = 0; i < Type::Count; ++i)
	{
		add_item(Names[i], (Type)i);
	}
	set_default_index(0);
}

i_population_wide_observer* i_population_wide_observer::create_instance(Type type, rtp_param param)
{
	return nullptr;	// TODO: 
}


