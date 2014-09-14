// rtp_boundvalue_consumer.h

#ifndef __NE_RTP_BOUND_VALUE_CONSUMER_H
#define __NE_RTP_BOUND_VALUE_CONSUMER_H

#include "rtp_boundvalue.h"


namespace rtp {

	class i_boundvalue_consumer
	{
	public:
		// Returns a list of unbound identifiers for the values needed by this consumer
		virtual unbound_id_set get_requirements() const = 0;

		// Registers the bindings corresponding to the retrieved requirements
		// (The index into the accessor matches the index in the returned unbound_id_set)
		virtual void register_bindings(boundvalue_set_accessor bindings) = 0;

	public:
		virtual ~i_boundvalue_consumer() {}
	};
}



#endif


