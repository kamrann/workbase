// genetic_population.h

#ifndef __WB_GA_GENETIC_POPULATION_H
#define __WB_GA_GENETIC_POPULATION_H

#include "genome.h"
#include "genalg_common.h"
#include "objective_value.h"

#include <vector>
#include <set>


namespace ga {

	class genetic_population
	{
	public:
		struct individual
		{
			genome gn;
			objective_value obj_val;
			fitness_value_t fitness;

			individual():
				gn{},
				obj_val{},
				fitness{}
			{}

			struct fitness_compare
			{
				inline bool operator() (individual const& lhs, individual const& rhs)
				{
					return lhs.fitness > rhs.fitness;
				}
			};
		};

		typedef std::vector< individual > population_t;
		typedef population_t::iterator iterator;
		typedef population_t::const_iterator const_iterator;

	public:
		inline size_t size() const
		{
			return m_individuals.size();
		}

		inline void resize(size_t sz)
		{
			m_individuals.resize(sz);
		}

		inline individual& operator[] (int i)
		{
			return m_individuals[i];
		}

		inline individual const& operator[] (int i) const
		{
			return m_individuals[i];
		}

		inline iterator begin()
		{
			return std::begin(m_individuals);
		}

		inline iterator end()
		{
			return std::end(m_individuals);
		}

		inline const_iterator begin() const
		{
			return std::begin(m_individuals);
		}

		inline const_iterator end() const
		{
			return std::end(m_individuals);
		}

		inline const_iterator cbegin() const
		{
			return m_individuals.cbegin();
		}

		inline const_iterator cend() const
		{
			return m_individuals.cend();
		}

		fitness_value_t max_fitness() const
		{
			fitness_value_t highest = -std::numeric_limits< double >::max();
			for(size_t i = 0; i < size(); ++i)
			{
				auto f = m_individuals[i].fitness;
				if(f > highest)
				{
					highest = f;
				}
			}
			return highest;
		}

		std::set< size_t > fittest() const
		{
			std::set< size_t > result;
			fitness_value_t highest = -std::numeric_limits< double >::max();
			for(size_t i = 0; i < size(); ++i)
			{
				auto f = m_individuals[i].fitness;
				if(f > highest)
				{
					highest = f;
					result.clear();
					result.insert(i);
				}
				else if(f == highest)
				{
					result.insert(i);
				}
			}
			return result;
		}

	private:
		population_t m_individuals;
	};

}


#endif


