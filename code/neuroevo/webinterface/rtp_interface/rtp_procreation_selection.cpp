// rtp_procreation_selection.cpp

#include "rtp_procreation_selection.h"

#include "ga/random_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
#include "ga/roulette_procreation_selection.h"


std::string const rtp_procreation_selection::Names[] = {
	"Random",
	"Equal",
//	"Roulette",
};

rtp_procreation_selection::enum_param_type::enum_param_type()
{
	for(size_t i = 0; i < Type::Count; ++i)
	{
		add_item(Names[i], (Type)i);
	}
	set_default_index(0);
}

i_procreation_selection* rtp_procreation_selection::create_instance(rtp_param param, rgen_t& rgen)
{
	Type type = boost::any_cast<Type>(param);
	switch(type)
	{
		case Random:
		return new proc_sel_wrapper< random_procreation_selection< rgen_t > >(rgen);

		case Equal:
		return new proc_sel_wrapper< equal_procreation_selection< rgen_t > >(rgen);

//		case Roulette:
//		return new proc_sel_wrapper< roulette_procreation_selection< rgen_t > >(rgen);

		default:
		return nullptr;
	}
}



