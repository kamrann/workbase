// mlp.h

#ifndef __WB_MLF_MLP_H
#define __WB_MLF_MLP_H

#include "../../interface/neuralnet.h"
#include "../../interface/neuron_type.h"
#include "../../interface/activation_functions.h"
#include "../../interface/neuron_filter.h"
#include "../../interface/iterator_range.h"


namespace nnet {

	class connection_data;

	class mlp:
		public i_neuralnet
	{
	public:
		mlp();

	public:
		typedef std::vector< size_t > layer_counts_t;

	private:
		struct neuron
		{
			NeuronType type;
			ActivationFn activation_fn;
			value_t sum;
			value_t activation;
		};

		struct connection
		{
			size_t src;
			size_t dst;
			weight_t weight;
			value_t signal;
		};

		typedef std::vector< neuron > neuron_list_t;
		typedef std::vector< connection > connection_list_t;

	public:
		size_t neuron_count(neuron_filter const& filter = neuron_filter()) const;
		neuron_range get_neurons(neuron_filter const& filter = neuron_filter()) const;

		inline size_t num_inputs() const
		{
			return m_layer_counts.front() - 1;	// Bias node
		}

		inline size_t num_outputs() const
		{
			return m_layer_counts.back();	// No bias node at output layer
		}

		// Neuron counts include bias neurons
		inline size_t total_neuron_count() const
		{
			return m_neurons.size();
		}

		inline size_t total_hidden_count() const
		{
			size_t count = 0;
			for(size_t layer = 1; layer < m_num_layers - 1; ++layer)
			{
				count += m_layer_counts[layer];
			}
			return count;
		}

		inline size_t num_connections() const
		{
			return m_connections.size();
		}

		inline bool is_output_layer(size_t layer) const
		{
			return layer == m_num_layers - 1;
		}

		void reset();
		void create(layer_counts_t const& layer_counts);
		void set_activation_function(size_t layer, ActivationFn fn);
		bool load_weights(weight_array_t const& weights);
		output execute(input const& in);

	private:
		neuron_data as_neuron_data(neuron_id id) const;
		connection_data as_connection_data(connection const& conn) const;
		size_t num_bias(size_t layer) const;
		size_t input_start() const;
		size_t hidden_start(size_t hidden_layer = 0) const;	// 0 = first hidden layer
		size_t output_start() const;
		size_t layer_start(size_t layer) const;	// 0 = input layer, 1 = first hidden layer
		size_t layer_count(size_t layer) const;

		void create_neurons();
		void initialize_input_neurons();
		void initialize_hidden_neurons();
		void initialize_output_neurons();
		void initialize_bias_nodes();
		void create_connections();

	private:
		neuron_list_t m_neurons;
		connection_list_t m_connections;
		size_t m_num_layers;
		layer_counts_t m_layer_counts;

	public:
		// i_neural_net interface
		virtual size_t neuron_count() const override;
		virtual size_t input_count() const override;
		virtual size_t output_count() const override;
		virtual size_t hidden_count() const override;

		virtual output run(input const& in) override;

		virtual bool provides_access(AccessOptions opt) const override;

		virtual std::unique_ptr< i_neuron_access > neuron_access() const override;

		virtual std::unique_ptr< i_connection_access > connection_access() const override;

		virtual std::unique_ptr< i_layers > layer_access() const override;

		virtual activity_state current_activity_state() const override;

	private:
		class neuron_accessor;
		class connection_accessor;
	};

}


#endif

