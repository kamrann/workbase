// rtp_procreation_selection.cpp

#include "rtp_procreation_selection.h"

#include "ga/random_procreation_selection.h"
#include "ga/equal_procreation_selection.h"
#include "ga/roulette_procreation_selection.h"


namespace rtp {

	std::string const rtp_procreation_selection::Names[] = {
		"Random",
		"Equal",
		//	"Roulette",
	};


	std::unique_ptr< i_procreation_selection > rtp_procreation_selection::create_instance(rtp_param param, rgen_t& rgen)
	{
		Type type = boost::any_cast<Type>(param);
		switch(type)
		{
			case Random:
			return std::make_unique< proc_sel_wrapper< random_procreation_selection< rgen_t > > >(rgen);

			case Equal:
			return std::make_unique< proc_sel_wrapper< equal_procreation_selection< rgen_t > > >(rgen);

			//		case Roulette:
			//		return new proc_sel_wrapper< roulette_procreation_selection< rgen_t > >(rgen);

			default:
			return nullptr;
		}
	}

	std::unique_ptr< i_procreation_selection > rtp_procreation_selection::create_instance(Type type, rgen_t& rgen)
	{
		switch(type)
		{
			case Random:
			return std::make_unique< proc_sel_wrapper< random_procreation_selection< rgen_t > > >(rgen);

			case Equal:
			return std::make_unique< proc_sel_wrapper< equal_procreation_selection< rgen_t > > >(rgen);

			//		case Roulette:
			//		return new proc_sel_wrapper< roulette_procreation_selection< rgen_t > >(rgen);

			default:
			return nullptr;
		}
	}

}

