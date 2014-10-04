// connection_access.h

#ifndef __WB_NN_CONNECTION_ACCESS_H
#define __WB_NN_CONNECTION_ACCESS_H

#include "iterator.h"


namespace nnet {

	class i_connection_access
	{
	public:
		virtual connection_data get_connection_data(connection_id id) const = 0;
		virtual connection_range get_range(
			layer_filter const& src_lfilter,
			layer_filter const& dst_lfilter,
			neuron_filter const& src_nfilter,
			neuron_filter const& dst_nfilter
			) const = 0;

		inline connection_range get_range() const
		{
			return get_range(layer_filter::Any, layer_filter::Any, neuron_filter::Any, neuron_filter::Any);
		}
		inline connection_range get_range(neuron_filter const& src_nfilter, neuron_filter const& dst_nfilter) const
		{
			return get_range(layer_filter::Any, layer_filter::Any, src_nfilter, dst_nfilter);
		}
		inline connection_range get_range(layer_filter const& src_lfilter, layer_filter const& dst_lfilter) const
		{
			return get_range(src_lfilter, dst_lfilter, neuron_filter::Any, neuron_filter::Any);
		}

	public:
		virtual ~i_connection_access()
		{}
	};

}


#endif


