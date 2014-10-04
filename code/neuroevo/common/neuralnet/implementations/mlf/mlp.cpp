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
#include "../../interface/modifiable.h"


namespace nnet {

	mlp::mlp()
	{
		reset();
	}

	void mlp::reset()
	{
		m_neurons.clear();
		m_connections.clear();
		m_conn_mp.clear();
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

	void mlp::set_activation_function(neuron_id neuron, activation_function fn)
	{
		m_neurons[neuron].activation_fn = fn;
	}

	void mlp::set_activation_function_layer(size_t layer, activation_function fn)
	{
		auto const start = layer_start(layer);
		auto const count = layer_count< ExcludeBias >(layer);
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].activation_fn = fn;
		}
	}

	void mlp::set_weight(connection_id conn, value_t weight)
	{
		m_connections[conn].weight = weight;
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
	
	neuron_range mlp::get_neurons(layer_filter const& lfilter, neuron_filter const& nfilter) const
	{
		// TODO: Optimize by considering lfilter and nfilter together.
		// -> eg. only look for Input neurons within Input layer.

		neuron_iterator::data_array_t data;

		// Iterate over our layers
		for(size_t ly = 0; ly < m_num_layers; ++ly)
		{
			// Construct layer data
			layer_data ld{
				ly == 0 ? LayerType::Input : ((ly == m_num_layers - 1) ? LayerType::Output : LayerType::Hidden)
			};
			if(ld.type == LayerType::Hidden)
			{
				ld.subindex = ly - 1;
			}

			// And skip if doesn't pass layer filter
			if(!lfilter.test(ld))
			{
				continue;
			}

			// Now iterate over neurons in the layer
			auto const start = layer_start(ly);
			auto const end = start + layer_count< IncludeBias >(ly);
			for(auto id = start; id < end; ++id)
			{
				// And test against neuron filter
				auto nd = as_neuron_data(id);
				if(nfilter.test(nd))
				{
					data.emplace_back(std::move(nd));
				}
			}
		}
		return neuron_range(
			neuron_iterator(std::move(data)),
			neuron_iterator(),
			data.size()
			);
	}

	layer_data mlp::neuron_layer(neuron_id id) const
	{
		for(size_t layer = 0; layer < m_num_layers; ++layer)
		{
			if(id < layer_start(layer) + layer_count< IncludeBias >(layer))
			{
				// TODO: encapsulate: layer_data_from_index(size_t idx)
				if(layer == 0)
				{
					return layer_data{ LayerType::Input };
				}
				else if(layer == m_num_layers - 1)
				{
					return layer_data{ LayerType::Output };
				}
				else
				{
					return layer_data{ LayerType::Hidden, layer - 1 };
				}
			}
		}

		throw std::exception("Problem");
	}

	connection_id mlp::get_connection_id(neuron_id src, neuron_id dst) const
	{
		return m_conn_mp.at(std::make_pair(src, dst));
	}

	connection_range mlp::get_connections(
		layer_filter const& src_lfilter,
		layer_filter const& dst_lfilter,
		neuron_filter const& src_nfilter,
		neuron_filter const& dst_nfilter
		) const
	{
		connection_iterator::data_array_t data;

		// Iterate over possible source layers
		for(size_t ly_src = 0; ly_src < m_num_layers - 1; ++ly_src)
		{
			auto const ly_dst = ly_src + 1;

			// Construct layer data
			layer_data ld_src{
				ly_src == 0 ? LayerType::Input : ((ly_src == m_num_layers - 1) ? LayerType::Output : LayerType::Hidden)
			};
			if(ld_src.type == LayerType::Hidden)
			{
				ld_src.subindex = ly_src - 1;
			}

			layer_data ld_dst{
				ly_dst == 0 ? LayerType::Input : ((ly_dst == m_num_layers - 1) ? LayerType::Output : LayerType::Hidden)
			};
			if(ld_src.type == LayerType::Hidden)
			{
				ld_src.subindex = ly_dst - 1;
			}

			// And skip if don't pass layer filters
			if(!src_lfilter.test(ld_src) || !dst_lfilter.test(ld_dst))
			{
				continue;
			}

			// Now iterate over neurons in src layer
			auto const src_start = layer_start(ly_src);
			auto const src_end = src_start + layer_count< IncludeBias >(ly_src);
			for(auto src_id = src_start; src_id < src_end; ++src_id)
			{
				// And test against src neuron filter
				auto src_nd = as_neuron_data(src_id);
				if(!src_nfilter.test(src_nd))
				{
					continue;
				}

				// Neuron passed source filter, so iterate over dst neurons
				// TODO: should just test each neuron against src/dest filters once at most!!
				auto const dst_start = layer_start(ly_dst);
				auto const dst_end = dst_start + layer_count< ExcludeBias >(ly_dst);
				for(auto dst_id = dst_start; dst_id < dst_end; ++dst_id)
				{
					auto dst_nd = as_neuron_data(dst_id);
					if(dst_nfilter.test(dst_nd))
					{
						auto const id = get_connection_id(src_id, dst_id);
						data.emplace_back(as_connection_data(id));
					}
				}
			}
		}
		return connection_range(
			connection_iterator(std::move(data)),
			connection_iterator(),
			data.size()
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

	connection_data mlp::as_connection_data(connection_id id) const
	{
		auto const& conn = m_connections[id];
		return connection_data{
			id,
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
			count += layer_count< IncludeBias >(i);
		}
		return count;
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
			m_neurons[n].activation_fn = ActivationFnType::None;
		}
	}

	void mlp::initialize_hidden_neurons()
	{
		auto const start = hidden_start();
		auto const count = total_hidden_count();
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].type = NeuronType::Hidden;
			m_neurons[n].activation_fn = ActivationFnType::Linear;
		}
	}

	void mlp::initialize_output_neurons()
	{
		auto const start = output_start();
		auto const count = num_outputs();
		for(size_t n = start; n < start + count; ++n)
		{
			m_neurons[n].type = NeuronType::Output;
			m_neurons[n].activation_fn = ActivationFnType::Linear;
		}
	}

	void mlp::initialize_bias_nodes()
	{
		// TODO: modifiable
		double const BiasActivation = 1.0;

		size_t idx = 0;
		for(size_t layer = 0; layer < m_num_layers - 1; ++layer)
		{
			idx += layer_count< IncludeBias >(layer);
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
			auto const src_count = layer_count< IncludeBias >(l_src);
			auto const dst_count = layer_count< ExcludeBias >(l_dest);
			count += src_count * dst_count;
		}
		
		m_connections.resize(count, connection{ 0, 0, 0.0 });

		// Order the connections by their destination first, so that the first n connections are from
		// every neuron in the input layer to the first neuron in the next layer (not every outgoing connection
		// from the first input).
		// This makes sense since when we sum up connection signals, we are doing so over incoming connections,
		// not outgoing ones.
		size_t c = 0;
		for(size_t l_src = 0; l_src < m_num_layers - 1; ++l_src)
		{
			auto const l_dest = l_src + 1;
			auto const src_count = layer_count< IncludeBias >(l_src);
			auto const dst_count = layer_count< ExcludeBias >(l_dest);

			auto const src_start = layer_start(l_src);
			auto const dst_start = layer_start(l_dest);
			for(size_t dst = dst_start; dst < dst_start + dst_count; ++dst)
			{
				for(size_t src = src_start; src < src_start + src_count; ++src, ++c)
				{
					m_connections[c].src = src;
					m_connections[c].dst = dst;
					m_conn_mp[std::make_pair(src, dst)] = c;
				}
			}
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
		size_t n = layer_count< IncludeBias >(0);
		size_t n_prev_base = 0;
		size_t c = 0;
		for(size_t layer = 1; layer < m_num_layers; ++layer)
		{
			auto const prev_count = layer_count< IncludeBias >(layer - 1);
			auto const count = layer_count< IncludeBias >(layer);
			// Ignore bias node
			auto const non_bias_count = count - num_bias(layer);

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
				m_neurons[n].activation = evaluate(m_neurons[n].activation_fn, m_neurons[n].sum);
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
		virtual layer_data get_neuron_layer(neuron_id id) const override;
		virtual connection_range get_incoming_connections(neuron_id id) const override;
		virtual neuron_range get_range(layer_filter const& lfilter, neuron_filter const& nfilter) const override;

	private:
		mlp const& m_nn;
	};

	neuron_data mlp::neuron_accessor::get_neuron_data(neuron_id id) const
	{
		return m_nn.as_neuron_data(id);
	}
	
	layer_data mlp::neuron_accessor::get_neuron_layer(neuron_id id) const
	{
		return m_nn.neuron_layer(id);
	}

	connection_range mlp::neuron_accessor::get_incoming_connections(neuron_id id) const
	{
		connection_iterator::data_array_t data;
		// TODO: Either store within neuron, or calculate from neuron layer, which can deduce from id
		connection_id cn_id = 0;
		for(auto const& conn : m_nn.m_connections)
		{
			if(conn.dst == id)
			{
				data.emplace_back(m_nn.as_connection_data(cn_id));
			}
			++cn_id;
		}
		return connection_range(
			connection_iterator(std::move(data)),
			connection_iterator(),
			data.size()
			);
	}

	neuron_range mlp::neuron_accessor::get_range(layer_filter const& lfilter, neuron_filter const& nfilter) const
	{
		return m_nn.get_neurons(lfilter, nfilter);
	}


	class mlp::connection_accessor:
		public i_connection_access
	{
	public:
		connection_accessor(mlp const& nn): m_nn(nn)
		{}

	public:
		virtual connection_data get_connection_data(connection_id id) const override;
		virtual connection_range get_range(
			layer_filter const& src_lfilter,
			layer_filter const& dst_lfilter,
			neuron_filter const& src_nfilter,
			neuron_filter const& dst_nfilter
			) const override;

	private:
		mlp const& m_nn;
	};

	connection_data mlp::connection_accessor::get_connection_data(connection_id id) const
	{
		return m_nn.as_connection_data(id);
	}

	connection_range mlp::connection_accessor::get_range(
		layer_filter const& src_lfilter,
		layer_filter const& dst_lfilter,
		neuron_filter const& src_nfilter,
		neuron_filter const& dst_nfilter
		) const
	{
		return m_nn.get_connections(src_lfilter, dst_lfilter, src_nfilter, dst_nfilter);
	}


	class mlp::modifier:
		public i_modifiable
	{
	public:
		modifier(mlp& nn);

	public:
		virtual void set_activation_fn(neuron_id neuron, activation_function fn) override;
		virtual void set_weight(connection_id conn, value_t weight) override;

	private:
		mlp& m_nn;
	};

	mlp::modifier::modifier(mlp& nn): m_nn(nn)
	{}

	void mlp::modifier::set_activation_fn(neuron_id neuron, activation_function fn)
	{
		m_nn.set_activation_function(neuron, fn);
	}

	void mlp::modifier::set_weight(connection_id conn, value_t weight)
	{
		m_nn.set_weight(conn, weight);
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
		return num_hidden();
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
			case AccessOptions::Modification:
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
		return std::make_unique< connection_accessor >(*this);
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

	std::unique_ptr< i_modifiable > mlp::modifiable()
	{
		return std::make_unique< modifier >(*this);
	}

}