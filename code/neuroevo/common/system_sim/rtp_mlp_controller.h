// rtp_mlp_controller.h

#ifndef __NE_RTP_GENERIC_MLP_CONTROLLER_H
#define __NE_RTP_GENERIC_MLP_CONTROLLER_H

#include "neuralnet/implementations/mlf/mlp.h"

#include <vector>


namespace sys {

	class generic_mlp_controller
	{
	public:
		void set_weights(std::vector< double > const& weights);

	protected:
		nnet::mlp nn;
	};

}


#endif

