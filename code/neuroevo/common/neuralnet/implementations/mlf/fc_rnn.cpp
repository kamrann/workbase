// fc_rnn.cpp

#include "fc_rnn.h"

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

	fc_rnn::fc_rnn()
	{
		reset();
	}

	void fc_rnn::reset()
	{
		vInput.resize(0);
		vHidden.resize(0);
		vOutput.resize(0);

		af_hidden = activation_function{ ActivationFnType::SigmoidSymmetric };
		af_output = activation_function{ ActivationFnType::SigmoidSymmetric };

		m_connections.clear();
	}

	void fc_rnn::create(size_t num_input, size_t num_hidden, size_t num_output, value_t bias)
	{
		reset();

		vInput.resize(num_input + 1);	// Single bias neuron in the input layer
		vInput.setZero();
		vInput[num_input] = bias;
		vHidden.resize(num_hidden);
		vHidden.setZero();
		vOutput.resize(num_output);
		vOutput.setZero();

		wmInput.resize(num_hidden, num_input + 1);
		wmInput.setZero();
		wmHidden.resize(num_hidden, num_hidden);
		wmHidden.setZero();
		wmOutput.resize(num_output, num_input + 1 + num_hidden);
		wmOutput.setZero();

		initialize_connection_map();
	}

	size_t fc_rnn::calculate_connection_count() const
	{
		return
			input_layer_count() * hidden_count() +
			hidden_count() * hidden_count() +
			(input_layer_count() + hidden_count()) * output_count()
			;
	}

	void fc_rnn::initialize_connection_map()
	{
		connection_id id = 0;

		// Input->Hidden
		for(neuron_id in = 0; in < input_layer_count(); ++in)
		{
			for(neuron_id hid = 0; hid < hidden_count(); ++hid)
			{
				m_connections.insert({ id, std::make_pair(input_offset() + in, hidden_offset() + hid) });
				++id;
			}
		}

		// Hidden->Hidden
		for(neuron_id hid1 = 0; hid1 < hidden_count(); ++hid1)
		{
			for(neuron_id hid2 = 0; hid2 < hidden_count(); ++hid2)
			{
				m_connections.insert({ id, std::make_pair(hidden_offset() + hid1, hidden_offset() + hid2) });
				++id;
			}
		}

		// Input,Hidden->Output
		for(neuron_id ih = 0; ih < input_layer_count() + hidden_count(); ++ih)
		{
			for(neuron_id out = 0; out < output_count(); ++out)
			{
				m_connections.insert({ id, std::make_pair(input_offset() + ih, output_offset() + out) });
				++id;
			}
		}
	}

	void fc_rnn::set_activation_function_hidden(activation_function fn)
	{
		af_hidden = fn;
	}

	void fc_rnn::set_activation_function_output(activation_function fn)
	{
		af_output = fn;
	}

	void fc_rnn::set_weight(connection_id conn, value_t weight)
	{
		connection_weight(conn) = weight;
	}

	size_t fc_rnn::neuron_count(neuron_filter const& filter) const
	{
		// TODO: Unnecessarily slow, since we know layer information
		size_t const count = neuron_count();
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

	neuron_range fc_rnn::get_neurons(layer_filter const& lfilter, neuron_filter const& nfilter) const
	{
		// TODO: Optimize by considering lfilter and nfilter together.
		// -> eg. only look for Input neurons within Input layer.

		neuron_iterator::data_array_t data;

		auto const NumLayerTypes = 3;
		auto const layer_types = std::array < LayerType, NumLayerTypes > {
			{ LayerType::Input, LayerType::Hidden, LayerType::Output }
		};

		// Iterate over our layers
		for(size_t ly = 0; ly < NumLayerTypes; ++ly)
		{
			// Construct layer data
			layer_data ld{
				layer_types[ly]
			};

			// And skip if doesn't pass layer filter
			if(!lfilter.test(ld))
			{
				continue;
			}

			// Now iterate over neurons in the layer
			auto const start = layer_offset(layer_types[ly]);
			auto const end = start + layer_count(layer_types[ly]);
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

	layer_data fc_rnn::neuron_layer(neuron_id id) const
	{
		if(id < input_layer_count())
		{
			return layer_data{ LayerType::Input };
		}

		id -= input_layer_count();
		if(id < hidden_count())
		{
			return layer_data{ LayerType::Hidden };
		}

		id -= hidden_count();
		if(id < output_count())
		{
			return layer_data{ LayerType::Output };
		}

		throw std::runtime_error("invalid neuron id");
	}

	connection_id fc_rnn::get_connection_id(neuron_id src, neuron_id dst) const
	{
		return m_connections.right.at(std::make_pair(src, dst));
	}

	connection_range fc_rnn::get_connections(
		layer_filter const& src_lfilter,
		layer_filter const& dst_lfilter,
		neuron_filter const& src_nfilter,
		neuron_filter const& dst_nfilter
		) const
	{
		connection_iterator::data_array_t data;

		auto const src_layers = std::array < LayerType, 2 > {
			{ LayerType::Input, LayerType::Hidden }
		};
		auto const dst_layers = std::map< LayerType, std::array < LayerType, 2 > >{
			{ LayerType::Input, { { LayerType::Hidden, LayerType::Output } } },
			{ LayerType::Hidden, { { LayerType::Hidden, LayerType::Output } } }
		};

		// Iterate over possible source layers
		for(auto ly_src : src_layers)
		{
			layer_data ld_src{ ly_src };
			// Skip if don't pass layer filters
			if(!src_lfilter.test(ld_src))
			{
				continue;
			}

			for(auto ly_dst : dst_layers.at(ly_src))
			{
				layer_data ld_dst{ ly_dst };
				// Skip if don't pass layer filters
				if(!dst_lfilter.test(ld_dst))
				{
					continue;
				}

				// Now iterate over neurons in src layer
				auto const src_start = layer_offset(ly_src);
				auto const src_end = src_start + layer_count(ly_src);
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
					auto const dst_start = layer_offset(ly_dst);
					auto const dst_end = dst_start + layer_count(ly_dst);
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
		}
		return connection_range(
			connection_iterator(std::move(data)),
			connection_iterator(),
			data.size()
			);
	}


	neuron_data fc_rnn::as_neuron_data(neuron_id id) const
	{
		auto rel_id = id;
		if(rel_id < input_layer_count())
		{
			neuron_data nd{ id, NeuronType::Input, activation_function{} };
			if(rel_id == input_layer_count() - 1)
			{
				nd.type = NeuronType::Bias;
			}
			return nd;
		}

		rel_id -= input_layer_count();
		if(rel_id < hidden_count())
		{
			return neuron_data{ id, NeuronType::Hidden, af_hidden };
		}

		rel_id -= hidden_count();
		return neuron_data{ id, NeuronType::Output, af_output };
	}

	value_t fc_rnn::connection_weight(connection_id id) const
	{
		if(id < wmInput.size())
		{
			return wmInput(id);
		}

		id -= wmInput.size();
		if(id < wmHidden.size())
		{
			return wmHidden(id);
		}

		id -= wmHidden.size();
		return wmOutput(id);
	}

	value_t& fc_rnn::connection_weight(connection_id id)
	{
		if(id < wmInput.size())
		{
			return wmInput(id);
		}

		id -= wmInput.size();
		if(id < wmHidden.size())
		{
			return wmHidden(id);
		}

		id -= wmHidden.size();
		return wmOutput(id);
	}

	connection_data fc_rnn::as_connection_data(connection_id id) const
	{
		auto const& conn = m_connections.left.at(id);
		return connection_data{
			id,
			conn.first,
			conn.second,
			connection_weight(id)
		};
	}

	size_t fc_rnn::input_offset() const
	{
		return 0;
	}

	size_t fc_rnn::hidden_offset() const
	{
		return input_layer_count();
	}

	size_t fc_rnn::output_offset() const
	{
		return input_layer_count() + hidden_count();
	}

	size_t fc_rnn::layer_offset(LayerType ly) const
	{
		switch(ly)
		{
			case LayerType::Input:		return 0;
			case LayerType::Hidden:		return hidden_offset();
			case LayerType::Output:		return output_offset();
			default:					throw std::runtime_error("invalid layer type");
		}
	}

	// todo: option to exclude bias
	size_t fc_rnn::layer_count(LayerType ly) const
	{
		switch(ly)
		{
			case LayerType::Input:		return input_layer_count();
			case LayerType::Hidden:		return hidden_count();
			case LayerType::Output:		return output_count();
			default:					throw std::runtime_error("invalid layer type");
		}
	}

	bool fc_rnn::load_weights(weight_array_t const& weights)
	{
		if(weights.size() != num_connections())
		{
			return false;
		}

		auto it1 = std::begin(weights);
		auto it2 = it1 + wmInput.size();
		std::copy(it1, it2, wmInput.data());
		it1 = it2;
		it2 = it1 + wmHidden.size();
		std::copy(it1, it2, wmHidden.data());
		it1 = it2;
		it2 = it1 + wmOutput.size();
		std::copy(it1, it2, wmOutput.data());
		return true;
	}

	output fc_rnn::execute(input const& in)
	{
		// Set activation of input neurons
		auto const num_input = in.size();
		for(size_t n = 0; n < num_input; ++n)
		{
			vInput[n] = in[n];
		}

		// Summation for hidden neurons
		Eigen::VectorXd vHiddenSums =
			wmInput * vInput +
			wmHidden * vHidden;
		// Transfer function
		vHidden =
			evaluate(af_hidden, vHiddenSums.array());

		// TODO: Maybe should just store as a single vector?
		Eigen::VectorXd joined(input_layer_count() + hidden_count());
		joined << vInput, vHidden;
		Eigen::VectorXd vOutputSums =
			wmOutput * joined;
		Eigen::VectorXd vOutput =
			evaluate(af_output, vOutputSums.array());

		// Return the output values
		output out{ output_count() };
		std::copy(vOutput.data(), vOutput.data() + output_count(), out.begin());
		return out;
	}


	/********** i_neural_net interface ***********/

	std::string fc_rnn::network_type_name() const
	{
		return "rnn";
	}

	class fc_rnn::neuron_accessor:
		public i_neuron_access
	{
	public:
		neuron_accessor(fc_rnn const& nn): m_nn(nn)
		{}

	public:
		virtual neuron_data get_neuron_data(neuron_id id) const override;
		virtual layer_data get_neuron_layer(neuron_id id) const override;
		virtual connection_range get_incoming_connections(neuron_id id) const override;
		virtual neuron_range get_range(layer_filter const& lfilter, neuron_filter const& nfilter) const override;

	private:
		fc_rnn const& m_nn;
	};

	neuron_data fc_rnn::neuron_accessor::get_neuron_data(neuron_id id) const
	{
		return m_nn.as_neuron_data(id);
	}
	
	layer_data fc_rnn::neuron_accessor::get_neuron_layer(neuron_id id) const
	{
		return m_nn.neuron_layer(id);
	}

	connection_range fc_rnn::neuron_accessor::get_incoming_connections(neuron_id id) const
	{
		connection_iterator::data_array_t data;
		auto ly = m_nn.neuron_layer(id);
		switch(ly.type)
		{
			case LayerType::Hidden:
			{
				// offset of neuron within its layer
				// corresponds to a row in the connection matrices
				auto offset = id - m_nn.hidden_offset();

				// iterate over the neurons in the input layer
				// (ie. iterate over columns of the input->hidden matrix)
				for(size_t i = 0, cn = offset; i < m_nn.input_layer_count(); ++i, cn += m_nn.hidden_count())
				{
					data.push_back(m_nn.as_connection_data(cn));
				}

				// iterate over the neurons in the hidden layer 
				// (ie. iterate over columns of the hidden->hidden matrix)
				for(size_t i = 0, cn = m_nn.input_layer_count() * m_nn.hidden_count() + offset;
					i < m_nn.hidden_count(); ++i, cn += m_nn.hidden_count())
				{
					data.push_back(m_nn.as_connection_data(cn));
				}

			}
			break;

			case LayerType::Output:
			{
				// offset of neuron within its layer
				// corresponds to a row in the connection matrix
				auto offset = id - m_nn.output_offset();

				// iterate over the neurons in the input/hidden layers
				// (ie. iterate over columns of the input/hidden->output matrix)
				for(size_t i = 0, cn = m_nn.input_layer_count() * m_nn.hidden_count() + m_nn.hidden_count() * m_nn.hidden_count();
					i < m_nn.input_layer_count() + m_nn.hidden_count(); ++i, cn += m_nn.output_count())
				{
					data.push_back(m_nn.as_connection_data(cn));
				}
			}
			break;

			// No incoming connections for neurons in input layer
		}

		return connection_range(
			connection_iterator(std::move(data)),
			connection_iterator(),
			data.size()
			);
	}

	neuron_range fc_rnn::neuron_accessor::get_range(layer_filter const& lfilter, neuron_filter const& nfilter) const
	{
		return m_nn.get_neurons(lfilter, nfilter);
	}


	class fc_rnn::connection_accessor:
		public i_connection_access
	{
	public:
		connection_accessor(fc_rnn const& nn): m_nn(nn)
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
		fc_rnn const& m_nn;
	};

	connection_data fc_rnn::connection_accessor::get_connection_data(connection_id id) const
	{
		return m_nn.as_connection_data(id);
	}

	connection_range fc_rnn::connection_accessor::get_range(
		layer_filter const& src_lfilter,
		layer_filter const& dst_lfilter,
		neuron_filter const& src_nfilter,
		neuron_filter const& dst_nfilter
		) const
	{
		return m_nn.get_connections(src_lfilter, dst_lfilter, src_nfilter, dst_nfilter);
	}


	class fc_rnn::modifier:
		public i_modifiable
	{
	public:
		modifier(fc_rnn& nn);

	public:
//		virtual void set_activation_fn(neuron_id neuron, activation_function fn) override;
		virtual void set_weight(connection_id conn, value_t weight) override;

	private:
		fc_rnn& m_nn;
	};

	fc_rnn::modifier::modifier(fc_rnn& nn): m_nn(nn)
	{}

/*	void fc_rnn::modifier::set_activation_fn(neuron_id neuron, activation_function fn)
	{
		m_nn.set_activation_function(neuron, fn);
	}
*/
	void fc_rnn::modifier::set_weight(connection_id conn, value_t weight)
	{
		m_nn.set_weight(conn, weight);
	}


	size_t fc_rnn::neuron_count() const
	{
		return input_layer_count() + hidden_count() + output_count();
	}
		
	size_t fc_rnn::input_count() const
	{
		// NOTE: Excludes bias
		return input_layer_count() - 1;
	}
		
	size_t fc_rnn::output_count() const
	{
		return vOutput.size();
	}

	size_t fc_rnn::hidden_count() const
	{
		return vHidden.size();
	}

	void fc_rnn::reset_state()
	{
		vInput.topRows(input_count()).setZero();	// todo: not sure vInput should even be a member should it?
			// only used within execute() i think, probably no need for it to persist
		vHidden.setZero();
		vOutput.setZero();
	}
		
	output fc_rnn::run(input const& in)
	{
		return execute(in);
	}

	bool fc_rnn::provides_access(AccessOptions opt) const
	{
		switch(opt)
		{
			case AccessOptions::Neurons:
			case AccessOptions::State:
			case AccessOptions::Modification:
			return true;

			default:
			return false;
		}
	}
		
	std::unique_ptr< i_neuron_access > fc_rnn::neuron_access() const
	{
		return std::make_unique< neuron_accessor >(*this);
	}
		
	std::unique_ptr< i_connection_access > fc_rnn::connection_access() const
	{
		return std::make_unique< connection_accessor >(*this);
	}

	std::unique_ptr< i_layers > fc_rnn::layer_access() const
	{
		return nullptr;
	}
		
	activity_state fc_rnn::current_activity_state() const
	{
		activity_state state;
/* TODO:
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
*/
		return state;
	}

	std::unique_ptr< i_modifiable > fc_rnn::modifiable()
	{
		return std::make_unique< modifier >(*this);
	}

}