// fixed_neural_net.h

#ifndef __FIXED_NEURAL_NET_H
#define __FIXED_NEURAL_NET_H

#include <vector>
#include <cassert>


// TODO: Perhaps want to separate Solution Impl Type (first part above) from Genetic Encoding, so we can specify a Solution Impl Type
// (as above for example) and then specify multiple possible genetic encodings for it.

template <
	typename System,
//	typename Input
	typename Solution
>
class fixed_neural_net
{
public:
	typedef System system_t;
	typedef Solution solution;
//	typedef Input input_t;

	struct genome: public std::vector< double >
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

	typedef double genome_diff_t;

	static inline genome_diff_t genome_difference(genome const& gn1, genome const& gn2)
	{
		assert(gn1.length() == gn2.length());

		genome_diff_t diff = 0.0;
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

	static inline size_t get_genome_length()
	{
		solution temp;
		return temp.nn.get_total_connections();
	}

	template < typename RGen >
	static inline genome generate_random_genome(RGen& rgen)
	{
		solution temp;
		size_t num_weights = get_genome_length();
		std::vector< FANN::connection > conns(num_weights);
		temp.nn.get_connection_array(&conns[0]);

		std::map< unsigned int, size_t > input_degree_map;
		for(size_t i = 0; i < num_weights; ++i)
		{
			++input_degree_map[conns[i].to_neuron];
		}

		boost::random::uniform_real_distribution<> rdist(-1.0, 1.0);
		genome gn;
		for(size_t i = 0; i < num_weights; ++i)
		{
			gn.push_back(
				rdist(rgen) / std::sqrt(input_degree_map.at(conns[i].to_neuron))
				);
		}
		return gn;
	}

	static solution decode_genome(genome const& gn)
	{
		size_t const NumWeights = gn.length();
		assert(NumWeights == get_genome_length());
		
		solution sol;
		std::vector< FANN::connection > conns(NumWeights);
		sol.nn.get_connection_array(&conns[0]);
		for(size_t c = 0; c < NumWeights; ++c)
		{
			conns[c].weight = gn[c];
		}
		sol.nn.set_weight_array(&conns[0], NumWeights);

		return sol;
	}
};


#endif


