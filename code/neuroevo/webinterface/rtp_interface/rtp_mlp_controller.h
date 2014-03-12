// rtp_mlp_controller.h

#ifndef __NE_RTP_MLP_CONTROLLER_H
#define __NE_RTP_MLP_CONTROLLER_H

#include <doublefann.h>
#include <fann_cpp.h>

#include <vector>


class generic_mlp_controller
{
public:
	void set_weights(std::vector< double > const& weights);

protected:
	FANN::neural_net nn;
};



#endif

