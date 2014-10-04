// connection.h

#ifndef __WB_NN_CONNECTION_H
#define __WB_NN_CONNECTION_H

#include "basic_types.h"


namespace nnet {

	class connection_data
	{
	public:
		connection_id id;
		neuron_id src;
		neuron_id dst;
		weight_t weight;
	};

}


#endif


