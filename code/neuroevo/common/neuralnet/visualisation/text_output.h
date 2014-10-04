// text_output.h

#ifndef __WB_NN_VIS_TEXT_H
#define __WB_NN_VIS_TEXT_H

#include "../interface/nnet_fwd.h"

#include <string>


namespace nnet {

	namespace vis {

		enum class NeuronNameStyle {
			Normal,
			Short,
		};

		std::string neuron_name(i_neuralnet* net, neuron_id id, NeuronNameStyle style = NeuronNameStyle::Normal);
		
	}
}


#endif


