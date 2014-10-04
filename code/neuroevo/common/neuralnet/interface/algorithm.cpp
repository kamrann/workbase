// algorithm.cpp

#include "algorithm.h"
#include "neuralnet.h"
#include "neuron_access.h"


namespace nnet {

	input_output_ranges calculate_neuron_io_ranges(neuron_id id, i_neuron_access const* access, input_output_range_map& existing)
	{
		auto it = existing.find(id);
		if(it != existing.end())
		{
			return it->second;
		}

		input_output_ranges io_ranges;

		auto nr = access->get_neuron_data(id);
		if(has_inputs(nr.type))
		{
			auto incoming = access->get_incoming_connections(id);
			activation_range sum_range{ 0.0 };	// TODO: Decide if empty range + some range should give some range, or be undefined
				// presumably former, in which case need to modify implementation, and then wouldnt need the 0.0 here
			for(auto const& cn : incoming)
			{
				auto const src = cn.src;
				auto const src_range = calculate_neuron_io_ranges(src, access, existing).output;
				auto signal_range = src_range * cn.weight;
				sum_range += signal_range;
			}

			io_ranges.input = sum_range;
			io_ranges.output = get_function_range(nr.activation_fn, sum_range);
		}
		else
		{
			switch(nr.type)
			{
				case NeuronType::Bias:
				{
					// TODO: bias stored in neuron?
					io_ranges.output = activation_range{ 1.0 };
				}
				break;

				default:
				{
					// Already dealt with by caller
					//io_ranges.output = activation_range::whole();
				}
				break;
			}
		}

		existing.insert(std::make_pair(id, io_ranges));
		return io_ranges;
	}

	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net, input_range const& input, std::set< neuron_id > const& neurons)
	{
		auto neuron_access = net->neuron_access();
		input_output_range_map mp;
		// TODO: Assuming here that neuron ids are 0 based and that the input neurons come before all others
		for(size_t n = 0; n < net->input_count(); ++n)
		{
			mp[n].output = input[n];
		}

		for(auto const& nr : neurons)
		{
			calculate_neuron_io_ranges(nr, neuron_access.get(), mp);
		}
		auto mp_it = std::begin(mp);
		while(mp_it != std::end(mp))
		{
			if(neurons.find(mp_it->first) == neurons.end())
			{
				mp_it = mp.erase(mp_it);
			}
			else
			{
				++mp_it;
			}
		}
		return mp;
	}

	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net, input_range const& input)
	{
		auto neuron_access = net->neuron_access();
		auto neurons = neuron_access->get_range();
		std::set< neuron_id > ids;
		std::transform(
			std::begin(neurons),
			std::end(neurons),
			std::inserter(ids, std::end(ids)),
			[](neuron_data const& nd)
		{
			return nd.id;
		});
		
		return calculate_input_output_ranges(net, input, ids);
	}

	input_output_ranges calculate_input_output_ranges(i_neuralnet const* net, input_range const& input, neuron_id neuron)
	{
		auto mp = calculate_input_output_ranges(net, input, std::set < neuron_id > { neuron });
		return mp.at(neuron);
	}

}



