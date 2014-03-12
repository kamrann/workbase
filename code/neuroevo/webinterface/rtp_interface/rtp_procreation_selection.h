// rtp_procreation_selection.h

#ifndef __NE_RTP_PROCREATION_SELECTION_H
#define __NE_RTP_PROCREATION_SELECTION_H

#include "rtp_defs.h"
#include "params/enum_par.h"

#include "ga/rtp_interface/rtp_procreation_selection.h"


class rtp_procreation_selection
{
public:
	enum Type {
		Random,
		Equal,
//		Roulette,

		Count,
	};

	static std::string const Names[Count];

	class enum_param_type: public rtp_enum_param_type
	{
	public:
		enum_param_type();
	};

public:
	static i_procreation_selection* create_instance(rtp_param param, rgen_t& rgen);
};


#endif


