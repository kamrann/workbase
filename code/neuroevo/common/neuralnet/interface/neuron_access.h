// neuron_access.h

#ifndef __WB_NN_NEURON_ACCESS_H
#define __WB_NN_NEURON_ACCESS_H

#include "basic_types.h"
#include "neuron_filter.h"
#include "layer_filter.h"
#include "iterator_range.h"


namespace nnet {

	class neuron_data;
	struct layer_data;

	class i_neuron_access
	{
	public:
		virtual neuron_data get_neuron_data(neuron_id id) const = 0;
		virtual layer_data get_neuron_layer(neuron_id id) const = 0;
		virtual connection_range get_incoming_connections(neuron_id id) const = 0;
		virtual neuron_range get_range(layer_filter const& lfilter, neuron_filter const& nfilter) const = 0;

		inline neuron_range get_range() const
		{
			return get_range(layer_filter::Any, neuron_filter::Any);
		}
		inline neuron_range get_range(neuron_filter const& nfilter) const
		{
			return get_range(layer_filter::Any, nfilter);
		}
		inline neuron_range get_range(layer_filter const& lfilter) const
		{
			return get_range(lfilter, neuron_filter::Any);
		}

	public:
		virtual ~i_neuron_access()
		{}
	};

}


#endif


