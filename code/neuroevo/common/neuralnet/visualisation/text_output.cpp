// text_output.cpp

#include "text_output.h"
#include "../interface/neuralnet.h"
#include "../interface/neuron_access.h"
#include "../interface/neuron.h"

#include <sstream>


namespace nnet {

	namespace vis {

		std::string neuron_name(i_neuralnet* net, neuron_id id, NeuronNameStyle style)
		{
			auto nr_access = net->neuron_access();
			auto nr = nr_access->get_neuron_data(id);
			auto layer = nr_access->get_neuron_layer(id);

			// TODO: This is super slow
			size_t local_index = 0;

			if(nr.type == NeuronType::Bias)
			{
				// For bias neurons, just index across entire network, ignoring their nominal layers
				auto range = nr_access->get_range(neuron_filter{ { NeuronType::Bias } });
				// And linear search the range for this neuron
				for(auto const& n : range)
				{
					if(n.id == id)
					{
						break;
					}
					++local_index;
				}
			}
			else
			{
				// Otherwise, get range of neurons of same type in same layer
				auto range = nr_access->get_range(layer_filter{ layer_set{ layer } }, neuron_filter{ { nr.type } });
				// And linear search the range for this neuron
				for(auto const& n : range)
				{
					if(n.id == id)
					{
						break;
					}
					++local_index;
				}
			}

			std::stringstream ss;
			switch(style)
			{
				case NeuronNameStyle::Normal:
				ss << NeuronTypeNames[(int)nr.type] << " ";
				break;
				case NeuronNameStyle::Short:
				ss << NeuronTypeAbbreviations[(int)nr.type];
				break;
			}
			if(nr.type != NeuronType::Bias && layer.type == LayerType::Hidden && layer.subindex)
			{
				ss << (*layer.subindex + 1) << ":";
			}
			ss << (local_index + 1);
			return ss.str();
		}
		
	}
}



