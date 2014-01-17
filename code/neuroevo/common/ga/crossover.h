// crossover.h

#ifndef __CROSSOVER_H
#define __CROSSOVER_H

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <array>
#include <algorithm>


// Simple crossover functor taking NumParents genomes, all of the same length, and randomly taking a contiguous segment from each
// to contribute to the offspring genome.
template < typename Genome, size_t NumParents = 2, typename RGen = boost::random::mt19937 >
struct basic_crossover
{
	typedef Genome genome_t;

	size_t genome_length;

	//boost::random::uniform_int_distribution<> rdist;
	RGen& rgen;

	struct shuffle_ftr
	{
		RGen& rgen;

		shuffle_ftr(RGen& rg): rgen(rg)
		{}

		inline int operator() (int i) const
		{
			return boost::random::uniform_int_distribution<>(0, i - 1)(rgen);
		}
	};

	basic_crossover(size_t gn_len, RGen& rg): genome_length(gn_len), /*rdist(0, gn_len),*/ rgen(rg)
	{}

	inline size_t get_num_parents() const
	{
		return NumParents;
	}

	template < typename InputIterator, typename OutputIterator >
	inline void operator() (InputIterator parents, OutputIterator offspring)
	{
		boost::random::uniform_int_distribution<> rdist(0, genome_length);

		// Split the chromosome space randomly into NumParents segments
		std::array< size_t, NumParents + 1 > cx_points = { 0 };
		cx_points[NumParents] = genome_length;
		for(size_t i = 0; i < NumParents - 1; ++i)
		{
			cx_points[1 + i] = rdist(rgen);
		}
		std::sort(cx_points.begin() + 1, cx_points.begin() + NumParents);

		// Now randomise the parent order
		// NOTE: Think this will be needed for certain implementations to prevent an individual in the parent population
		// from contributing genes from the same end of its chromosome to all its offspring.
		std::array< size_t, NumParents > p_order = { 0 };
		for(size_t i = 0; i < NumParents; ++i)
		{
			p_order[i] = i;
		}
		std::random_shuffle(p_order.begin(), p_order.end(), shuffle_ftr(rgen));

		// TODO: Should this be done here, or higher up? eg. when creating next_gen in gen_alg::epoch()
		offspring->genome.resize(genome_length);

		// Now copy over the corresponding segment of genome from each parent to the child
		for(size_t i = 0; i < NumParents; ++i)
		{
			std::copy(
				parents[p_order[i]]->genome.begin() + cx_points[i],
				parents[p_order[i]]->genome.begin() + cx_points[i + 1],
				offspring->genome.begin() + cx_points[i]
			);
		}
	}
};


#endif


