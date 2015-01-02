// fc_rnn.h

#ifndef __WB_MLF_FC_RNN_H
#define __WB_MLF_FC_RNN_H

#include "../../interface/neuralnet.h"
#include "../../interface/neuron_type.h"
#include "../../interface/activation_functions.h"
#include "../../interface/neuron_filter.h"
#include "../../interface/layer_filter.h"
#include "../../interface/iterator_range.h"

#include "util/bimap.h"

#include <Eigen/Dense>

#include <vector>
#include <map>
#include <utility>
#include <memory>


namespace nnet {

	class connection_data;

	class fc_rnn:
		public i_neuralnet
	{
	public:
		fc_rnn();

	public:
		typedef std::vector< size_t > layer_counts_t;

	private:
		typedef bimap< connection_id, std::pair< neuron_id, neuron_id > > connection_map_t;

	public:
		size_t neuron_count(neuron_filter const& filter) const;
		neuron_range get_neurons(layer_filter const& lfilter = layer_filter::Any, neuron_filter const& nfilter = neuron_filter::Any) const;
		layer_data neuron_layer(neuron_id id) const;

		connection_id get_connection_id(neuron_id src, neuron_id dst) const;
		connection_range get_connections(
			layer_filter const& src_lfilter = layer_filter::Any,
			layer_filter const& dst_lfilter = layer_filter::Any,
			neuron_filter const& src_nfilter = neuron_filter::Any,
			neuron_filter const& dst_nfilter = neuron_filter::Any
			) const;

		inline size_t num_connections() const
		{
			return m_connections.size();
		}

		inline size_t input_layer_count() const
		{
			return vInput.size();
		}

		void reset();
		void create(size_t num_input, size_t num_hidden, size_t num_output, value_t bias = 1.0);
		void set_activation_function_hidden(activation_function fn);
		void set_activation_function_output(activation_function fn);
		void set_weight(connection_id neuron, value_t weight);
		bool load_weights(weight_array_t const& weights);
		output execute(input const& in);

	private:
		neuron_data as_neuron_data(neuron_id id) const;
		value_t connection_weight(connection_id id) const;
		value_t& connection_weight(connection_id id);
		connection_data as_connection_data(connection_id id) const;
		size_t input_offset() const;
		size_t hidden_offset() const;
		size_t output_offset() const;
		size_t layer_offset(LayerType ly) const;
		size_t layer_count(LayerType ly) const;

		size_t calculate_connection_count() const;
		void initialize_connection_map();

	private:
		// Neuron activation vectors
		Eigen::VectorXd vInput;			// includes a single bias neuron
		Eigen::VectorXd vHidden;
		Eigen::VectorXd vOutput;
		// Weight matrices
		Eigen::MatrixXd wmInput;		// from input to hidden
		Eigen::MatrixXd wmHidden;		// from hidden to hidden
		Eigen::MatrixXd wmOutput;		// from both input and hidden to output

		activation_function af_hidden;
		activation_function af_output;

		connection_map_t m_connections;

	public:
		// i_neural_net interface
		virtual std::string network_type_name() const override;

		virtual size_t neuron_count() const override;
		virtual size_t input_count() const override;
		virtual size_t output_count() const override;
		virtual size_t hidden_count() const override;

		virtual void reset_state() override;
		virtual output run(input const& in) override;

		virtual bool provides_access(AccessOptions opt) const override;

		virtual std::unique_ptr< i_neuron_access > neuron_access() const override;
		virtual std::unique_ptr< i_connection_access > connection_access() const override;
		virtual std::unique_ptr< i_layers > layer_access() const override;
		virtual activity_state current_activity_state() const override;
		virtual std::unique_ptr< i_modifiable > modifiable() override;

	private:
		class neuron_accessor;
		class connection_accessor;
		class modifier;
	};

}


#endif

