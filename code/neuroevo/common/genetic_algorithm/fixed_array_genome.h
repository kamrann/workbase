// fixed_array_genome.h

#ifndef __WB_GA_FIXED_ARRAY_GENOME_H
#define __WB_GA_FIXED_ARRAY_GENOME_H

#include "genalg_common.h"
#include "genome.h"

#include <vector>


namespace ga {

	/*
	An base implementation of a genome which is a fixed length array of genes.
	*/
	template < typename Gene >
	class fixed_array_genome
	{
	public:
		typedef Gene gene_t;
		typedef std::vector< gene_t > gene_array_t;
		typedef typename gene_array_t::iterator iterator;
		typedef typename gene_array_t::const_iterator const_iterator;

	public:
		fixed_array_genome()
		{}
		fixed_array_genome(size_t sz):
			m_genes(sz, gene_t{})
		{}

	public:
		inline size_t size() const
		{
			return m_genes.size();
		}

		inline gene_t const& operator[] (int i) const
		{
			return m_genes[i];
		}

		inline gene_t& operator[] (int i)
		{
			return m_genes[i];
		}

		inline iterator begin()
		{
			return std::begin(m_genes);
		}

		inline iterator end()
		{
			return std::end(m_genes);
		}

		inline const_iterator begin() const
		{
			return std::begin(m_genes);
		}

		inline const_iterator end() const
		{
			return std::end(m_genes);
		}

		inline const_iterator cbegin() const
		{
			return m_genes.cbegin();
		}

		inline const_iterator cend() const
		{
			return m_genes.cend();
		}

		inline void resize(size_t sz)
		{
			return m_genes.resize(sz);
		}

		template < typename InIt >
		inline void emplace(iterator pos, InIt first, InIt last)
		{
			std::copy(first, last, pos);
		}

	protected:
		gene_array_t m_genes;
	};

	/*
	Default crossover operator - extremely simple. See crossover.h in ga for more generic implementation.
	*/
	template < typename Genome >
	struct basic_array_crossover
	{
		typedef Genome genome_t;

		basic_array_crossover(size_t length):
			dist(0, length)
		{}

		genome_t operator() (genome_t const& p1, genome_t const& p2, rgen_t& rgen)
		{
			assert(p1.size() == p2.size());

			auto const length = p1.size();
			genome_t result;
			result.resize(length);
			auto const pos = dist(rgen);
			// TODO: This is assuming that any individual could take place of either p1 or p2, if there will be bias
			// (ie. the first individual in a population will always be p1, never p2) then the resulting offspring
			// will also be biased in the genes they inherit.
			result.emplace(std::begin(result), std::begin(p1), std::begin(p1) + pos);
			result.emplace(std::begin(result) + pos, std::begin(p2) + pos, std::end(p2));
			return result;
		}

		std::uniform_int_distribution< unsigned int > dist;
	};
	/*
	template < typename Gene >
	genome basic_array_crossover(genome const& p1, genome const& p2, rgen_t& rgen)
	{
		return std::make_unique< fixed_array_genome< Gene > >(
			basic_array_crossover(
			p1.as< fixed_array_genome< Gene > >(),
			p2.as< fixed_array_genome< Gene > >(),
			rgen)
			);
	}
	*/

}


#endif


