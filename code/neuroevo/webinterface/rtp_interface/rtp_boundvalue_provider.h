// rtp_boundvalue_provider.h

#ifndef __NE_RTP_BOUND_VALUE_PROVIDER_H
#define __NE_RTP_BOUND_VALUE_PROVIDER_H

#include "rtp_boundvalue.h"


namespace rtp {

	class i_boundvalue_provider
	{
	public:
		virtual boundvalue_buffer create_bindings(unbound_id_set const& ids) = 0;

		// Less efficient, direct value accessor
//		virtual boundvalue get_value(boundvalue_id id) = 0;

	public:
		virtual ~i_boundvalue_provider() {}
	};

}


#endif


