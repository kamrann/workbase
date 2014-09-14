// rtp_procreation_selection.h

#ifndef __NE_RTP_PROCREATION_SELECTION_H
#define __NE_RTP_PROCREATION_SELECTION_H

#include "rtp_defs.h"
#include "params/enum_par.h"

#include "ga/rtp_interface/rtp_procreation_selection.h"


namespace rtp {

	class rtp_procreation_selection
	{
	public:
		enum Type {
			Random,
			Equal,
			//		Roulette,

			Count,
			Default = Random,
		};

		static std::string const Names[Count];

	public:
		static std::unique_ptr< i_procreation_selection > create_instance(rtp_param param, rgen_t& rgen);
		static std::unique_ptr< i_procreation_selection > create_instance(Type type, rgen_t& rgen);
	};

}


#endif


