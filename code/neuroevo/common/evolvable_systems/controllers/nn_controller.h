// nn_controller.h

#ifndef __WB_EV_SYS_NN_CONTROLLER_H
#define __WB_EV_SYS_NN_CONTROLLER_H

#include "system_sim/controller.h"

#include "neuralnet/interface/neuralnet.h"


namespace sys {
	namespace ev {

		class nn_controller:
			public i_controller
		{
		public:
			nn_controller(std::unique_ptr< nnet::i_neuralnet > nn, input_id_list_t input_ids);

		public:
			virtual input_id_list_t get_input_ids() const override;
			virtual output_list_t process(input_list_t const& inputs) override;

		protected:
			input_id_list_t input_ids_;
			std::unique_ptr< nnet::i_neuralnet > nn_;
		};

	}
}


#endif


