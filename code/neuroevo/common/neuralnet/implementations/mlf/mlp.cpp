// mlp.cpp

#include "mlp.h"

#include "../../interface/input.h"
#include "../../interface/output.h"

#include "../../interface/neuron_access.h"
#include "../../interface/iterator.h"
#include "../../interface/iterator_range.h"
#include "../../interface/connection_access.h"
#include "../../interface/layer_access.h"
#include "../../interface/internal_state_access.h"


namespace nnet {

	mlp::mlp()
	{
		reset();
	}

	void mlp::reset()
	{
		m_neurons.clear();
		m_connections.clear();
		m_num_layers = 0;
		m_layer_counts.clear();
	}

	void mlp::create(layer_counts_t const& layer_counts)
	{
		reset();

		m_num_layers = layer_counts.size();
		for(size_t layer = 0; layer < m_num_layers; ++layer)
		{
			m_layer_counts.push_back(layer_counts[layer] + num_bias(layer));
		}
		create_neurons();
		create_connections();
	}

	void mlp::set_activation_function(size_t layer, ActivationFn fn)
	{
		auto const start = layer_start(layer);
		auto const count = layer_count(layer);
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].activation_fn = fn;
		}
	}

	size_t mlp::neuron_count(neuron_filter const& filter) const
	{
		// TODO: Unnecessarily slow, since we know layer information
		size_t const count = m_neurons.size();
		size_t pass = 0;
		for(size_t id = 0; id < count; ++id)
		{
			if(filter.test(as_neuron_data(id)))
			{
				++pass;
			}
		}
		return pass;
	}
	
	neuron_range mlp::get_neurons(neuron_filter const& filter) const
	{
		// TODO: Unnecessarily slow, since we know layer information
		neuron_iterator::data_array_t data;
		size_t const count = m_neurons.size();
		for(size_t id = 0; id < count; ++id)
		{
			auto nd = as_neuron_data(id);
			if(filter.test(nd))
			{
				data.emplace_back(std::move(nd));
			}
		}
		return neuron_range(
			neuron_iterator(std::move(data)),
			neuron_iterator()
			);
	}


	neuron_data mlp::as_neuron_data(neuron_id id) const
	{
		auto const& nr = m_neurons[id];
		return neuron_data{
			id,
			nr.type,
			nr.activation_fn
		};
	}

	connection_data mlp::as_connection_data(connection const& conn) const
	{
		return connection_data{
			conn.src,
			conn.dst,
			conn.weight
		};
	}

	size_t mlp::num_bias(size_t layer) const
	{
		return is_output_layer(layer) ? 0 : 1;
	}

	size_t mlp::input_start() const
	{
		return 0;
	}

	size_t mlp::hidden_start(size_t hidden_layer) const
	{
		return layer_start(1 + hidden_layer);
	}

	size_t mlp::output_start() const
	{
		return total_neuron_count() - num_outputs();
	}

	size_t mlp::layer_start(size_t layer) const
	{
		size_t count = 0;
		for(size_t i = 0; i < layer; ++i)
		{
			count += m_layer_counts[i];
		}
		return count;
	}

	size_t mlp::layer_count(size_t layer) const
	{
		return m_layer_counts[layer] - num_bias(layer);
	}


	void mlp::create_neurons()
	{
		size_t count = 0;
		for(auto const lc : m_layer_counts)
		{
			count += lc;
		}
		m_neurons.resize(count);

		initialize_input_neurons();
		initialize_hidden_neurons();
		initialize_output_neurons();
		initialize_bias_nodes();
	}

	void mlp::initialize_input_neurons()
	{
		auto const start = input_start();
		auto const count = num_inputs();
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].type = NeuronType::Input;
			m_neurons[n].activation_fn = ActivationFn::None;
		}
	}

	void mlp::initialize_hidden_neurons()
	{
		auto const start = hidden_start();
		auto const count = total_hidden_count();
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].type = NeuronType::Hidden;
			m_neurons[n].activation_fn = ActivationFn::Linear;
		}
	}

	void mlp::initialize_output_neurons()
	{
		auto const start = output_start();
		auto const count = num_outputs();
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].type = NeuronType::Output;
			m_neurons[n].activation_fn = ActivationFn::Linear;
		}
	}

	void mlp::initialize_bias_nodes()
	{
		// TODO: modifiable
		double const BiasActivation = 1.0;

		size_t idx = 0;
		for(size_t layer = 0; layer < m_num_layers - 1; ++layer)
		{
			idx += m_layer_counts[layer];
			size_t bias = idx - 1;
			m_neurons[bias].type = NeuronType::Bias;
			m_neurons[bias].activation = BiasActivation;
		}
	}

	void mlp::create_connections()
	{
		size_t count = 0;
		for(size_t l_src = 0; l_src < m_num_layers - 1; ++l_src)
		{
			auto const l_dest = l_src + 1;
			auto const src_count = m_layer_counts[l_src];
			auto const dst_count = m_layer_counts[l_dest] - num_bias(l_dest);
			count += src_count * dst_count;
		}
		
		m_connections.resize(count, connection{ 0, 0, 0.0 });

		size_t c = 0;
		for(size_t l_src = 0; l_src < m_num_layers - 1; ++l_src)
		{
			auto const l_dest = l_src + 1;
			auto const src_count = m_layer_counts[l_src];
			auto const dst_count = m_layer_counts[l_dest] - num_bias(l_dest);

			auto src_start = layer_start(l_src);
			auto dst_start = layer_start(l_dest);
			for(size_t src = src_start; src < src_start + src_count; ++src)
			{
				for(size_t dst = dst_start; dst < dst_start + dst_count; ++dst, ++c)
				{
					m_connections[c].src = src;
					m_connections[c].dst = dst;
				}
			}

			count += m_layer_counts[l_src] * (m_layer_counts[l_dest] - num_bias(l_dest));
		}
	}

	bool mlp::load_weights(weight_array_t const& weights)
	{
		if(weights.size() != num_connections())
		{
			return false;
		}

		for(size_t c = 0; c < num_connections(); ++c)
		{
			m_connections[c].weight = weights[c];
		}
		return true;
	}

	output mlp::execute(input const& in)
	{
		// Set activation of input neurons
		auto const num_input = num_inputs();
		for(size_t n = 0; n < num_input; ++n)
		{
			m_neurons[n].activation = in[n];
		}

		// Loop over all subsequent layers
		size_t n = m_layer_counts[0];
		size_t n_prev_base = 0;
		size_t c = 0;
		for(size_t layer = 1; layer < m_num_layers; ++layer)
		{
			auto const prev_count = m_layer_counts[layer - 1];
			auto const count = m_layer_counts[layer];
			// Ignore bias node
			auto const non_bias_count = is_output_layer(layer) ? count : (count - 1);

			// For every neuron in the layer (except the bias)...
			for(size_t n_local = 0; n_local < non_bias_count; ++n_local, ++n)
			{
				// Calculate the neuron's activation
				m_neurons[n].sum = 0.0;
				// Need to loop over every incoming connection
				for(size_t ic = 0, n_prev = n_prev_base; ic < prev_count; ++ic, ++c, ++n_prev)
				{
					m_connections[c].signal = m_neurons[n_prev].activation * m_connections[c].weight;
					m_neurons[n].sum += m_connections[c].signal;
				}

				// Activation function
				// TODO:
				m_neurons[n].activation = m_neurons[n].sum;
			}

			// Skip over bias
			++n;

			n_prev_base = n - count;
		}

		// Return the output values
		output out{ num_outputs() };
		size_t out_n = m_neurons.size() - num_outputs();
		for(size_t i = 0; i < num_outputs(); ++i, ++out_n)
		{
			out[i] = m_neurons[out_n].activation;
		}
		return out;
	}


	/********** i_neural_net interface ***********/

	class mlp::neuron_accessor:
		public i_neuron_access
	{
	public:
		neuron_accessor(mlp const& nn): m_nn(nn)
		{}

	public:
		virtual neuron_data get_neuron_data(neuron_id id) const override;
		virtual connection_range get_incoming_connections(neuron_id id) const override;
		virtual neuron_range get_range(neuron_filter const& filter) const override;

	private:
		mlp const& m_nn;
	};

	neuron_data mlp::neuron_accessor::get_neuron_data(neuron_id id) const
	{
		return m_nn.as_neuron_data(id);
	}
	
	connection_range mlp::neuron_accessor::get_incoming_connections(neuron_id id) const
	{
		connection_iterator::data_array_t data;
		// TODO: Either store within neuron, or calculate from neuron layer, which can deduce from id
		for(auto const& conn : m_nn.m_connections)
		{
			if(conn.dst == id)
			{
				data.emplace_back(m_nn.as_connection_data(conn));
			}
		}
		return connection_range(
			connection_iterator(std::move(data)),
			connection_iterator()
			);
	}

	neuron_range mlp::neuron_accessor::get_range(neuron_filter const& filter) const
	{
		return m_nn.get_neurons(filter);
	}


	class mlp::connection_accessor:
		public i_connection_access
	{
	public:
		connection_accessor(
			connection_list_t::const_iterator start,
			connection_list_t::const_iterator end
			);

	public:
		virtual size_t count() const override;

		virtual connection_iterator begin() override;
		virtual connection_iterator end() override;

	private:
		connection_list_t::const_iterator m_start, m_end;
	};

	mlp::connection_accessor::connection_accessor(
		connection_list_t::const_iterator start,
		connection_list_t::const_iterator end
		):
		m_start(start),
		m_end(end)
	{}

	size_t mlp::connection_accessor::count() const
	{
		return m_end - m_start;
	}

	connection_iterator mlp::connection_accessor::begin()
	{
		connection_iterator::data_array_t data(count());
		size_t n = 0;
		for(auto it = m_start; it != m_end; ++it, ++n)
		{
			data[n].src = it->src;
			data[n].dst = it->dst;
			data[n].weight = it->weight;
		}
		return connection_iterator(std::move(data));
	}

	connection_iterator mlp::connection_accessor::end()
	{
		return connection_iterator();
	}


	size_t mlp::neuron_count() const
	{
		return total_neuron_count();
	}
		
	size_t mlp::input_count() const
	{
		return num_inputs();
	}
		
	size_t mlp::output_count() const
	{
		return num_outputs();
	}

	size_t mlp::hidden_count() const
	{
		return total_hidden_count();
	}
		
	output mlp::run(input const& in)
	{
		return execute(in);
	}

	bool mlp::provides_access(AccessOptions opt) const
	{
		switch(opt)
		{
			case AccessOptions::Neurons:
			case AccessOptions::Layers:
			case AccessOptions::State:
			return true;

			default:
			return false;
		}
	}
		
	std::unique_ptr< i_neuron_access > mlp::neuron_access() const
	{
		return std::make_unique< neuron_accessor >(*this);
	}
		
	std::unique_ptr< i_connection_access > mlp::connection_access() const
	{
		return std::make_unique< connection_accessor >(
			std::begin(m_connections),
			std::end(m_connections)
			);
	}

	std::unique_ptr< i_layers > mlp::layer_access() const
	{
		return nullptr;
	}
		
	activity_state mlp::current_activity_state() const
	{
		activity_state state;
		for(size_t n = 0; n < m_neurons.size(); ++n)
		{
			state.neurons[n].sum = m_neurons[n].sum;
			state.neurons[n].activation = m_neurons[n].activation;
		}
		for(size_t c = 0; c < m_connections.size(); ++c)
		{
			auto conn = std::make_pair(m_connections[c].src, m_connections[c].dst);
			state.connections[conn].signal = m_connections[c].signal;
		}
		return state;
	}

}