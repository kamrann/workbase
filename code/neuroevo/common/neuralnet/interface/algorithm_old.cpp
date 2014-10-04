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

		activation_range input_r{ activation_range::unbounded() };
		activation_range output_r{ activation_range::unbounded() };
		auto nr = access->get_neuron_data(id);
		if(has_inputs(nr.type))
		{
			// TODO: For now assuming each range is contiguous with no gaps
			// Not true when mixing continuous and discrete inputs!!
			double minimum = 0.0;
			double maximum = 0.0;

			auto incoming = access->get_incoming_connections(id);
			for(auto const& cn : incoming)
			{
				auto const src = cn.src;
				auto const src_range = calculate_neuron_io_ranges(src, access, existing).output;
				auto signal_range = src_range;
				signal_range.multiply_by(cn.weight);

				if(minimum != -activation_range::infinity())
				{
					if(signal_range.is_bounded_below())
					{
						minimum += signal_range.lower_bound;
					}
					else
					{
						minimum = -activation_range::infinity();
					}
				}

				if(maximum != activation_range::infinity())
				{
					if(signal_range.is_bounded_above())
					{
						maximum += signal_range.upper_bound;
					}
					else
					{
						maximum = activation_range::infinity();
					}
				}
			}

			// TODO: Assuming always continuous
			input_r = activation_range{ minimum, maximum, false };
			output_r = get_function_range(nr.activation_fn, input_r);
		}
		else
		{
			switch(nr.type)
			{
				case NeuronType::Bias:
				{
					// TODO: bias stored in neuron?
					input_r = output_r = activation_range{ 1.0, 1.0, true };
				}
				break;

				default:
				// Treat input neurons as having unbounded range (already defaulted above)
				break;
			}
		}

		auto io_ranges = input_output_ranges{ input_r, output_r };
		existing.insert(std::make_pair(id, io_ranges));
		return io_ranges;
	}

	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net, std::set< neuron_id > const& neurons)
	{
		auto neuron_access = net->neuron_access();
		auto neurons = neuron_access->get_range();
		input_output_range_map mp;
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

	input_output_range_map calculate_input_output_ranges(i_neuralnet const* net)
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
		
		return calculate_input_output_ranges(net, ids);
	}

	input_output_ranges calculate_input_output_ranges(i_neuralnet const* net, neuron_id neuron)
	{
		auto mp = calculate_input_output_ranges(net, std::set < neuron_id > { neuron });
		return mp.at(neuron);
	}

}



