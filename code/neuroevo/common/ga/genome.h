// genome.h

#ifndef __GENOME_H
#define __GENOME_H

#include <boost/random/uniform_real_distribution.hpp>

#include <vector>
#include <ostream>


struct fixed_real_genome
{
	typedef std::vector< double >			gene_array_t;
	typedef gene_array_t::iterator			iterator;
	typedef gene_array_t::const_iterator	const_iterator;

	gene_array_t genes;

	inline iterator begin()
	{
		return genes.begin();
	}

	inline const_iterator begin() const
	{
		return genes.begin();
	}

	inline iterator end()
	{
		return genes.end();
	}

	inline const_iterator end() const
	{
		return genes.end();
	}

	inline double& operator[] (int const i)
	{
		return genes[i];
	}

	inline double const& operator[] (int const i) const
	{
		return genes[i];
	}

	inline size_t length() const
	{
		return genes.size();
	}

	inline void resize(size_t len)
	{
		genes.clear();
		genes.resize(len, 0.0);
	}

	// TODO: This may eventually want to be moved to a higher level so that can be made problem specific
	template < typename RGen >
	inline void set_random(size_t const length, RGen& rgen)
	{
		boost::random::uniform_real_distribution<> rdist(-1.0, 1.0);	// TODO:
		genes.resize(length);
		for(double& g: genes)
		{
			g = rdist(rgen);
		}
	}

	// Lexicographical ordering
	inline bool operator< (fixed_real_genome const& other) const
	{
		for(size_t i = 0; i < length(); ++i)
		{
			if(genes[i] < other[i])
			{
				return true;
			}
			else if(genes[i] > other[i])
			{
				return false;
			}
		}
		return false;
	}

	friend std::ostream& operator<< (std::ostream& out, fixed_real_genome const& gn);
};


inline std::ostream& operator<< (std::ostream& out, fixed_real_genome const& gn)
{
	out.precision(2);
	std::fixed(out);

	out << "{ ";
	for(double d: gn.genes)
	{
		out << d << " ";
	}
	out << "}";
	return out;
}


#endif


