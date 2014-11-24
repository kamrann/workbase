// nn_controller.cpp

#include "nn_controller.h"

#include "neuralnet/interface/output.h"


namespace sys {
	namespace ev {

		nn_controller::nn_controller(std::unique_ptr< nnet::i_neuralnet > nn, input_id_list_t input_ids):
			nn_(std::move(nn)),
			input_ids_(input_ids)
		{

		}

		auto nn_controller::get_input_ids() const -> input_id_list_t
		{
			return input_ids_;
		}

		auto nn_controller::process(input_list_t const& inputs) -> output_list_t
		{
			auto in = nnet::input{ inputs };
			auto out = nn_->run(in);
			return out.data();
		}

	}
}





