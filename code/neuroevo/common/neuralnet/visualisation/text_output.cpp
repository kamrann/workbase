// text_output.cpp

#include "text_output.h"
#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"
#include "../interface/neuron.h"

#include <sstream>


namespace nnet {

	namespace vis {

		std::string neuron_name(i_neuralnet* net, neuron_id id)
		{
			auto nr_access = net->neuron_access();
			auto nr = nr_access->get_neuron_data(id);
			std::stringstream ss;
			ss << NeuronTypeNames[(int)nr.type] << " [" << (id + 1) << "]";
			return ss.str();
		}
		
	}
}



