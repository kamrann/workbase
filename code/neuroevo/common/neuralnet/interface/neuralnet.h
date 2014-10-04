// neuralnet.h

#ifndef __WB_NN_NEURALNET_H
#define __WB_NN_NEURALNET_H

#include "basic_types.h"
#include "input.h"
#include "access_options.h"

#include <memory>


namespace nnet {

	class output;
	struct activity_state;

	class i_neuron_access;
	class i_connection_access;
	class i_layers;
	class i_modifiable;

	class i_neuralnet
	{
	public:
		virtual size_t neuron_count() const = 0;
		virtual size_t input_count() const = 0;
		virtual size_t output_count() const = 0;
		virtual size_t hidden_count() const = 0;

		virtual output run(input const& in) = 0;

		virtual bool provides_access(AccessOptions opt) const = 0;

		virtual std::unique_ptr< i_neuron_access > neuron_access() const = 0;

		virtual std::unique_ptr< i_connection_access > connection_access() const = 0;

		virtual std::unique_ptr< i_layers > layer_access() const = 0;

		//virtual std::unique_ptr< i_internal_state > internal_state_access() const = 0;
		virtual activity_state current_activity_state() const = 0;

		virtual std::unique_ptr< i_modifiable > modifiable() = 0;

	public:
		virtual ~i_neuralnet()
		{}
	};

}


#endif


