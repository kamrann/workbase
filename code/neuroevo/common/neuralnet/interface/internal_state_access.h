// internal_state_access.h

#ifndef __WB_NN_INTERNAL_STATE_ACCESS_H
#define __WB_NN_INTERNAL_STATE_ACCESS_H

#include "basic_types.h"

#include <map>


namespace nnet {

	struct activity_state
	{
		struct neuron_state
		{
			value_t sum;
			value_t activation;
		};

		struct connection_state
		{
			value_t signal;
		};

		typedef std::map< neuron_id, neuron_state > neuron_map_t;
		typedef std::map< std::pair< neuron_id, neuron_id >, connection_state > connection_map_t;

		neuron_map_t neurons;
		connection_map_t connections;
	};

/*	class i_internal_state
	{
	public:
		virtual value_t neuron_activation(neuron_id id) const = 0;

	public:
		virtual ~i_internal_state()
		{}
	};
*/
}


#endif


