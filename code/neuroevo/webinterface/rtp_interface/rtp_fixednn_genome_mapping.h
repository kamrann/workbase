// rtp_fixednn_genome_mapping.h

#ifndef __NE_RTP_FIXEDNN_GENOME_MAPPING_H
#define __NE_RTP_FIXEDNN_GENOME_MAPPING_H

#include "rtp_genome.h"
#include "systems/rtp_agent.h"
#include "systems/sat/scenarios/full_stop/agents.h"	// TODO: TEMP

#include <vector>
#include <cassert>


class fixednn_genome_mapping: public i_genome_mapping
{
public:
	fixednn_genome_mapping(std::vector< size_t > const& layer_counts)
	{
		m_layer_neuron_counts = layer_counts;
	}

	fixednn_genome_mapping(size_t layers, size_t inputs, size_t outputs, size_t per_hidden_layer = 1)
	{
		m_layer_neuron_counts.resize(layers, per_hidden_layer);
		m_layer_neuron_counts[0] = inputs;
		m_layer_neuron_counts[layers - 1] = outputs;
	}

	struct genome:
		public i_genome,
		public std::vector< double >
	{
		inline size_t length() const
		{
			return size();
		}

		// Lexicographical ordering
		inline bool operator< (genome const& other) const
		{
			size_t const len = length();
			for(size_t i = 0; i < len; ++i)
			{
				if((*this)[i] < other[i])
				{
					return true;
				}
				else if((*this)[i] > other[i])
				{
					return false;
				}
			}
			return false;
		}

		//friend std::ostream& operator<< (std::ostream& out, genome const& gn);
	};

	virtual i_genome_mapping::genome_diff_t genome_difference(i_genome const* ign1, i_genome const* ign2)
	{
		genome const& gn1 = *(genome const*)ign1;
		genome const& gn2 = *(genome const*)ign2;

		assert(gn1.length() == gn2.length());

		i_genome_mapping::genome_diff_t diff = 0.0;
		size_t const len = gn1.length();
		for(size_t i = 0; i < len; ++i)
		{
			double gene_diff = gn2[i] - gn1[i];
			diff += gene_diff * gene_diff;
		}
		return std::sqrt(diff);

		// TODO: Some kind of normalization?
		// Weight values are unbounded, but seems like a weight change may be more volatile in one gene than another.
		// This could lead to genes where the precise value was not so important to the network output having a large effect
		// on the difference measure, which seems bad.
	}

	virtual size_t get_genome_length()
	{
		// TODO: Should be calcuable directly from m_layer_neuron_counts, assuming fully connected mlp
		FANN::neural_net nn_temp;
		nn_temp.create_standard_array(m_layer_neuron_counts.size(), &m_layer_neuron_counts[0]);
		return nn_temp.get_total_connections();
	}

	virtual i_genome* generate_random_genome(rgen_t& rgen)
	{
		genome* gn = new genome();
		boost::random::uniform_real_distribution<> rdist(-1.0, 1.0);
		for(size_t layer = 1; layer < m_layer_neuron_counts.size(); ++layer)
		{
			size_t const input_degree = m_layer_neuron_counts[layer - 1] + 1;
			size_t const num_connections_to_layer = m_layer_neuron_counts[layer] * input_degree;
			for(size_t c = 0; c < num_connections_to_layer; ++c)
			{
				double const weight = rdist(rgen) / std::sqrt(input_degree);
				gn->push_back(weight);
			}
		}
		return gn;
	}

	// TODO: Feel like would be nicer to return a newly constructed agent, but that would require some kind of agent factory, or runtime traits class.
	virtual void decode_genome(i_genome const* ign, i_agent* ia)
	{
		genome const* gn = (genome const*)ign;
		rtp_sat::generic_mlp_agent* agent = dynamic_cast<rtp_sat::generic_mlp_agent*>(ia);
		//
		size_t const NumWeights = gn->length();
		assert(NumWeights == get_genome_length());
		//
		agent->set_weights(*gn);
	}

private:
	std::vector< size_t > m_layer_neuron_counts;
};


#endif

