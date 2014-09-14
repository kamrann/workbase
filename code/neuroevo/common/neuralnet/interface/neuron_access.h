// neuron_access.h

#ifndef __WB_NN_NEURON_ACCESS_H
#define __WB_NN_NEURON_ACCESS_H

#include "basic_types.h"
#include "neuron_filter.h"
#include "iterator_range.h"


namespace nnet {

	class neuron_data;

	class i_neuron_access
	{
	public:
//		virtual size_t count() const = 0;

//		virtual neuron_iterator begin() = 0;
//		virtual neuron_iterator end() = 0;

		virtual neuron_data get_neuron_data(neuron_id id) const = 0;
		virtual connection_range get_incoming_connections(neuron_id id) const = 0;
		virtual neuron_range get_range(neuron_filter const& filter = neuron_filter::Any) const = 0;

	public:
		virtual ~i_neuron_access()
		{}
	};

}


#endif


